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

#ifndef ICONUSAGETRACKER_H
#define ICONUSAGETRACKER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QDateTime>
#include <QMutex>
#include <QStringList>

/*!
 * \brief Tracks icon usage statistics for smart preloading
 *
 * IconUsageTracker records access patterns for icons, enabling smart preloading
 * decisions based on frequency and recency. Statistics are persisted to disk
 * and survive application restarts.
 *
 * Features:
 * - Access frequency tracking (hit count)
 * - Last access time tracking (LRU support)
 * - First access time tracking (age calculation)
 * - Top-N most used icons query
 * - Recently used icons query
 * - Persistent statistics (JSON format)
 * - Thread-safe operations
 *
 * Usage:
 * \code
 * // Record icon access
 * IconUsageTracker::instance()->recordAccess("document-open", 32);
 *
 * // Get top 10 most used icons for preloading
 * QStringList topIcons = IconUsageTracker::instance()->getTopUsed(10);
 *
 * // Get recently used icons
 * QStringList recentIcons = IconUsageTracker::instance()->getRecentlyUsed(20);
 * \endcode
 */
class IconUsageTracker : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Icon usage statistics entry
     */
    struct UsageEntry {
        QString iconName;       ///< Icon name (e.g., "document-open")
        int size;               ///< Requested size (e.g., 32)
        int accessCount;        ///< Total access count
        QDateTime lastAccessed; ///< Last access timestamp
        QDateTime firstAccessed;///< First access timestamp

        UsageEntry()
            : size(0)
            , accessCount(0)
        {}
    };

    /*!
     * \brief Get singleton instance
     */
    static IconUsageTracker* instance();

    /*!
     * \brief Record an icon access
     * \param iconName Icon name (e.g., "document-open")
     * \param size Requested size (e.g., 32)
     * \param state Icon state (0=Normal, 1=Disabled, 2=Active, etc.)
     *
     * Thread-safe. Updates access count and last access time.
     */
    void recordAccess(const QString &iconName, int size = 32, int state = 0);

    /*!
     * \brief Get top N most frequently used icons
     * \param count Number of icons to return (default: 10)
     * \return List of icon names sorted by access count (descending)
     *
     * Useful for determining which icons to preload.
     */
    QStringList getTopUsed(int count = 10) const;

    /*!
     * \brief Get N most recently used icons
     * \param count Number of icons to return (default: 20)
     * \return List of icon names sorted by last access time (newest first)
     *
     * Useful for predicting next icon accesses.
     */
    QStringList getRecentlyUsed(int count = 20) const;

    /*!
     * \brief Get all usage entries
     * \return Hash map of cache keys to usage entries
     */
    QHash<QString, UsageEntry> getAllStats() const;

    /*!
     * \brief Get total number of tracked icons
     */
    int getTotalIconCount() const;

    /*!
     * \brief Get total access count across all icons
     */
    qint64 getTotalAccessCount() const;

    /*!
     * \brief Clear all usage statistics
     *
     * Thread-safe. Clears in-memory data and persists empty state to disk.
     */
    void clearStats();

    /*!
     * \brief Enable/disable usage tracking
     * \param enabled Whether to enable tracking
     */
    void setEnabled(bool enabled);

    /*!
     * \brief Check if usage tracking is enabled
     */
    bool isEnabled() const;

    /*!
     * \brief Save statistics to disk
     *
     * Normally called automatically, but can be called manually for explicit persistence.
     */
    void saveStats();

private:
    explicit IconUsageTracker(QObject *parent = nullptr);
    ~IconUsageTracker();

    // Prevent copy
    IconUsageTracker(const IconUsageTracker&) = delete;
    IconUsageTracker& operator=(const IconUsageTracker&) = delete;

    /*!
     * \brief Generate cache key for usage tracking
     * \param iconName Icon name
     * \param size Icon size
     * \param state Icon state
     * \return Cache key (format: "iconName@size_state")
     */
    QString generateKey(const QString &iconName, int size, int state) const;

    /*!
     * \brief Initialize statistics file path
     */
    void initializeStatsFile();

    /*!
     * \brief Load statistics from disk
     */
    void loadStats();

    // Static singleton members
    static IconUsageTracker *s_instance;
    static QMutex s_instanceMutex;

    // Instance data
    QString m_statsFilePath;                    ///< Path to JSON stats file
    QHash<QString, UsageEntry> m_usageStats;    ///< Usage statistics map
    mutable QMutex m_mutex;                     ///< Thread safety mutex
    bool m_enabled;                             ///< Whether tracking is enabled
    bool m_dirty;                               ///< Whether stats need saving
};

#endif // ICONUSAGETRACKER_H
