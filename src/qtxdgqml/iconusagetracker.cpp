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

#include "iconusagetracker.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutexLocker>
#include <QDebug>
#include <algorithm>

// Auto-save threshold: save stats after every N accesses
static const int AUTO_SAVE_THRESHOLD = 50;

// Static members
IconUsageTracker *IconUsageTracker::s_instance = nullptr;
QMutex IconUsageTracker::s_instanceMutex;

IconUsageTracker* IconUsageTracker::instance()
{
    if (!s_instance) {
        QMutexLocker locker(&s_instanceMutex);
        if (!s_instance) {
            s_instance = new IconUsageTracker();
        }
    }
    return s_instance;
}

IconUsageTracker::IconUsageTracker(QObject *parent)
    : QObject(parent)
    , m_enabled(true)
    , m_dirty(false)
{
    initializeStatsFile();
    loadStats();

    qDebug() << "IconUsageTracker initialized:"
             << "stats_file=" << m_statsFilePath
             << "tracked_icons=" << m_usageStats.size();
}

IconUsageTracker::~IconUsageTracker()
{
    if (m_dirty) {
        saveStats();
    }
}

void IconUsageTracker::initializeStatsFile()
{
    // Use XDG cache directory
    QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheRoot.isEmpty()) {
        cacheRoot = QDir::homePath() + QLatin1String("/.cache");
    }

    QString cacheDir = cacheRoot + QLatin1String("/libqtxdg");
    m_statsFilePath = cacheDir + QLatin1String("/icon-usage.json");

    // Create directory if not exists
    QDir dir;
    if (!dir.exists(cacheDir)) {
        if (!dir.mkpath(cacheDir)) {
            qWarning() << "IconUsageTracker: Failed to create stats directory:" << cacheDir;
            m_enabled = false;
        }
    }
}

QString IconUsageTracker::generateKey(const QString &iconName, int size, int state) const
{
    // Format: "iconName@size_state"
    return QString::fromLatin1("%1@%2_%3")
        .arg(iconName)
        .arg(size)
        .arg(state);
}

void IconUsageTracker::recordAccess(const QString &iconName, int size, int state)
{
    if (!m_enabled || iconName.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    QString key = generateKey(iconName, size, state);
    QDateTime now = QDateTime::currentDateTime();

    auto it = m_usageStats.find(key);
    if (it != m_usageStats.end()) {
        // Existing entry - update
        it->accessCount++;
        it->lastAccessed = now;
    } else {
        // New entry - create
        UsageEntry entry;
        entry.iconName = iconName;
        entry.size = size;
        entry.accessCount = 1;
        entry.lastAccessed = now;
        entry.firstAccessed = now;
        m_usageStats[key] = entry;
    }

    m_dirty = true;

    // Auto-save every N accesses to avoid losing data
    static int accessCounter = 0;
    if (++accessCounter >= AUTO_SAVE_THRESHOLD) {
        saveStats();
        accessCounter = 0;
    }
}

QStringList IconUsageTracker::getTopUsed(int count) const
{
    QMutexLocker locker(&m_mutex);

    // Collect all entries
    QList<UsageEntry> entries;
    for (auto it = m_usageStats.begin(); it != m_usageStats.end(); ++it) {
        entries.append(it.value());
    }

    // Sort by access count (descending)
    std::sort(entries.begin(), entries.end(),
        [](const UsageEntry &a, const UsageEntry &b) {
            return a.accessCount > b.accessCount;
        });

    // Extract top N icon names
    QStringList result;
    for (int i = 0; i < qMin(count, entries.size()); ++i) {
        result.append(entries[i].iconName);
    }

    return result;
}

QStringList IconUsageTracker::getRecentlyUsed(int count) const
{
    QMutexLocker locker(&m_mutex);

    // Collect all entries
    QList<UsageEntry> entries;
    for (auto it = m_usageStats.begin(); it != m_usageStats.end(); ++it) {
        entries.append(it.value());
    }

    // Sort by last accessed time (newest first)
    std::sort(entries.begin(), entries.end(),
        [](const UsageEntry &a, const UsageEntry &b) {
            return a.lastAccessed > b.lastAccessed;
        });

    // Extract most recent N icon names
    QStringList result;
    for (int i = 0; i < qMin(count, entries.size()); ++i) {
        result.append(entries[i].iconName);
    }

    return result;
}

QHash<QString, IconUsageTracker::UsageEntry> IconUsageTracker::getAllStats() const
{
    QMutexLocker locker(&m_mutex);
    return m_usageStats;
}

int IconUsageTracker::getTotalIconCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_usageStats.size();
}

