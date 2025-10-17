/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2025 LXQt team
 * Authors:
 *   LXQt team
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "diskiconcache.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Default maximum cache size: 512MB
static const qint64 DEFAULT_MAX_CACHE_SIZE = 512 * 1024 * 1024;

// Static members
DiskIconCache *DiskIconCache::s_instance = nullptr;
QMutex DiskIconCache::s_instanceMutex;

DiskIconCache* DiskIconCache::instance()
{
    if (!s_instance) {
        QMutexLocker locker(&s_instanceMutex);
        if (!s_instance) {
            s_instance = new DiskIconCache();
        }
    }
    return s_instance;
}

DiskIconCache::DiskIconCache(QObject *parent)
    : QObject(parent)
    , m_maxCacheSize(DEFAULT_MAX_CACHE_SIZE)
    , m_enabled(true)
{
    initializeCacheDir();
    loadCacheIndex();

    qDebug() << "DiskIconCache initialized:"
             << "dir=" << m_cacheDir
             << "max_size=" << (m_maxCacheSize / 1024 / 1024) << "MB"
             << "items=" << m_cacheIndex.size();
}

DiskIconCache::~DiskIconCache()
{
    saveCacheIndex();
}

void DiskIconCache::initializeCacheDir()
{
    // Use XDG cache directory
    QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheRoot.isEmpty()) {
        cacheRoot = QDir::homePath() + QLatin1String("/.cache");
    }

    m_cacheDir = cacheRoot + QLatin1String("/libqtxdg/icon-cache");

    // Create directory if not exists
    QDir dir;
    if (!dir.exists(m_cacheDir)) {
        if (dir.mkpath(m_cacheDir)) {
            qDebug() << "Created disk cache directory:" << m_cacheDir;
        } else {
            qWarning() << "Failed to create disk cache directory:" << m_cacheDir;
            m_enabled = false;
        }
    }
}

QString DiskIconCache::getFilePath(const QString &cacheKey) const
{
    // Use MD5 hash as filename to avoid invalid characters
    QByteArray hash = QCryptographicHash::hash(
        cacheKey.toUtf8(),
        QCryptographicHash::Md5
    );
    QString filename = QString::fromLatin1(hash.toHex()) + QLatin1String(".png");

    return m_cacheDir + QLatin1String("/") + filename;
}

QImage DiskIconCache::loadFromDisk(const QString &cacheKey)
{
    if (!m_enabled) {
        return QImage();
    }

    QMutexLocker locker(&m_mutex);

    // Check if in index
    auto it = m_cacheIndex.find(cacheKey);
    if (it == m_cacheIndex.end()) {
        return QImage();  // Not in cache
    }

    CacheEntry &entry = it.value();
    QString filePath = entry.filePath;

    // Check if file exists
    if (!QFile::exists(filePath)) {
        qDebug() << "Disk cache file missing:" << cacheKey;
        m_cacheIndex.erase(it);
        return QImage();
    }

    // Load image
    QImage image(filePath);
    if (image.isNull()) {
        qWarning() << "Failed to load disk cache image:" << filePath;
        // Remove corrupted entry
        QFile::remove(filePath);
        m_cacheIndex.erase(it);
        return QImage();
    }

    // Update access time
    entry.lastAccessed = QDateTime::currentDateTime();

    qDebug() << "Disk cache HIT:" << cacheKey;
    return image;
}

bool DiskIconCache::saveToDisk(const QString &cacheKey, const QImage &image)
{
    if (!m_enabled || image.isNull()) {
        return false;
    }

    QMutexLocker locker(&m_mutex);

    QString filePath = getFilePath(cacheKey);

    // Save as PNG (compressed)
    if (!image.save(filePath, "PNG")) {
        qWarning() << "Failed to save disk cache image:" << filePath;
        return false;
    }

    // Get file size
    QFileInfo fileInfo(filePath);
    qint64 fileSize = fileInfo.size();

    // Add to index
    CacheEntry entry;
    entry.filePath = filePath;
    entry.fileSize = fileSize;
    entry.lastAccessed = QDateTime::currentDateTime();

    m_cacheIndex[cacheKey] = entry;

    qDebug() << "Disk cache SAVE:" << cacheKey
             << "size=" << (fileSize / 1024) << "KB";

    // Check if eviction needed
    checkAndEvict();

    return true;
}

void DiskIconCache::clearCache()
{
    QMutexLocker locker(&m_mutex);

    qDebug() << "Clearing disk cache...";

    // Remove all files
    for (const CacheEntry &entry : m_cacheIndex) {
        QFile::remove(entry.filePath);
    }

    m_cacheIndex.clear();
    saveCacheIndex();

    qDebug() << "Disk cache cleared";
}

qint64 DiskIconCache::getCacheSize() const
{
    QMutexLocker locker(&m_mutex);

    qint64 totalSize = 0;
    for (const CacheEntry &entry : m_cacheIndex) {
        totalSize += entry.fileSize;
    }

    return totalSize;
}

int DiskIconCache::getCacheCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_cacheIndex.size();
}

