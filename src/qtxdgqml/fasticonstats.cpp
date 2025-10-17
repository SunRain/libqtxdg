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

#include "fasticonstats.h"
#include "fasticonprovider.h"
#include "diskiconcache.h"
#include "iconusagetracker.h"
#include <QDebug>

// Static provider instance
FastIconProvider* FastIconStats::s_provider = nullptr;

FastIconStats::FastIconStats(QObject *parent)
    : QObject(parent)
{
}

void FastIconStats::setProvider(FastIconProvider *provider)
{
    s_provider = provider;
}

int FastIconStats::hitCount() const
{
    if (!s_provider) return 0;
    return s_provider->cacheStats().hitCount;
}

int FastIconStats::missCount() const
{
    if (!s_provider) return 0;
    return s_provider->cacheStats().missCount;
}

int FastIconStats::totalCount() const
{
    if (!s_provider) return 0;
    return s_provider->cacheStats().totalCount;
}

int FastIconStats::cachedItems() const
{
    if (!s_provider) return 0;
    return s_provider->cacheStats().cachedItems;
}

qint64 FastIconStats::cacheBytes() const
{
    if (!s_provider) return 0;
    return s_provider->cacheStats().cacheBytes;
}

double FastIconStats::hitRate() const
{
    if (!s_provider) return 0.0;
    return s_provider->cacheStats().hitRate() * 100.0;  // Return as percentage
}

void FastIconStats::clearCache()
{
    if (s_provider) {
        s_provider->clearCache();
        Q_EMIT statsChanged();
        qDebug() << "FastIconProvider cache cleared";
    }
}

void FastIconStats::resetStats()
{
    if (s_provider) {
        s_provider->resetStats();
        Q_EMIT statsChanged();
        qDebug() << "FastIconProvider stats reset";
    }
}

void FastIconStats::refresh()
{
    Q_EMIT statsChanged();
}

void FastIconStats::setCacheSize(int megabytes)
{
    if (s_provider) {
        s_provider->setCacheSize(megabytes);
        qDebug() << "FastIconProvider cache size set to" << megabytes << "MB";
    }
}

int FastIconStats::cacheSize() const
{
    if (!s_provider) return 0;
    return s_provider->cacheSize();
}

void FastIconStats::setMaxThreadCount(int count)
{
    if (s_provider) {
        s_provider->setMaxThreadCount(count);
        qDebug() << "FastIconProvider thread count set to" << count;
    }
}

int FastIconStats::maxThreadCount() const
{
    if (!s_provider) return 0;
    return s_provider->maxThreadCount();
}

QVariantMap FastIconStats::getStats() const
{
    QVariantMap map;

    if (!s_provider) {
        map[QStringLiteral("available")] = false;
        return map;
    }

    auto stats = s_provider->cacheStats();

    map[QStringLiteral("available")] = true;
    map[QStringLiteral("hitCount")] = stats.hitCount;
    map[QStringLiteral("missCount")] = stats.missCount;
    map[QStringLiteral("totalCount")] = stats.totalCount;
    map[QStringLiteral("cachedItems")] = stats.cachedItems;
    map[QStringLiteral("cacheBytes")] = QVariant::fromValue(stats.cacheBytes);
    map[QStringLiteral("hitRate")] = stats.hitRate() * 100.0;

    return map;
}

// GPU property getters (Stage 3)
int FastIconStats::gpuTextureCount() const
{
    if (!s_provider) return 0;
    return s_provider->gpuCacheStats().textureCount;
}

qint64 FastIconStats::gpuMemoryBytes() const
{
    if (!s_provider) return 0;
    return s_provider->gpuCacheStats().gpuBytes;
}

double FastIconStats::textureReuseRate() const
{
    if (!s_provider) return 0.0;
    return s_provider->gpuCacheStats().reuseRate * 100.0;  // Return as percentage
}

// GPU cache management (Stage 3)
void FastIconStats::clearGpuCache()
{
    if (s_provider) {
        s_provider->clearGpuCache();
        Q_EMIT statsChanged();
        qDebug() << "FastIconStats: GPU cache cleared";
    }
}

void FastIconStats::resetGpuStats()
{
    if (s_provider) {
        s_provider->resetGpuStats();
        Q_EMIT statsChanged();
        qDebug() << "FastIconStats: GPU stats reset";
    }
}

