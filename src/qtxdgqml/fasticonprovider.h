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

#ifndef FASTICONPROVIDER_H
#define FASTICONPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QThreadPool>
#include <QCache>
#include <QImage>
#include <QMutex>
#include <QFuture>
#include <QAtomicInt>

class FastIconResponse;

/*!
 * \brief High-performance async icon provider for QML
 *
 * This provider implements a multi-level caching strategy and
 * asynchronous loading to prevent UI thread blocking.
 *
 * Features:
 * - Fully asynchronous icon loading (0ms UI blocking)
 * - L2 CPU memory cache (QCache based)
 * - Thread pool for parallel loading
 * - URL parameter support for size, fallback, state
 *
 * URL Format:
 *   image://fasticon/iconName?size=24x24&fallback=default&state=0
 *
 * Where:
 *   - iconName: XDG icon name (e.g., "document-open")
 *   - size: Requested size in format WxH (default: 32x32)
 *   - fallback: Fallback icon name (optional)
 *   - state: Icon state 0=Normal, 1=Disabled, 2=Pressed, 3=Hover (default: 0)
 */
class FastIconProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT

public:
    FastIconProvider();
    ~FastIconProvider() override;

    /*!
     * \brief Request asynchronous icon loading
     *
     * This method is called by QML Image component and returns
     * immediately with a FastIconResponse object. The actual
     * loading happens in background thread.
     */
    QQuickImageResponse* requestImageResponse(
        const QString &id,
        const QSize &requestedSize) override;

    // Cache management
    void clearCache();
    void setCacheSize(int megabytes);
    int cacheSize() const;

    // Statistics
    struct CacheStats {
        int hitCount = 0;
        int missCount = 0;
        int totalCount = 0;
        int cachedItems = 0;
        qint64 cacheBytes = 0;

        double hitRate() const {
            return totalCount > 0 ? (double)hitCount / totalCount : 0.0;
        }
    };

    CacheStats cacheStats() const;
    void resetStats();

    // GPU cache statistics (Stage 3)
    struct GpuCacheStats {
        int textureCount = 0;
        qint64 gpuBytes = 0;
        double reuseRate = 0.0;
    };

    GpuCacheStats gpuCacheStats() const;
    void clearGpuCache();
    void resetGpuStats();

    // Thread pool configuration
    void setMaxThreadCount(int count);
    int maxThreadCount() const;

    // Preload management (Stage 3.3)
    /*!
     * \brief Asynchronously preload multiple icons into L2 cache
     *
     * \param iconNames List of icon names to preload
     * \param size Icon size
     * \param state Icon state (default: 0 = Normal)
     * \return QFuture<int> Returns number of successfully loaded icons
     */
    QFuture<int> preloadIcons(const QStringList &iconNames,
                              const QSize &size,
                              int state = 0);

    /*!
     * \brief Cancel current preload task (if running)
     */
    void cancelPreload();

    /*!
     * \brief Check if preload task is currently running
     */
    bool isPreloading() const;

    // Auto-preload management (Stage 4.1.5)
    /*!
     * \brief Enable/disable auto-preload on startup
     * \param enabled Whether to enable auto-preload
     *
     * When enabled, most frequently used icons (from IconUsageTracker)
     * will be preloaded automatically when the provider is initialized.
     */
    void setAutoPreloadEnabled(bool enabled);

    /*!
     * \brief Check if auto-preload is enabled
     */
    bool isAutoPreloadEnabled() const;

    /*!
     * \brief Set number of icons to auto-preload
     * \param count Number of top used icons to preload (default: 30)
     */
    void setAutoPreloadCount(int count);

    /*!
     * \brief Get auto-preload count
     */
    int autoPreloadCount() const;

    /*!
     * \brief Manually trigger auto-preload
     *
     * Preloads top N most used icons based on IconUsageTracker statistics.
     * This is automatically called on startup if auto-preload is enabled.
     */
    void triggerAutoPreload();

Q_SIGNALS:
    /*!
     * \brief Preload progress signal
     * \param current Current loaded count
     * \param total Total count
     */
    void preloadProgress(int current, int total);

    /*!
     * \brief Preload completed signal
     * \param successCount Number of successfully loaded icons
     * \param failedCount Number of failed icons
     */
    void preloadCompleted(int successCount, int failedCount);

private:
    friend class FastIconResponse;

    // L2 CPU memory cache
    QCache<QString, QImage> m_imageCache;
    mutable QMutex m_cacheMutex;

    // Cache statistics
    mutable CacheStats m_stats;
    mutable QMutex m_statsMutex;

    // Async loading thread pool
    QThreadPool m_loaderPool;

    // Preload state (Stage 3.3)
    QAtomicInt m_preloadCancelled{0};  // Cancel flag
    bool m_isPreloading{false};
    mutable QMutex m_preloadMutex;

    // Auto-preload configuration (Stage 4.1.5)
    bool m_autoPreloadEnabled{true};    // Auto-preload on startup
    int m_autoPreloadCount{30};         // Number of icons to preload
    mutable QMutex m_autoPreloadMutex;

    // Helper methods
    QString cacheKey(const QString &iconName, const QSize &size, int state) const;
    QImage* getCachedImage(const QString &key);
    void putCachedImage(const QString &key, const QImage &image);

    void recordCacheHit();
    void recordCacheMiss();
};

#endif // FASTICONPROVIDER_H