void DiskIconCache::setEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_enabled = enabled;
    qDebug() << "Disk cache" << (enabled ? "enabled" : "disabled");
}

bool DiskIconCache::isEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_enabled;
}

void DiskIconCache::setMaxCacheSize(qint64 bytes)
{
    QMutexLocker locker(&m_mutex);
    m_maxCacheSize = bytes;
    qDebug() << "Disk cache max size set to" << (bytes / 1024 / 1024) << "MB";
    checkAndEvict();
}

qint64 DiskIconCache::maxCacheSize() const
{
    QMutexLocker locker(&m_mutex);
    return m_maxCacheSize;
}

void DiskIconCache::checkAndEvict()
{
    // Calculate current size
    qint64 currentSize = 0;
    for (const CacheEntry &entry : m_cacheIndex) {
        currentSize += entry.fileSize;
    }

    if (currentSize > m_maxCacheSize) {
        qint64 bytesToFree = currentSize - m_maxCacheSize;
        qDebug() << "Disk cache full, evicting"
                 << (bytesToFree / 1024 / 1024) << "MB";
        evictLRU(bytesToFree);
    }
}

void DiskIconCache::evictLRU(qint64 bytesToFree)
{
    // Collect all entries
    QList<QString> keys;
    for (auto it = m_cacheIndex.begin(); it != m_cacheIndex.end(); ++it) {
        keys.append(it.key());
    }

    // Sort by last accessed time (oldest first)
    std::sort(keys.begin(), keys.end(), [this](const QString &a, const QString &b) {
        return m_cacheIndex[a].lastAccessed < m_cacheIndex[b].lastAccessed;
    });

    // Evict oldest entries
    qint64 freedBytes = 0;
    int evictedCount = 0;

    for (const QString &key : keys) {
        if (freedBytes >= bytesToFree) {
            break;
        }

        const CacheEntry &entry = m_cacheIndex[key];
        freedBytes += entry.fileSize;
        evictedCount++;

        // Remove file
        QFile::remove(entry.filePath);

        qDebug() << "Evicted disk cache entry:" << key
                 << "freed=" << (entry.fileSize / 1024) << "KB";

        m_cacheIndex.remove(key);
    }

    qDebug() << "Disk cache LRU eviction complete:"
             << "evicted=" << evictedCount
             << "freed=" << (freedBytes / 1024 / 1024) << "MB";
}

void DiskIconCache::loadCacheIndex()
{
    QString indexFile = m_cacheDir + QLatin1String("/cache-index.json");

    QFile file(indexFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No disk cache index found, scanning directory...";
        updateCacheIndex();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid disk cache index format";
        updateCacheIndex();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray entries = root[QStringLiteral("entries")].toArray();

    for (const QJsonValue &value : entries) {
        QJsonObject entryObj = value.toObject();

        QString key = entryObj[QStringLiteral("key")].toString();
        QString filePath = entryObj[QStringLiteral("filePath")].toString();
        qint64 fileSize = entryObj[QStringLiteral("fileSize")].toVariant().toLongLong();
        QDateTime lastAccessed = QDateTime::fromString(
            entryObj[QStringLiteral("lastAccessed")].toString(),
            Qt::ISODate
        );

        // Verify file exists
        if (QFile::exists(filePath)) {
            CacheEntry entry;
            entry.filePath = filePath;
            entry.fileSize = fileSize;
            entry.lastAccessed = lastAccessed;
            m_cacheIndex[key] = entry;
        }
    }

    qDebug() << "Loaded disk cache index:" << m_cacheIndex.size() << "entries";
}

void DiskIconCache::saveCacheIndex()
{
    QString indexFile = m_cacheDir + QLatin1String("/cache-index.json");

    QJsonArray entries;
    for (auto it = m_cacheIndex.begin(); it != m_cacheIndex.end(); ++it) {
        const CacheEntry &entry = it.value();

        QJsonObject entryObj;
        entryObj[QStringLiteral("key")] = it.key();
        entryObj[QStringLiteral("filePath")] = entry.filePath;
        entryObj[QStringLiteral("fileSize")] = QJsonValue::fromVariant(entry.fileSize);
        entryObj[QStringLiteral("lastAccessed")] = entry.lastAccessed.toString(Qt::ISODate);

        entries.append(entryObj);
    }

    QJsonObject root;
    root[QStringLiteral("entries")] = entries;
    root[QStringLiteral("version")] = 1;

    QJsonDocument doc(root);

    QFile file(indexFile);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save disk cache index:" << indexFile;
        return;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved disk cache index:" << m_cacheIndex.size() << "entries";
}

void DiskIconCache::updateCacheIndex()
{
    m_cacheIndex.clear();

    QDirIterator it(m_cacheDir, QStringList() << QStringLiteral("*.png"), QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);

        // We don't have the original cache key, so skip these orphaned files
        // They will be naturally replaced as cache is used
        qDebug() << "Found orphaned cache file (will be replaced):" << filePath;
    }

    saveCacheIndex();
}
