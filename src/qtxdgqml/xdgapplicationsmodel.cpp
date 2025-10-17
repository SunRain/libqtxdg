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

#include "xdgapplicationsmodel.h"
#include "xdgmimeapps.h"
#include "xdgdesktopfile.h"
#include "xdgdirs.h"
#include <QtConcurrent>
#include <QElapsedTimer>
#include <QDir>
#include <QDebug>

// Initialize static members
QList<QSharedPointer<XdgDesktopFile>> XdgApplicationsModel::s_cachedApps;
QDateTime XdgApplicationsModel::s_cacheTime;
QMutex XdgApplicationsModel::s_cacheMutex;

XdgApplicationsModel::XdgApplicationsModel(QObject *parent)
    : XdgSimpleListModel(parent)
    , m_mimeApps(new XdgMimeApps(this))
    , m_loaded(false)
{
    // Setup file system watcher for auto-reload
    setupFileWatcher();
}

XdgApplicationsModel::~XdgApplicationsModel()
{
    // Cancel any ongoing async loading
    if (m_watcher && !m_watcher->isFinished()) {
        m_watcher->cancel();
        m_watcher->deleteLater();
    }
    // Smart pointers automatically clean up m_applications
}

int XdgApplicationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // Trigger lazy loading on first access
    ensureLoaded();

    return m_applications.count();
}

void XdgApplicationsModel::setCategory(const QString &category)
{
    if (m_category == category)
        return;

    // Cancel any ongoing loading
    if (m_watcher && !m_watcher->isFinished()) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
        setLoading(false);
    }

    m_category = category;
    Q_EMIT categoryChanged();

    // Reload applications with new category filter
    if (m_loaded) {
        loadApplicationsFromCache();
    }
}

void XdgApplicationsModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;

    // Cancel any ongoing loading
    if (m_watcher && !m_watcher->isFinished()) {
        m_watcher->cancel();
        m_watcher->deleteLater();
        m_watcher = nullptr;
        setLoading(false);
    }

    m_searchText = text;
    Q_EMIT searchTextChanged();

    // Invalidate regex cache when search text changes
    m_cachedRegexPattern.clear();

    // Reload applications with new search filter
    if (m_loaded) {
        loadApplicationsFromCache();
    }
}

void XdgApplicationsModel::setSearchMode(SearchMode mode)
{
    if (m_searchMode == mode)
        return;

    m_searchMode = mode;
    Q_EMIT searchModeChanged();

    // Invalidate regex cache when mode changes
    m_cachedRegexPattern.clear();

    // Reload applications with new search mode
    if (m_loaded && !m_searchText.isEmpty()) {
        loadApplicationsFromCache();
    }
}

void XdgApplicationsModel::reload()
{
    m_loaded = false;
    loadAsync();
}

QVariantMap XdgApplicationsModel::get(int row) const
{
    QVariantMap map;

    if (row < 0 || row >= m_applications.count())
        return map;

    const XdgDesktopFile *app = m_applications.at(row).data();

    map[QStringLiteral("desktopId")] = app->fileName();
    map[QStringLiteral("name")] = app->name();
    map[QStringLiteral("iconName")] = app->iconName();
    map[QStringLiteral("exec")] = app->value(QStringLiteral("Exec")).toString();
    map[QStringLiteral("comment")] = app->comment();
    map[QStringLiteral("categories")] = app->categories();
    map[QStringLiteral("mimeTypes")] = app->mimeTypes();

    return map;
}

