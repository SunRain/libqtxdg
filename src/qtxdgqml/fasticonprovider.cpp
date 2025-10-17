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

#include "fasticonprovider.h"
#include "fasticonresponse.h"
#include "cachedtexturefactory.h"
#include "iconusagetracker.h"
#include <QMutexLocker>
#include <QDebug>
#include <QTimer>
#include <QtConcurrent>
#include "xdgicon.h"

// Default cache size: 128MB
static const int DEFAULT_CACHE_SIZE_MB = 128;

FastIconProvider::FastIconProvider()
    : QQuickAsyncImageProvider()
{
    // Set default cache size (128MB)
    setCacheSize(DEFAULT_CACHE_SIZE_MB);

    // Configure thread pool for async loading
    // Use half of available CPU cores, minimum 2, maximum 4
    int cores = QThread::idealThreadCount();
    int threadCount = qBound(2, cores / 2, 4);
    m_loaderPool.setMaxThreadCount(threadCount);

    qDebug() << "FastIconProvider initialized:"
             << "cache=" << DEFAULT_CACHE_SIZE_MB << "MB"
             << "threads=" << threadCount;

    // Trigger auto-preload if enabled (Stage 4.1.5)
    if (m_autoPreloadEnabled) {
        // Delay preload slightly to allow app startup to complete
        QTimer::singleShot(500, this, &FastIconProvider::triggerAutoPreload);
    }
}

FastIconProvider::~FastIconProvider()
{
    // Wait for all loading tasks to complete
    m_loaderPool.waitForDone();

    qDebug() << "FastIconProvider destroyed. Final stats:"
             << "hits=" << m_stats.hitCount
             << "misses=" << m_stats.missCount
             << "hit_rate=" << QString::number(m_stats.hitRate() * 100, 'f', 1) << "%";
}

QQuickImageResponse* FastIconProvider::requestImageResponse(
    const QString &id,
    const QSize &requestedSize)
{
    // Parse URL parameters from id
    // Expected format: iconName?size=24x24&fallback=default&state=0
    QString iconName = id;
    QString fallback;
    FastIconResponse::IconState state = FastIconResponse::Normal;

    // Split by '?' to separate icon name and parameters
    int paramStart = id.indexOf(QLatin1Char('?'));
    if (paramStart != -1) {
        iconName = id.left(paramStart);

        // Parse parameters
        QString params = id.mid(paramStart + 1);
        QStringList paramList = params.split(QLatin1Char('&'));

        for (const QString &param : paramList) {
            int eqPos = param.indexOf(QLatin1Char('='));
            if (eqPos == -1) continue;

            QString key = param.left(eqPos);
            QString value = param.mid(eqPos + 1);

            if (key == QLatin1String("fallback")) {
                fallback = value;
            } else if (key == QLatin1String("state")) {
                bool ok;
                int stateInt = value.toInt(&ok);
                if (ok && stateInt >= 0 && stateInt <= 3) {
                    state = static_cast<FastIconResponse::IconState>(stateInt);
                }
            }
            // Note: size parameter is ignored, we use requestedSize from QML
        }
    }

    // Determine final size (use default 32x32 if not specified)
    QSize finalSize = requestedSize;
    if (finalSize.isEmpty() || finalSize.width() <= 0 || finalSize.height() <= 0) {
        finalSize = QSize(32, 32);
    }

    // Create async response
    return new FastIconResponse(iconName, finalSize, fallback, state, this);
}

void FastIconProvider::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_imageCache.clear();
    qDebug() << "FastIconProvider cache cleared";
}

void FastIconProvider::setCacheSize(int megabytes)
{
    QMutexLocker locker(&m_cacheMutex);
    // Convert MB to bytes
    qint64 bytes = static_cast<qint64>(megabytes) * 1024 * 1024;
    m_imageCache.setMaxCost(bytes);
}

int FastIconProvider::cacheSize() const
{
    QMutexLocker locker(&m_cacheMutex);
    return m_imageCache.maxCost() / (1024 * 1024);
}

FastIconProvider::CacheStats FastIconProvider::cacheStats() const
{
    QMutexLocker locker(&m_statsMutex);
    CacheStats stats = m_stats;
    stats.cachedItems = m_imageCache.count();
    stats.cacheBytes = m_imageCache.totalCost();
    return stats;
}

void FastIconProvider::resetStats()
{
    QMutexLocker locker(&m_statsMutex);
    m_stats = CacheStats();
}

void FastIconProvider::setMaxThreadCount(int count)
{
    m_loaderPool.setMaxThreadCount(qBound(1, count, 8));
}

int FastIconProvider::maxThreadCount() const
{
    return m_loaderPool.maxThreadCount();
}

QString FastIconProvider::cacheKey(const QString &iconName, const QSize &size, int state) const
{
    return QStringLiteral("%1@%2x%3s%4")
        .arg(iconName)
        .arg(size.width())
        .arg(size.height())
        .arg(state);
}

QImage* FastIconProvider::getCachedImage(const QString &key)
{
    QMutexLocker locker(&m_cacheMutex);
    QImage *cached = m_imageCache.object(key);

    if (cached) {
        recordCacheHit();
        return new QImage(*cached);  // Return copy
    } else {
        recordCacheMiss();
        return nullptr;
    }
}

void FastIconProvider::putCachedImage(const QString &key, const QImage &image)
{
    QMutexLocker locker(&m_cacheMutex);

    // Calculate image byte size (width * height * bytes_per_pixel)
    int cost = image.sizeInBytes();

    // Insert into cache (QCache takes ownership)
    QImage *cached = new QImage(image);
    m_imageCache.insert(key, cached, cost);
}

void FastIconProvider::recordCacheHit()
{
    QMutexLocker locker(&m_statsMutex);
    m_stats.hitCount++;
    m_stats.totalCount++;
}