// Preload management (Stage 3.3)
void FastIconStats::preloadIcons(const QStringList &iconNames,
                                int size,
                                int state)
{
    if (!s_provider) {
        qWarning() << "FastIconStats: provider not available";
        return;
    }

    QSize qSize(size, size);

    // Connect signals (use Qt::UniqueConnection to avoid duplicates)
    connect(s_provider, &FastIconProvider::preloadProgress,
            this, &FastIconStats::preloadProgressChanged,
            Qt::UniqueConnection);

    connect(s_provider, &FastIconProvider::preloadCompleted,
            this, &FastIconStats::preloadFinished,
            Qt::UniqueConnection);

    // Start preload
    s_provider->preloadIcons(iconNames, qSize, state);

    qDebug() << "FastIconStats: preload started for" << iconNames.size() << "icons";
}

void FastIconStats::cancelPreload()
{
    if (s_provider) {
        s_provider->cancelPreload();
        qDebug() << "FastIconStats: preload cancel requested";
    }
}

bool FastIconStats::isPreloading() const
{
    if (!s_provider) return false;
    return s_provider->isPreloading();
}

// Disk cache property getters (Stage 4.1)
bool FastIconStats::diskCacheEnabled() const
{
    return DiskIconCache::instance()->isEnabled();
}

int FastIconStats::diskCacheCount() const
{
    return DiskIconCache::instance()->getCacheCount();
}

qint64 FastIconStats::diskCacheBytes() const
{
    return DiskIconCache::instance()->getCacheSize();
}

qint64 FastIconStats::diskCacheMaxSize() const
{
    return DiskIconCache::instance()->maxCacheSize();
}

// Disk cache management (Stage 4.1)
void FastIconStats::clearDiskCache()
{
    DiskIconCache::instance()->clearCache();
    Q_EMIT statsChanged();
    qDebug() << "FastIconStats: Disk cache cleared";
}

void FastIconStats::setDiskCacheEnabled(bool enabled)
{
    DiskIconCache::instance()->setEnabled(enabled);
    Q_EMIT statsChanged();
    qDebug() << "FastIconStats: Disk cache" << (enabled ? "enabled" : "disabled");
}

void FastIconStats::setDiskCacheMaxSize(int megabytes)
{
    qint64 bytes = static_cast<qint64>(megabytes) * 1024 * 1024;
    DiskIconCache::instance()->setMaxCacheSize(bytes);
    Q_EMIT statsChanged();
    qDebug() << "FastIconStats: Disk cache max size set to" << megabytes << "MB";
}

// Auto-preload management (Stage 4.1.5)
void FastIconStats::setAutoPreloadEnabled(bool enabled)
{
    if (s_provider) {
        s_provider->setAutoPreloadEnabled(enabled);
        qDebug() << "FastIconStats: Auto-preload" << (enabled ? "enabled" : "disabled");
    }
}

bool FastIconStats::isAutoPreloadEnabled() const
{
    if (!s_provider) return false;
    return s_provider->isAutoPreloadEnabled();
}

void FastIconStats::setAutoPreloadCount(int count)
{
    if (s_provider) {
        s_provider->setAutoPreloadCount(count);
        qDebug() << "FastIconStats: Auto-preload count set to" << count;
    }
}

int FastIconStats::autoPreloadCount() const
{
    if (!s_provider) return 0;
    return s_provider->autoPreloadCount();
}

void FastIconStats::triggerAutoPreload()
{
    if (s_provider) {
        s_provider->triggerAutoPreload();
        qDebug() << "FastIconStats: Auto-preload triggered manually";
    }
}

// Usage tracking property getters (Stage 4.1.6)
bool FastIconStats::usageTrackingEnabled() const
{
    return IconUsageTracker::instance()->isEnabled();
}

int FastIconStats::trackedIconCount() const
{
    return IconUsageTracker::instance()->getTotalIconCount();
}

qint64 FastIconStats::totalIconAccesses() const
{
    return IconUsageTracker::instance()->getTotalAccessCount();
}

// Usage tracking management (Stage 4.1.6)
void FastIconStats::setUsageTrackingEnabled(bool enabled)
{
    IconUsageTracker::instance()->setEnabled(enabled);
    Q_EMIT statsChanged();
    qDebug() << "FastIconStats: Usage tracking" << (enabled ? "enabled" : "disabled");
}

void FastIconStats::clearUsageStats()
{
    IconUsageTracker::instance()->clearStats();
    Q_EMIT statsChanged();
    qDebug() << "FastIconStats: Usage stats cleared";
}

