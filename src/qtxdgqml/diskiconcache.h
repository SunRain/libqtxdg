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

#ifndef DISKICONCACHE_H
#define DISKICONCACHE_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QHash>
#include <QDateTime>

/*!
 * \brief Disk-based persistent icon cache (L3 cache)
 *
 * This class provides a persistent disk cache for decoded icon images,
 * surviving application restarts and significantly reducing cold start latency.
 *
 * Features:
 * - Persistent storage in ~/.cache/libqtxdg/icon-cache/
 * - PNG compression for efficient storage
 * - LRU eviction (max 512MB disk space)
 * - Automatic cache validation (checksum verification)
 * - Thread-safe operations
 *
 * Cache Key Format: iconName@widthxheights_state
 * File Format: PNG with metadata
 */
class DiskIconCache : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Get singleton instance
     */
    static DiskIconCache* instance();

    /*!
     * \brief Load image from disk cache
     * \param cacheKey Cache key (format: iconName@widthxheights_state)
     * \return Cached image, or null image if not found/invalid
     */
    QImage loadFromDisk(const QString &cacheKey);

    /*!
     * \brief Save image to disk cache
     * \param cacheKey Cache key
     * \param image Image to save
     * \return true if saved successfully
     */
    bool saveToDisk(const QString &cacheKey, const QImage &image);

    /*!
     * \brief Clear all disk cache
     */
    void clearCache();

    /*!
     * \brief Get total disk cache size in bytes
     */
    qint64 getCacheSize() const;

    /*!
     * \brief Get number of cached items
     */
    int getCacheCount() const;

    /*!
     * \brief Enable or disable disk cache
     * \param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);

    /*!
     * \brief Check if disk cache is enabled
     */
    bool isEnabled() const;

    /*!
     * \brief Set maximum disk cache size in bytes
     * \param bytes Maximum size (default: 512MB)
     */
    void setMaxCacheSize(qint64 bytes);

    /*!
     * \brief Get maximum cache size
     */
    qint64 maxCacheSize() const;

private:
    explicit DiskIconCache(QObject *parent = nullptr);
    ~DiskIconCache() override;

    // Disable copy
    DiskIconCache(const DiskIconCache&) = delete;
    DiskIconCache& operator=(const DiskIconCache&) = delete;

    /*!
     * \brief Initialize cache directory
     */
    void initializeCacheDir();

    /*!
     * \brief Get file path for cache key
     */
    QString getFilePath(const QString &cacheKey) const;

    /*!
     * \brief Check if cache needs eviction and evict if necessary
     */
    void checkAndEvict();

    /*!
     * \brief Evict LRU items to free specified bytes
     * \param bytesToFree Bytes to free
     */
    void evictLRU(qint64 bytesToFree);

    /*!
     * \brief Update cache index (file metadata)
     */
    void updateCacheIndex();

    /*!
     * \brief Load cache index from disk
     */
    void loadCacheIndex();

    /*!
     * \brief Save cache index to disk
     */
    void saveCacheIndex();

    // Cache entry metadata
    struct CacheEntry {
        QString filePath;
        qint64 fileSize;
        QDateTime lastAccessed;
    };

    QString m_cacheDir;  // Cache directory path
    qint64 m_maxCacheSize;  // Maximum cache size (default: 512MB)
    bool m_enabled;  // Cache enabled flag

    QHash<QString, CacheEntry> m_cacheIndex;  // In-memory index
    mutable QMutex m_mutex;  // Thread safety

    static DiskIconCache *s_instance;
    static QMutex s_instanceMutex;
};

#endif // DISKICONCACHE_H
