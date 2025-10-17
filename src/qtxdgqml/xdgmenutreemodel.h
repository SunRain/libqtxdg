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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef XDGMENUTREEMODEL_H
#define XDGMENUTREEMODEL_H

#include <QStandardItemModel>
#include <QString>
#include <QFutureWatcher>
#include <QDomDocument>

class XdgMenu;
class QDomElement;

/*!
 * \brief Tree model for XDG menu system
 *
 * Provides a hierarchical tree model for displaying application menus in TreeView.
 * Uses QStandardItemModel to simplify implementation.
 *
 * Features:
 * - Hierarchical category/application structure
 * - Lazy loading of menu items
 * - Search and filtering support
 * - Integration with Qt TreeView
 *
 * Usage in QML:
 * \code
 * TreeView {
 *     model: XdgMenuTreeModel {
 *         menuFile: "/etc/xdg/menus/applications.menu"
 *     }
 *     delegate: ItemDelegate {
 *         text: model.display
 *         icon.name: model.iconName
 *     }
 * }
 * \endcode
 */
class XdgMenuTreeModel : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString menuFile READ menuFile WRITE setMenuFile NOTIFY menuFileChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(int loadTimeMs READ loadTimeMs NOTIFY loadTimeChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)
    Q_PROPERTY(int totalApplications READ totalApplications NOTIFY totalApplicationsChanged)

public:
    enum CustomRoles {
        TypeRole = Qt::UserRole + 1,      // "category" or "application"
        DesktopIdRole,                     // Desktop file ID
        IconNameRole,                      // Icon name
        CategoryNameRole,                  // Category name
        CommentRole,                       // Description/comment
        ExecRole,                          // Exec command (for applications)
        GenericNameRole,                   // Generic name
        DesktopFilePathRole,               // Full path to .desktop file
        TerminalRole,                      // Whether to run in terminal
        StartupNotifyRole,                 // Startup notification support
        PathRole                           // Working directory
    };
    Q_ENUM(CustomRoles)

    explicit XdgMenuTreeModel(QObject *parent = nullptr);
    ~XdgMenuTreeModel() override;

    // Properties
    QString menuFile() const { return m_menuFile; }
    void setMenuFile(const QString &file);

    bool loaded() const { return m_loaded; }
    bool loading() const { return m_loading; }
    int loadTimeMs() const { return m_loadTimeMs; }
    QString lastError() const { return m_lastError; }
    int totalApplications() const { return m_totalApplications; }

    // QML invokable methods
    Q_INVOKABLE void reload();
    Q_INVOKABLE void loadAsync();
    Q_INVOKABLE QModelIndex findCategory(const QString &categoryName);
    Q_INVOKABLE QVariantList applicationsInCategory(const QString &category);
    Q_INVOKABLE int categoryCount() const;

Q_SIGNALS:
    void menuFileChanged();
    void loadedChanged();
    void loadingChanged();
    void loadingStarted();
    void loadingFinished();
    void loadTimeChanged();
    void errorOccurred(const QString &error);
    void totalApplicationsChanged();

private Q_SLOTS:
    void onLoadingFinished();

private:
    void buildTree();
    void buildTreeFromDocument(const QDomDocument &doc);
    void processCategory(QStandardItem *parentItem, const QDomElement &element);
    void addCategoryItem(QStandardItem *parent, const QString &name, const QString &icon);
    void addApplicationItem(QStandardItem *parent, const QString &desktopId);
    void addApplicationFromAppLink(QStandardItem *parent, const QDomElement &appLink);

    void setError(const QString &error);
    void clearError();

    XdgMenu *m_menu;
    QString m_menuFile;
    bool m_loaded;
    bool m_loading = false;
    int m_loadTimeMs = 0;
    QString m_lastError;
    int m_totalApplications;
    QFutureWatcher<QDomDocument> *m_watcher = nullptr;
};

#endif // XDGMENUTREEMODEL_H