void FastIconStats::saveUsageStats()
{
    IconUsageTracker::instance()->saveStats();
    qDebug() << "FastIconStats: Usage stats saved";
}

QStringList FastIconStats::getTopUsedIcons(int count) const
{
    return IconUsageTracker::instance()->getTopUsed(count);
}

QStringList FastIconStats::getRecentlyUsedIcons(int count) const
{
    return IconUsageTracker::instance()->getRecentlyUsed(count);
}

QVariantMap FastIconStats::getPerformanceReport() const
{
    QVariantMap report;

    // L2 CPU Cache Statistics
    QVariantMap l2Cache;
    l2Cache[QStringLiteral("hitCount")] = hitCount();
    l2Cache[QStringLiteral("missCount")] = missCount();
    l2Cache[QStringLiteral("totalCount")] = totalCount();
    l2Cache[QStringLiteral("cachedItems")] = cachedItems();
    l2Cache[QStringLiteral("cacheBytes")] = QVariant::fromValue(cacheBytes());
    l2Cache[QStringLiteral("cacheMB")] = cacheBytes() / 1024.0 / 1024.0;
    l2Cache[QStringLiteral("hitRate")] = hitRate() * 100.0;
    report[QStringLiteral("l2Cache")] = l2Cache;

    // L1 GPU Cache Statistics
    QVariantMap l1GpuCache;
    l1GpuCache[QStringLiteral("textureCount")] = gpuTextureCount();
    l1GpuCache[QStringLiteral("gpuBytes")] = QVariant::fromValue(gpuMemoryBytes());
    l1GpuCache[QStringLiteral("gpuMB")] = gpuMemoryBytes() / 1024.0 / 1024.0;
    l1GpuCache[QStringLiteral("reuseRate")] = textureReuseRate();
    report[QStringLiteral("l1GpuCache")] = l1GpuCache;

    // L3 Disk Cache Statistics
    QVariantMap l3DiskCache;
    l3DiskCache[QStringLiteral("enabled")] = diskCacheEnabled();
    l3DiskCache[QStringLiteral("count")] = diskCacheCount();
    l3DiskCache[QStringLiteral("bytes")] = QVariant::fromValue(diskCacheBytes());
    l3DiskCache[QStringLiteral("sizeMB")] = diskCacheBytes() / 1024.0 / 1024.0;
    l3DiskCache[QStringLiteral("maxBytes")] = QVariant::fromValue(diskCacheMaxSize());
    l3DiskCache[QStringLiteral("maxSizeMB")] = diskCacheMaxSize() / 1024.0 / 1024.0;
    l3DiskCache[QStringLiteral("usagePercent")] = diskCacheMaxSize() > 0
        ? (diskCacheBytes() * 100.0 / diskCacheMaxSize()) : 0.0;
    report[QStringLiteral("l3DiskCache")] = l3DiskCache;

    // Usage Tracking Statistics
    QVariantMap usageTracking;
    usageTracking[QStringLiteral("enabled")] = usageTrackingEnabled();
    usageTracking[QStringLiteral("trackedIconCount")] = trackedIconCount();
    usageTracking[QStringLiteral("totalAccesses")] = QVariant::fromValue(totalIconAccesses());

    // Top 5 most used icons
    QStringList topIcons = getTopUsedIcons(5);
    usageTracking[QStringLiteral("topIcons")] = topIcons;

    report[QStringLiteral("usageTracking")] = usageTracking;

    // Auto-preload Configuration
    QVariantMap autoPreload;
    autoPreload[QStringLiteral("enabled")] = isAutoPreloadEnabled();
    autoPreload[QStringLiteral("count")] = autoPreloadCount();
    autoPreload[QStringLiteral("isPreloading")] = isPreloading();
    report[QStringLiteral("autoPreload")] = autoPreload;

    // Overall Statistics
    QVariantMap overall;
    qint64 totalCacheBytes = cacheBytes() + gpuMemoryBytes() + diskCacheBytes();
    overall[QStringLiteral("totalCacheBytes")] = QVariant::fromValue(totalCacheBytes);
    overall[QStringLiteral("totalCacheMB")] = totalCacheBytes / 1024.0 / 1024.0;
    overall[QStringLiteral("totalCachedItems")] = cachedItems() + gpuTextureCount() + diskCacheCount();
    report[QStringLiteral("overall")] = overall;

    return report;
}
