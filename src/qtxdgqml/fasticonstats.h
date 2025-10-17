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

#ifndef FASTICONSTATS_H
#define FASTICONSTATS_H

#include <QObject>
#include <QQmlEngine>

class FastIconProvider;

/*!
 * \brief QML-accessible singleton for FastIconProvider statistics and control
 *
 * This class provides a bridge between QML and FastIconProvider, allowing
 * QML code to query cache statistics, clear cache, and control the provider.
 *
 * Usage in QML:
 * \code
 * import org.lxqt.qtxdg 6.0
 *
 * Button {
 *     text: "Clear Cache"
 *     onClicked: FastIconStats.clearCache()
 * }
 *
 * Label {
 *     text: "Hit rate: " + FastIconStats.hitRate + "%"
 * }
 * \endcode
 */
class FastIconStats : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int hitCount READ hitCount NOTIFY statsChanged)
    Q_PROPERTY(int missCount READ missCount NOTIFY statsChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY statsChanged)
    Q_PROPERTY(int cachedItems READ cachedItems NOTIFY statsChanged)
    Q_PROPERTY(qint64 cacheBytes READ cacheBytes NOTIFY statsChanged)
    Q_PROPERTY(double hitRate READ hitRate NOTIFY statsChanged)

    // GPU cache statistics (Stage 3)
    Q_PROPERTY(int gpuTextureCount READ gpuTextureCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 gpuMemoryBytes READ gpuMemoryBytes NOTIFY statsChanged)
    Q_PROPERTY(double textureReuseRate READ textureReuseRate NOTIFY statsChanged)

    // Disk cache statistics (Stage 4.1)
    Q_PROPERTY(bool diskCacheEnabled READ diskCacheEnabled NOTIFY statsChanged)
    Q_PROPERTY(int diskCacheCount READ diskCacheCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 diskCacheBytes READ diskCacheBytes NOTIFY statsChanged)
    Q_PROPERTY(qint64 diskCacheMaxSize READ diskCacheMaxSize NOTIFY statsChanged)

    // Usage tracking statistics (Stage 4.1.6)
    Q_PROPERTY(bool usageTrackingEnabled READ usageTrackingEnabled NOTIFY statsChanged)
    Q_PROPERTY(int trackedIconCount READ trackedIconCount NOTIFY statsChanged)
    Q_PROPERTY(qint64 totalIconAccesses READ totalIconAccesses NOTIFY statsChanged)

public:
    explicit FastIconStats(QObject *parent = nullptr);

    // Property getters
    int hitCount() const;
    int missCount() const;
    int totalCount() const;
    int cachedItems() const;
    qint64 cacheBytes() const;
    double hitRate() const;

    // GPU property getters (Stage 3)
    int gpuTextureCount() const;
    qint64 gpuMemoryBytes() const;
    double textureReuseRate() const;

    // Disk cache property getters (Stage 4.1)
    bool diskCacheEnabled() const;
    int diskCacheCount() const;
    qint64 diskCacheBytes() const;
    qint64 diskCacheMaxSize() const;

    // Usage tracking property getters (Stage 4.1.6)
    bool usageTrackingEnabled() const;
    int trackedIconCount() const;
    qint64 totalIconAccesses() const;

    // Provider registration (called by plugin)
    static void setProvider(FastIconProvider *provider);