QVariant XdgApplicationsModel::dataForRole(int row, int role) const
{
    if (row < 0 || row >= m_applications.count())
        return QVariant();

    const XdgDesktopFile *app = m_applications.at(row).data();

    switch (role) {
    case DesktopIdRole:
        return app->fileName();
    case NameRole:
        return app->name();
    case IconNameRole:
        return app->iconName();
    case ExecRole:
        return app->value(QStringLiteral("Exec")).toString();
    case CommentRole:
        return app->comment();
    case CategoriesRole:
        return app->categories();
    case MimeTypesRole:
        return app->mimeTypes();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> XdgApplicationsModel::customRoleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DesktopIdRole] = "desktopId";
    roles[NameRole] = "name";
    roles[IconNameRole] = "iconName";
    roles[ExecRole] = "exec";
    roles[CommentRole] = "comment";
    roles[CategoriesRole] = "categories";
    roles[MimeTypesRole] = "mimeTypes";
    return roles;
}

bool XdgApplicationsModel::matchesFilter(const XdgDesktopFile *app) const
{
    if (!app)
        return false;

    // Category filter (applied before text search)
    if (!m_category.isEmpty()) {
        if (!app->categories().contains(m_category)) {
            return false;
        }
    }

    // If no search text, accept all (category filter already applied)
    if (m_searchText.isEmpty())
        return true;

    // Dispatch to appropriate search method based on mode
    switch (m_searchMode) {
    case SearchMode::PlainText:
        return matchesFilterPlainText(app);
    case SearchMode::Regex:
        return matchesFilterRegex(app);
    case SearchMode::Fuzzy:
        return matchesFilterFuzzy(app);
    }

    return false;
}

bool XdgApplicationsModel::matchesFilterPlainText(const XdgDesktopFile *app) const
{
    const QString search = m_searchText.toLower();

    // Search in name
    if (app->name().toLower().contains(search))
        return true;

    // Search in comment
    if (app->comment().toLower().contains(search))
        return true;

    // Search in exec command
    if (app->value(QStringLiteral("Exec")).toString().toLower().contains(search))
        return true;

    return false;
}

bool XdgApplicationsModel::matchesFilterRegex(const XdgDesktopFile *app) const
{
    // Cache compiled regex for performance
    if (m_cachedRegexPattern != m_searchText) {
        m_cachedRegex = QRegularExpression(
            m_searchText,
            QRegularExpression::CaseInsensitiveOption
        );
        m_cachedRegexPattern = m_searchText;

        // If regex is invalid, fall back to plain text search
        if (!m_cachedRegex.isValid()) {
            qWarning() << "Invalid regex pattern:" << m_searchText
                      << "- falling back to plain text search";
            return matchesFilterPlainText(app);
        }
    }

    // Search in multiple fields
    QStringList searchFields = {
        app->name(),
        app->comment(),
        app->value(QStringLiteral("Exec")).toString()
    };

    for (const QString &field : searchFields) {
        if (m_cachedRegex.match(field).hasMatch()) {
            return true;
        }
    }

    return false;
}

bool XdgApplicationsModel::matchesFilterFuzzy(const XdgDesktopFile *app) const
{
    const QString pattern = m_searchText.toLower();

    // Calculate threshold: allow 30% edit distance
    int threshold = qMax(1, static_cast<int>(pattern.length() * 0.3));

    // Search in multiple fields
    QStringList searchFields = {
        app->name().toLower(),
        app->comment().toLower(),
        app->value(QStringLiteral("Exec")).toString().toLower()
    };

    for (const QString &field : searchFields) {
        // For performance, limit field length and do substring matching
        QString fieldToMatch = field.left(100);

        // Try to find pattern as substring first (fast path)
        if (fieldToMatch.contains(pattern)) {
            return true;
        }

        // Calculate edit distance
        int distance = levenshteinDistance(pattern, fieldToMatch);

        if (distance <= threshold) {
            return true;
        }
    }

    return false;
}

