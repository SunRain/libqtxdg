/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2025
 * Authors:
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef XDGAPPLICATIONSMODEL_H
#define XDGAPPLICATIONSMODEL_H

#include "xdgsimplelistmodel.h"
#include <QStringList>
#include <QSharedPointer>
#include <QMutex>
#include <QDateTime>
#include <QFutureWatcher>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QRegularExpression>

class XdgMimeApps;
class XdgDesktopFile;

/*!
 * \brief QML model for listing installed applications
 *
 * Provides a filterable list of installed applications with support for:
 * - Category filtering
 * - Search text filtering
 * - Lazy loading
 * - Dynamic reloading
 *
 * Usage in QML:
 * \code
 * ListView {
 *     model: XdgApplicationsModel {
 *         category: "Development"
 *         searchText: searchField.text
 *     }
 *     delegate: ItemDelegate {
 *         text: model.name
 *         icon.name: model.iconName
 *     }
 * }
 * \endcode
 */
class XdgApplicationsModel : public XdgSimpleListModel
{
    Q_OBJECT
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(bool autoReload READ autoReload WRITE setAutoReload NOTIFY autoReloadChanged)
    Q_PROPERTY(SearchMode searchMode READ searchMode WRITE setSearchMode NOTIFY searchModeChanged)

public:
    /*!
     * \brief Search mode for text filtering
     *
     * - PlainText: Simple case-insensitive substring matching (default, fastest)
     * - Regex: Regular expression matching (powerful but slower)
     * - Fuzzy: Fuzzy matching using Levenshtein distance (tolerates typos)
     */
    enum class SearchMode {
        PlainText = 0,  ///< Simple substring search (default)
        Regex = 1,      ///< Regular expression search
        Fuzzy = 2       ///< Fuzzy matching (Levenshtein distance)
    };
    Q_ENUM(SearchMode)

    enum Roles {
        DesktopIdRole = Qt::UserRole + 1,
        NameRole,
        IconNameRole,
        ExecRole,
        CommentRole,
        CategoriesRole,
        MimeTypesRole
    };
    Q_ENUM(Roles)

    explicit XdgApplicationsModel(QObject *parent = nullptr);
    ~XdgApplicationsModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Properties
    QString category() const { return m_category; }
    void setCategory(const QString &category);

    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &text);

    bool autoReload() const { return m_autoReload; }
    void setAutoReload(bool enabled);

    SearchMode searchMode() const { return m_searchMode; }
    void setSearchMode(SearchMode mode);

    // QML invokable methods
    Q_INVOKABLE void reload();
    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE void loadAsync();
    Q_INVOKABLE void invalidateCache();

Q_SIGNALS:
    void categoryChanged();
    void searchTextChanged();
    void autoReloadChanged();
    void searchModeChanged();
    void applicationsDirectoryChanged();

protected:
    QVariant dataForRole(int row, int role) const override;
    QHash<int, QByteArray> customRoleNames() const override;

private Q_SLOTS:
    void onLoadingFinished();
    void onDirectoryChanged(const QString &path);
    void onDebounceTimeout();

private:
    void loadApplications();
    void loadApplicationsFromCache();
    void updateFilterIncremental();
    bool matchesFilter(const XdgDesktopFile *app) const;
    bool matchesFilterPlainText(const XdgDesktopFile *app) const;
    bool matchesFilterRegex(const XdgDesktopFile *app) const;
    bool matchesFilterFuzzy(const XdgDesktopFile *app) const;
    int levenshteinDistance(const QString &s1, const QString &s2) const;
    void ensureLoaded() const;
    void setupFileWatcher();

    // Global cache (shared across all instances)
    static QList<QSharedPointer<XdgDesktopFile>> s_cachedApps;
    static QDateTime s_cacheTime;
    static QMutex s_cacheMutex;
    static constexpr int CACHE_TTL_SECONDS = 300;  // 5 minutes

    static bool isCacheValid();
    static void loadAllAppsToCache(XdgMimeApps *mimeApps);

    XdgMimeApps *m_mimeApps;
    QList<QSharedPointer<XdgDesktopFile>> m_applications;
    QString m_category;
    QString m_searchText;
    SearchMode m_searchMode = SearchMode::PlainText;
    mutable bool m_loaded;
    QFutureWatcher<QList<XdgDesktopFile*>> *m_watcher = nullptr;

    // Regex caching for performance
    mutable QRegularExpression m_cachedRegex;
    mutable QString m_cachedRegexPattern;

    // File system watching
    QFileSystemWatcher *m_fileWatcher = nullptr;
    QTimer *m_debounceTimer = nullptr;
    bool m_autoReload = true;
    static constexpr int DEBOUNCE_MS = 100;  // 100ms debounce (v1.1: reduced from 500ms)
};

#endif // XDGAPPLICATIONSMODEL_H