public Q_SLOTS:
    // Cache management
    void clearCache();
    void resetStats();
    void refresh();  // Update stats from provider

    // Cache configuration
    void setCacheSize(int megabytes);
    int cacheSize() const;

    // Thread pool configuration
    void setMaxThreadCount(int count);
    int maxThreadCount() const;

    // Performance testing helpers
    Q_INVOKABLE QVariantMap getStats() const;

    // GPU cache management (Stage 3)
    Q_INVOKABLE void clearGpuCache();
    Q_INVOKABLE void resetGpuStats();

    // Preload management (Stage 3.3)
    /*!
     * \brief QML callable: Preload multiple icons into L2 cache
     * \param iconNames Icon name list (QStringList or JS Array)
     * \param size Icon size (default: 32)
     * \param state Icon state (default: 0=Normal)
     */
    Q_INVOKABLE void preloadIcons(const QStringList &iconNames,
                                  int size = 32,
                                  int state = 0);

    /*!
     * \brief QML callable: Cancel preload operation
     */
    Q_INVOKABLE void cancelPreload();

    /*!
     * \brief QML callable: Check preload status
     */
    Q_INVOKABLE bool isPreloading() const;

    // Disk cache management (Stage 4.1)
    /*!
     * \brief QML callable: Clear disk cache
     */
    Q_INVOKABLE void clearDiskCache();

    /*!
     * \brief QML callable: Enable/disable disk cache
     * \param enabled Whether to enable disk cache
     */
    Q_INVOKABLE void setDiskCacheEnabled(bool enabled);

    /*!
     * \brief QML callable: Set disk cache max size
     * \param megabytes Maximum cache size in MB (default: 512MB)
     */
    Q_INVOKABLE void setDiskCacheMaxSize(int megabytes);

    // Auto-preload management (Stage 4.1.5)
    /*!
     * \brief QML callable: Enable/disable auto-preload
     * \param enabled Whether to enable auto-preload on startup
     */
    Q_INVOKABLE void setAutoPreloadEnabled(bool enabled);

    /*!
     * \brief QML callable: Check if auto-preload is enabled
     */
    Q_INVOKABLE bool isAutoPreloadEnabled() const;

    /*!
     * \brief QML callable: Set auto-preload count
     * \param count Number of top icons to preload (1-100)
     */
    Q_INVOKABLE void setAutoPreloadCount(int count);

    /*!
     * \brief QML callable: Get auto-preload count
     */
    Q_INVOKABLE int autoPreloadCount() const;

    /*!
     * \brief QML callable: Manually trigger auto-preload
     */
    Q_INVOKABLE void triggerAutoPreload();

    // Usage tracking management (Stage 4.1.6)
    /*!
     * \brief QML callable: Enable/disable usage tracking
     * \param enabled Whether to enable usage tracking
     */
    Q_INVOKABLE void setUsageTrackingEnabled(bool enabled);

    /*!
     * \brief QML callable: Clear usage statistics
     */
    Q_INVOKABLE void clearUsageStats();

    /*!
     * \brief QML callable: Save usage statistics to disk
     */
    Q_INVOKABLE void saveUsageStats();

    /*!
     * \brief QML callable: Get top N most used icons
     * \param count Number of icons to return (default: 10)
     * \return QStringList of icon names
     */
    Q_INVOKABLE QStringList getTopUsedIcons(int count = 10) const;

    /*!
     * \brief QML callable: Get N most recently used icons
     * \param count Number of icons to return (default: 20)
     * \return QStringList of icon names
     */
    Q_INVOKABLE QStringList getRecentlyUsedIcons(int count = 20) const;

    /*!
     * \brief QML callable: Get comprehensive performance report
     * \return QVariantMap with all performance metrics
     *
     * Returns a map containing:
     * - L2 cache stats (hit/miss/rate)
     * - L1 GPU cache stats
     * - L3 disk cache stats
     * - Usage tracking stats
     * - Auto-preload config
     */
    Q_INVOKABLE QVariantMap getPerformanceReport() const;

Q_SIGNALS:
    void statsChanged();

    /*!
     * \brief Preload progress signal (QML connectable)
     * \param current Current progress
     * \param total Total count
     */
    void preloadProgressChanged(int current, int total);

    /*!
     * \brief Preload completion signal (QML connectable)
     * \param successCount Success count
     * \param failedCount Failure count
     */
    void preloadFinished(int successCount, int failedCount);

private:
    static FastIconProvider *s_provider;
};

#endif // FASTICONSTATS_H