int XdgApplicationsModel::levenshteinDistance(const QString &s1, const QString &s2) const
{
    const int len1 = s1.length();
    const int len2 = s2.length();

    // Limit length to avoid performance issues
    if (len1 > 100 || len2 > 100) {
        return 999;  // Return large value to indicate no match
    }

    // Edge cases
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    // Use two rows for space optimization
    QVector<int> prevRow(len2 + 1);
    QVector<int> currRow(len2 + 1);

    // Initialize first row
    for (int i = 0; i <= len2; ++i) {
        prevRow[i] = i;
    }

    // Calculate edit distance
    for (int i = 0; i < len1; ++i) {
        currRow[0] = i + 1;

        for (int j = 0; j < len2; ++j) {
            int cost = (s1[i] == s2[j]) ? 0 : 1;

            currRow[j + 1] = qMin(qMin(
                prevRow[j + 1] + 1,     // deletion
                currRow[j] + 1),        // insertion
                prevRow[j] + cost       // substitution
            );
        }

        // Swap rows
        currRow.swap(prevRow);
    }

    return prevRow[len2];
}

void XdgApplicationsModel::ensureLoaded() const
{
    if (!m_loaded) {
        // Trigger async loading
        const_cast<XdgApplicationsModel*>(this)->loadAsync();
    }
}

// Async loading implementation
void XdgApplicationsModel::loadAsync()
{
    if (m_loading)
        return;

    setLoading(true);
    clearError();

    // Capture current filter settings
    QString category = m_category;
    QString searchText = m_searchText;

    // Launch async loading in background thread
    QFuture<QList<XdgDesktopFile*>> future = QtConcurrent::run([this, category]() {
        QMutexLocker locker(&s_cacheMutex);

        // Check if cache needs refresh
        if (!isCacheValid()) {
            loadAllAppsToCache(m_mimeApps);
        }

        // Return list of raw pointers for the result
        QList<XdgDesktopFile*> result;
        for (const auto &app : s_cachedApps) {
            result.append(app.data());
        }
        return result;
    });

    m_watcher = new QFutureWatcher<QList<XdgDesktopFile*>>(this);
    connect(m_watcher, &QFutureWatcher<QList<XdgDesktopFile*>>::finished,
            this, &XdgApplicationsModel::onLoadingFinished);
    m_watcher->setFuture(future);
}

void XdgApplicationsModel::onLoadingFinished()
{
    QElapsedTimer timer;
    timer.start();

    beginResetData();

    m_applications.clear();

    // Filter applications from cache (fast, in-memory operation)
    {
        QMutexLocker locker(&s_cacheMutex);
        for (const auto &app : s_cachedApps) {
            if (matchesFilter(app.data())) {
                m_applications.append(app);  // Share ownership
            }
        }
    }

    m_loaded = true;
    endResetData();

    recordLoadTime(timer.elapsed());
    setLoading(false);

    m_watcher->deleteLater();
    m_watcher = nullptr;
}

void XdgApplicationsModel::loadApplicationsFromCache()
{
    // Check if cache is still valid
    if (!isCacheValid()) {
        // Cache expired, use async loading
        loadAsync();
        return;
    }

    // Use incremental update to preserve ListView state
    updateFilterIncremental();
}

void XdgApplicationsModel::updateFilterIncremental()
{
    QElapsedTimer timer;
    timer.start();

    // Build new filtered list
    QList<QSharedPointer<XdgDesktopFile>> newApps;
    {
        QMutexLocker locker(&s_cacheMutex);
        for (const auto &app : s_cachedApps) {
            if (matchesFilter(app.data())) {
                newApps.append(app);
            }
        }
    }

    // Simple incremental update: remove items not in new list, add new items
    // This preserves ListView scroll position for items that remain

    // First, remove items that are no longer in the filtered list
    for (int i = m_applications.count() - 1; i >= 0; --i) {
        bool found = false;
        for (const auto &newApp : newApps) {
            if (m_applications[i]->fileName() == newApp->fileName()) {
                found = true;
                break;
            }
        }
        if (!found) {
            beginRemoveRows(QModelIndex(), i, i);
            m_applications.removeAt(i);
            endRemoveRows();
        }
    }

    // Then, add items that are not in the current list
    for (const auto &newApp : newApps) {
        bool found = false;
        for (const auto &app : m_applications) {
            if (app->fileName() == newApp->fileName()) {
                found = true;
                break;
            }
        }
        if (!found) {
            int insertPos = m_applications.count();
            beginInsertRows(QModelIndex(), insertPos, insertPos);
            m_applications.append(newApp);
            endInsertRows();
        }
    }

    Q_EMIT countChanged();
    recordLoadTime(timer.elapsed());
}