qint64 IconUsageTracker::getTotalAccessCount() const
{
    QMutexLocker locker(&m_mutex);

    qint64 total = 0;
    for (auto it = m_usageStats.begin(); it != m_usageStats.end(); ++it) {
        total += it->accessCount;
    }

    return total;
}

void IconUsageTracker::clearStats()
{
    QMutexLocker locker(&m_mutex);

    qDebug() << "IconUsageTracker: Clearing stats...";

    m_usageStats.clear();
    m_dirty = true;
    saveStats();

    qDebug() << "IconUsageTracker: Stats cleared";
}

void IconUsageTracker::setEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_enabled = enabled;
    qDebug() << "IconUsageTracker" << (enabled ? "enabled" : "disabled");
}

bool IconUsageTracker::isEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_enabled;
}

void IconUsageTracker::loadStats()
{
    QFile file(m_statsFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "IconUsageTracker: No existing stats file, starting fresh";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "IconUsageTracker: Invalid stats file format";
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray entries = root[QStringLiteral("entries")].toArray();

    int loadedCount = 0;
    for (const QJsonValue &value : entries) {
        QJsonObject entryObj = value.toObject();

        QString key = entryObj[QStringLiteral("key")].toString();
        QString iconName = entryObj[QStringLiteral("iconName")].toString();
        int size = entryObj[QStringLiteral("size")].toInt();
        int accessCount = entryObj[QStringLiteral("accessCount")].toInt();
        QDateTime lastAccessed = QDateTime::fromString(
            entryObj[QStringLiteral("lastAccessed")].toString(),
            Qt::ISODate
        );
        QDateTime firstAccessed = QDateTime::fromString(
            entryObj[QStringLiteral("firstAccessed")].toString(),
            Qt::ISODate
        );

        UsageEntry entry;
        entry.iconName = iconName;
        entry.size = size;
        entry.accessCount = accessCount;
        entry.lastAccessed = lastAccessed;
        entry.firstAccessed = firstAccessed;

        m_usageStats[key] = entry;
        loadedCount++;
    }

    qDebug() << "IconUsageTracker: Loaded" << loadedCount << "usage entries from disk";
}

void IconUsageTracker::saveStats()
{
    if (!m_dirty) {
        return;  // No changes to save
    }

    QJsonArray entries;
    for (auto it = m_usageStats.begin(); it != m_usageStats.end(); ++it) {
        const UsageEntry &entry = it.value();

        QJsonObject entryObj;
        entryObj[QStringLiteral("key")] = it.key();
        entryObj[QStringLiteral("iconName")] = entry.iconName;
        entryObj[QStringLiteral("size")] = entry.size;
        entryObj[QStringLiteral("accessCount")] = entry.accessCount;
        entryObj[QStringLiteral("lastAccessed")] = entry.lastAccessed.toString(Qt::ISODate);
        entryObj[QStringLiteral("firstAccessed")] = entry.firstAccessed.toString(Qt::ISODate);

        entries.append(entryObj);
    }

    QJsonObject root;
    root[QStringLiteral("entries")] = entries;
    root[QStringLiteral("version")] = 1;

    QJsonDocument doc(root);

    QFile file(m_statsFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "IconUsageTracker: Failed to save stats to" << m_statsFilePath;
        return;
    }

    file.write(doc.toJson());
    file.close();

    m_dirty = false;

    qDebug() << "IconUsageTracker: Saved" << m_usageStats.size() << "usage entries to disk";
}