void FastIconProvider::recordCacheMiss()
{
    QMutexLocker locker(&m_statsMutex);
    m_stats.missCount++;
    m_stats.totalCount++;
}

// GPU cache statistics (Stage 3)
FastIconProvider::GpuCacheStats FastIconProvider::gpuCacheStats() const
{
    GpuCacheStats stats;
    stats.textureCount = CachedTextureFactory::gpuCacheCount();
    stats.gpuBytes = CachedTextureFactory::gpuCacheBytes();
    stats.reuseRate = CachedTextureFactory::textureReuseRate();
    return stats;
}

void FastIconProvider::clearGpuCache()
{
    CachedTextureFactory::clearGpuCache();
    qDebug() << "FastIconProvider: GPU cache cleared";
}

void FastIconProvider::resetGpuStats()
{
    CachedTextureFactory::resetStats();
    qDebug() << "FastIconProvider: GPU stats reset";
}

// Preload management (Stage 3.3)
QFuture<int> FastIconProvider::preloadIcons(const QStringList &iconNames,
                                            const QSize &size,
                                            int state)
{
    QMutexLocker locker(&m_preloadMutex);

    if (m_isPreloading) {
        qWarning() << "Preload already in progress, ignoring request";
        return QFuture<int>();
    }

    m_isPreloading = true;
    m_preloadCancelled.storeRelaxed(0);

    qDebug() << "Starting icon preload:"
             << "count=" << iconNames.size()
             << "size=" << size
             << "state=" << state;

    // Use QtConcurrent::run to execute in thread pool
    return QtConcurrent::run(&m_loaderPool, [this, iconNames, size, state]() {
        int successCount = 0;
        int failedCount = 0;
        int total = iconNames.size();

        for (int i = 0; i < iconNames.size(); ++i) {
            // Check cancel flag
            if (m_preloadCancelled.loadRelaxed() != 0) {
                qDebug() << "Preload cancelled at" << i << "/" << total;
                break;
            }

            const QString &iconName = iconNames.at(i);
            QString key = cacheKey(iconName, size, state);

            // Check if already cached
            {
                QMutexLocker locker(&m_cacheMutex);
                if (m_imageCache.contains(key)) {
                    successCount++;
                    Q_EMIT preloadProgress(i + 1, total);
                    continue;
                }
            }

            // Load icon using XdgIcon
            QIcon icon = XdgIcon::fromTheme(iconName);
            if (!icon.isNull()) {
                QPixmap pixmap = icon.pixmap(size);
                if (!pixmap.isNull()) {
                    QImage image = pixmap.toImage();
                    putCachedImage(key, image);
                    successCount++;
                } else {
                    failedCount++;
                }
            } else {
                failedCount++;
            }

            // Emit progress
            Q_EMIT preloadProgress(i + 1, total);
        }

        // Cleanup preload state
        {
            QMutexLocker locker(&m_preloadMutex);
            m_isPreloading = false;
        }

        // Emit completion signal
        Q_EMIT preloadCompleted(successCount, failedCount);

        qDebug() << "Preload completed:"
                 << "success=" << successCount
                 << "failed=" << failedCount
                 << "cancelled=" << (m_preloadCancelled.loadRelaxed() != 0);

        return successCount;
    });
}

void FastIconProvider::cancelPreload()
{
    m_preloadCancelled.storeRelaxed(1);
    qDebug() << "Preload cancel requested";
}

bool FastIconProvider::isPreloading() const
{
    QMutexLocker locker(&m_preloadMutex);
    return m_isPreloading;
}

// Auto-preload management (Stage 4.1.5)

void FastIconProvider::setAutoPreloadEnabled(bool enabled)
{
    QMutexLocker locker(&m_autoPreloadMutex);
    m_autoPreloadEnabled = enabled;
    qDebug() << "Auto-preload" << (enabled ? "enabled" : "disabled");
}

bool FastIconProvider::isAutoPreloadEnabled() const
{
    QMutexLocker locker(&m_autoPreloadMutex);
    return m_autoPreloadEnabled;
}

void FastIconProvider::setAutoPreloadCount(int count)
{
    QMutexLocker locker(&m_autoPreloadMutex);
    m_autoPreloadCount = qBound(1, count, 100);  // Limit between 1-100
    qDebug() << "Auto-preload count set to" << m_autoPreloadCount;
}

int FastIconProvider::autoPreloadCount() const
{
    QMutexLocker locker(&m_autoPreloadMutex);
    return m_autoPreloadCount;
}

void FastIconProvider::triggerAutoPreload()
{
    // Check if auto-preload is enabled
    {
        QMutexLocker locker(&m_autoPreloadMutex);
        if (!m_autoPreloadEnabled) {
            qDebug() << "Auto-preload skipped: disabled";
            return;
        }
    }

    // Check if already preloading
    if (isPreloading()) {
        qDebug() << "Auto-preload skipped: already preloading";
        return;
    }

    // Get top used icons from IconUsageTracker
    IconUsageTracker *tracker = IconUsageTracker::instance();
    if (!tracker->isEnabled() || tracker->getTotalIconCount() == 0) {
        qDebug() << "Auto-preload skipped: no usage data available";
        return;
    }

    int count = autoPreloadCount();
    QStringList topIcons = tracker->getTopUsed(count);

    if (topIcons.isEmpty()) {
        qDebug() << "Auto-preload skipped: no icons to preload";
        return;
    }

    qDebug() << "Auto-preload triggered:"
             << "icons=" << topIcons.size()
             << "top_icon=" << (topIcons.isEmpty() ? QString() : topIcons.first());

    // Trigger preload with default size (32px is most common)
    preloadIcons(topIcons, QSize(32, 32), 0);
}