void XdgApplicationsModel::invalidateCache()
{
    QMutexLocker locker(&s_cacheMutex);
    s_cachedApps.clear();
    s_cacheTime = QDateTime();
}

// Static cache management methods
bool XdgApplicationsModel::isCacheValid()
{
    return s_cacheTime.isValid() &&
           s_cacheTime.secsTo(QDateTime::currentDateTime()) < CACHE_TTL_SECONDS;
}

void XdgApplicationsModel::loadAllAppsToCache(XdgMimeApps *mimeApps)
{
    // Clear old cache
    s_cachedApps.clear();

    // Load all applications
    QList<XdgDesktopFile *> rawApps = mimeApps->allApps();

    // Convert to shared pointers
    for (XdgDesktopFile *app : rawApps) {
        if (app) {
            s_cachedApps.append(QSharedPointer<XdgDesktopFile>(app));
        }
    }

    s_cacheTime = QDateTime::currentDateTime();
}

// Kept for backward compatibility / testing
void XdgApplicationsModel::loadApplications()
{
    clearError();

    QElapsedTimer timer;
    timer.start();

    beginResetData();

    m_applications.clear();

    // Get applications based on category filter
    QList<XdgDesktopFile *> allApps;

    if (m_category.isEmpty()) {
        allApps = m_mimeApps->allApps();
    } else {
        allApps = m_mimeApps->categoryApps(m_category);
    }

    // Apply search filter and convert to smart pointers
    for (XdgDesktopFile *app : allApps) {
        if (app && matchesFilter(app)) {
            m_applications.append(QSharedPointer<XdgDesktopFile>(app));
        } else {
            delete app;  // Clean up apps that don't match filter
        }
    }

    m_loaded = true;
    endResetData();

    recordLoadTime(timer.elapsed());
}

// File system watching implementation
void XdgApplicationsModel::setupFileWatcher()
{
    m_fileWatcher = new QFileSystemWatcher(this);
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(DEBOUNCE_MS);

    // Watch application directories
    QStringList dataDirs = XdgDirs::dataDirs();
    dataDirs.prepend(XdgDirs::dataHome());

    for (const QString &dir : dataDirs) {
        QString appsDir = dir + QStringLiteral("/applications");
        if (QDir(appsDir).exists()) {
            m_fileWatcher->addPath(appsDir);
        }
    }

    // Connect signals
    connect(m_fileWatcher, &QFileSystemWatcher::directoryChanged,
            this, &XdgApplicationsModel::onDirectoryChanged);
    connect(m_debounceTimer, &QTimer::timeout,
            this, &XdgApplicationsModel::onDebounceTimeout);
}

void XdgApplicationsModel::setAutoReload(bool enabled)
{
    if (m_autoReload == enabled)
        return;

    m_autoReload = enabled;
    Q_EMIT autoReloadChanged();
}

void XdgApplicationsModel::onDirectoryChanged(const QString &path)
{
    qDebug() << "[P1] Applications directory changed:" << path;
    Q_EMIT applicationsDirectoryChanged();

    if (m_autoReload && !m_loading) {
        // Restart debounce timer
        m_debounceTimer->start();
    }
}

void XdgApplicationsModel::onDebounceTimeout()
{
    qDebug() << "[P1] Debounce timeout - invalidating cache and reloading";
    invalidateCache();
    loadAsync();
}
