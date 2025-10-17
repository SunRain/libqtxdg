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

#ifndef CACHEDTEXTUREFACTORY_H
#define CACHEDTEXTUREFACTORY_H

#include <QQuickTextureFactory>
#include <QImage>
#include <QHash>
#include <QPointer>
#include <QMutex>
#include <QDateTime>

class QSGTexture;
class QQuickWindow;

/*!
 * \brief L1 GPU texture cache factory for FastIconProvider
 *
 * This factory implements GPU texture reuse across Image components.
 * Multiple Image components requesting the same icon (name + size + state)
 * will share the same QSGTexture object, dramatically reducing GPU memory
 * usage and texture upload overhead.
 *
 * Key features:
 * - Texture sharing: Same icon reuses same GPU texture
 * - LRU eviction: Automatic cache size management (max 256MB)
 * - Thread-safe: Mutex-protected cache operations
 * - Per-window cache: Each QQuickWindow has independent cache (GL context isolation)
 * - Atlas support: Small textures can use Qt's automatic atlas packing
 *
 * Architecture:
 *   QML Image → FastIconProvider → FastIconResponse
 *                                       ↓
 *                            CachedTextureFactory
 *                                       ↓
 *                     Check L1 GPU cache (static)
 *                     ├─ Hit: Reuse existing QSGTexture
 *                     └─ Miss: Create new + cache it
 */
class CachedTextureFactory : public QQuickTextureFactory
{
public:
    /*!
     * \brief Construct a cached texture factory
     * \param image The source image to create texture from
     * \param cacheKey Unique cache key for this texture
     * \param enableAtlas Enable Qt automatic atlas packing for small textures
     */
    explicit CachedTextureFactory(const QImage &image,
                                   const QString &cacheKey,
                                   bool enableAtlas = true);

    ~CachedTextureFactory() override;

    // QQuickTextureFactory interface
    /*!
     * \brief Create or retrieve cached texture
     *
     * This method is called by Qt Quick Scene Graph render thread.
     * It checks the per-window GPU cache first. If found and valid,
     * returns the cached texture. Otherwise creates a new texture,
     * caches it, and returns it.
     *
     * \param window The QQuickWindow requesting the texture
     * \return QSGTexture pointer (may be cached or newly created)
     */
    QSGTexture* createTexture(QQuickWindow *window) const override;

    /*!
     * \brief Get texture size
     * \return Size of the source image
     */
    QSize textureSize() const override;

    /*!
     * \brief Get estimated texture memory size
     * \return Estimated bytes for RGBA8 format
     */
    int textureByteCount() const override;

    // Static cache management
    /*!
     * \brief Clear all cached textures for a specific window
     * \param window The window to clear cache for (nullptr = all windows)
     */
    static void clearGpuCache(QQuickWindow *window = nullptr);

    /*!
     * \brief Get total number of cached textures
     * \return Total texture count across all windows
     */
    static int gpuCacheCount();

    /*!
     * \brief Get estimated total GPU memory usage
     * \return Estimated bytes (RGBA8 format assumption)
     */
    static qint64 gpuCacheBytes();

    /*!
     * \brief Get texture reuse statistics
     * \return Reuse rate (0.0 - 1.0)
     */
    static double textureReuseRate();

    /*!
     * \brief Reset reuse statistics counters
     */
    static void resetStats();

private:
    // Cache entry structure
    struct TextureEntry {
        QPointer<QSGTexture> texture;  // QPointer to detect texture deletion
        qint64 bytes;                  // Estimated memory size
        QDateTime lastUsed;            // For LRU eviction
        int reuseCount;                // How many times reused
    };

    // Per-window cache (GL context isolation)
    static QHash<QQuickWindow*, QHash<QString, TextureEntry>> s_perWindowCache;
    static QMutex s_cacheMutex;

    // Cache limits
    static const int MAX_GPU_CACHE_MB = 256;  // 256MB max GPU cache

    // Statistics
    static int s_textureCreateCount;
    static int s_textureReuseCount;

    // Helper methods
    /*!
     * \brief Evict least recently used textures for a window
     * \param window The window to evict from
     * \param bytesToFree Minimum bytes to free
     */
    static void evictLRU(QQuickWindow *window, qint64 bytesToFree);

    /*!
     * \brief Calculate estimated texture bytes
     * \param size Texture size
     * \return Estimated bytes (RGBA8)
     */
    static qint64 estimateTextureBytes(const QSize &size);

    // Member variables
    QImage m_image;
    QString m_cacheKey;
    bool m_enableAtlas;
};

#endif // CACHEDTEXTUREFACTORY_H
