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

#include "xdgmenutreemodel.h"
#include "xdgmenu.h"
#include "xdgdesktopfile.h"

#include <QStandardItem>
#include <QDomDocument>
#include <QDomElement>
#include <QDebug>
#include <QtConcurrent>
#include <QElapsedTimer>

XdgMenuTreeModel::XdgMenuTreeModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_menu(new XdgMenu(this))
    , m_loaded(false)
    , m_totalApplications(0)
{
    // 设置默认菜单文件
    m_menuFile = QStringLiteral("/etc/xdg/menus/applications.menu");

    // 设置列标题
    setHorizontalHeaderLabels({tr("Name")});
}

XdgMenuTreeModel::~XdgMenuTreeModel()
{
    // Cancel any ongoing async loading
    if (m_watcher && !m_watcher->isFinished()) {
        m_watcher->cancel();
        m_watcher->deleteLater();
    }
}

void XdgMenuTreeModel::setMenuFile(const QString &file)
{
    if (m_menuFile == file)
        return;

    m_menuFile = file;
    Q_EMIT menuFileChanged();

    // 自动重新加载
    if (m_loaded) {
        reload();
    }
}

void XdgMenuTreeModel::reload()
{
    m_loaded = false;
    loadAsync();
}

// Async loading implementation
void XdgMenuTreeModel::loadAsync()
{
    if (m_loading)
        return;

    m_loading = true;
    Q_EMIT loadingChanged();
    Q_EMIT loadingStarted();

    clearError();

    QString menuFile = m_menuFile;

    // Get environments from XDG_CURRENT_DESKTOP if available
    QStringList environments;
    QString currentDesktop = QString::fromLocal8Bit(qgetenv("XDG_CURRENT_DESKTOP"));
    if (!currentDesktop.isEmpty()) {
        environments = currentDesktop.toLower().split(QLatin1Char(':'));
    }
    // Default to LXQt and generic environments if not set
    if (environments.isEmpty()) {
        environments = QStringList() << QStringLiteral("LXQt") << QStringLiteral("X-LXQT");
    }

    // Launch async loading in background thread
    QFuture<QDomDocument> future = QtConcurrent::run([menuFile, environments]() {
        XdgMenu menu;
        menu.setEnvironments(environments);  // Set environments for filtering
        if (!menu.read(menuFile)) {
            return QDomDocument();  // Return empty document on error
        }
        return menu.xml();
    });

    m_watcher = new QFutureWatcher<QDomDocument>(this);
    connect(m_watcher, &QFutureWatcher<QDomDocument>::finished,
            this, &XdgMenuTreeModel::onLoadingFinished);
    m_watcher->setFuture(future);
}

void XdgMenuTreeModel::onLoadingFinished()
{
    QElapsedTimer timer;
    timer.start();

    QDomDocument doc = m_watcher->result();

    if (doc.isNull()) {
        setError(tr("Failed to load menu file: %1").arg(m_menuFile));
        m_loaded = false;
    } else {
        buildTreeFromDocument(doc);
        m_loaded = true;
    }

    m_loadTimeMs = timer.elapsed();
    m_loading = false;

    Q_EMIT loadedChanged();
    Q_EMIT loadingChanged();
    Q_EMIT loadingFinished();
    Q_EMIT loadTimeChanged();
    Q_EMIT totalApplicationsChanged();

    m_watcher->deleteLater();
    m_watcher = nullptr;

    qDebug() << "[P2] Menu tree loaded in" << m_loadTimeMs << "ms with" << m_totalApplications << "applications";
}

// Synchronous loading (kept for backward compatibility)
void XdgMenuTreeModel::buildTree()
{
    clearError();
    clear();
    m_totalApplications = 0;

    // 尝试读取菜单文件
    if (!m_menu->read(m_menuFile)) {
        setError(tr("Failed to load menu file: %1").arg(m_menuFile));
        m_loaded = false;
        Q_EMIT loadedChanged();
        return;
    }

    const QDomDocument doc = m_menu->xml();
    buildTreeFromDocument(doc);

    m_loaded = true;
    Q_EMIT loadedChanged();
    Q_EMIT totalApplicationsChanged();
}

void XdgMenuTreeModel::buildTreeFromDocument(const QDomDocument &doc)
{
    clear();
    m_totalApplications = 0;

    if (doc.isNull()) {
        return;
    }

    const QDomElement docElem = doc.documentElement();

    // Recursively process menu structure
    processCategory(invisibleRootItem(), docElem);
}

QModelIndex XdgMenuTreeModel::findCategory(const QString &categoryName)
{
    // 在根级别查找分类
    for (int row = 0; row < rowCount(); ++row) {
        QStandardItem *item = this->item(row);
        if (item && item->data(CategoryNameRole).toString() == categoryName) {
            return indexFromItem(item);
        }
    }
    return QModelIndex();
}

QVariantList XdgMenuTreeModel::applicationsInCategory(const QString &category)
{
    QVariantList apps;

    QModelIndex catIndex = findCategory(category);
    if (!catIndex.isValid())
        return apps;

    QStandardItem *catItem = itemFromIndex(catIndex);
    if (!catItem)
        return apps;

    // 遍历该分类下的所有应用
    for (int row = 0; row < catItem->rowCount(); ++row) {
        QStandardItem *appItem = catItem->child(row);
        if (appItem && appItem->data(TypeRole).toString() == QStringLiteral("application")) {
            QVariantMap appMap;
            appMap[QStringLiteral("name")] = appItem->text();
            appMap[QStringLiteral("desktopId")] = appItem->data(DesktopIdRole);
            appMap[QStringLiteral("iconName")] = appItem->data(IconNameRole);
            appMap[QStringLiteral("comment")] = appItem->data(CommentRole);
            apps.append(appMap);
        }
    }

    return apps;
}

int XdgMenuTreeModel::categoryCount() const
{
    return rowCount();
}

void XdgMenuTreeModel::processCategory(QStandardItem *parentItem, const QDomElement &element)
{
    // Extract current Menu's attributes (processed by XdgMenu processDirectoryEntries)
    QString menuName = element.attribute(QStringLiteral("name"));
    QString menuTitle = element.attribute(QStringLiteral("title"), menuName);
    QString menuIcon = element.attribute(QStringLiteral("icon"));
    QString menuComment = element.attribute(QStringLiteral("comment"));

    // Update parent item attributes if not root
    if (parentItem != invisibleRootItem()) {
        if (!menuTitle.isEmpty()) {
            parentItem->setText(menuTitle);
        }
        if (!menuIcon.isEmpty()) {
            parentItem->setData(menuIcon, IconNameRole);
        }
        if (!menuComment.isEmpty()) {
            parentItem->setData(menuComment, CommentRole);
        }
    }

    // Traverse all child elements
    QDomNode node = element.firstChild();
    while (!node.isNull()) {
        QDomElement elem = node.toElement();

        if (elem.isNull()) {
            node = node.nextSibling();
            continue;
        }

        // Process submenu <Menu>
        if (elem.tagName() == QStringLiteral("Menu")) {
            QString childName = elem.attribute(QStringLiteral("name"));
            QString childTitle = elem.attribute(QStringLiteral("title"), childName);
            QString childIcon = elem.attribute(QStringLiteral("icon"));

            if (!childName.isEmpty()) {
                QStandardItem *categoryItem = new QStandardItem(childTitle);
                categoryItem->setData(QStringLiteral("category"), TypeRole);
                categoryItem->setData(childName, CategoryNameRole);
                categoryItem->setData(childIcon, IconNameRole);
                categoryItem->setEditable(false);

                // Recursively process subcategory
                processCategory(categoryItem, elem);

                // Only add non-empty menus
                if (categoryItem->rowCount() > 0) {
                    parentItem->appendRow(categoryItem);
                } else {
                    delete categoryItem;  // Remove empty menu
                }
            }
        }
        // Process application <AppLink>
        else if (elem.tagName() == QStringLiteral("AppLink")) {
            addApplicationFromAppLink(parentItem, elem);
        }

        node = node.nextSibling();
    }
}

void XdgMenuTreeModel::addCategoryItem(QStandardItem *parent, const QString &name, const QString &icon)
{
    QStandardItem *item = new QStandardItem(name);
    item->setData(QStringLiteral("category"), TypeRole);
    item->setData(name, CategoryNameRole);
    item->setData(icon, IconNameRole);
    item->setEditable(false);

    parent->appendRow(item);
}

void XdgMenuTreeModel::addApplicationItem(QStandardItem *parent, const QString &desktopId)
{
    XdgDesktopFile desktop;
    if (!desktop.load(desktopId))
        return;

    QStandardItem *item = new QStandardItem(desktop.name());
    item->setData(QStringLiteral("application"), TypeRole);
    item->setData(desktopId, DesktopIdRole);
    item->setData(desktop.iconName(), IconNameRole);
    item->setData(desktop.comment(), CommentRole);
    item->setData(desktop.value(QStringLiteral("Exec")).toString(), ExecRole);
    item->setEditable(false);

    parent->appendRow(item);
    m_totalApplications++;
}

void XdgMenuTreeModel::addApplicationFromAppLink(QStandardItem *parent, const QDomElement &appLink)
{
    // Extract all attributes from AppLink element (generated by XdgMenuApplinkProcessor)
    QString id = appLink.attribute(QStringLiteral("id"));
    QString title = appLink.attribute(QStringLiteral("title"));
    QString comment = appLink.attribute(QStringLiteral("comment"));
    QString genericName = appLink.attribute(QStringLiteral("genericName"));
    QString exec = appLink.attribute(QStringLiteral("exec"));
    QString icon = appLink.attribute(QStringLiteral("icon"));
    QString desktopFile = appLink.attribute(QStringLiteral("desktopFile"));
    QString path = appLink.attribute(QStringLiteral("path"));
    bool terminal = appLink.attribute(QStringLiteral("terminal")) == QStringLiteral("true") ||
                   appLink.attribute(QStringLiteral("terminal")) == QStringLiteral("1");
    bool startupNotify = appLink.attribute(QStringLiteral("startupNoify")) == QStringLiteral("true") ||
                         appLink.attribute(QStringLiteral("startupNoify")) == QStringLiteral("1");

    // Validate required fields
    if (id.isEmpty() || title.isEmpty()) {
        return;  // Invalid AppLink
    }

    // Create application item
    QStandardItem *item = new QStandardItem(title);
    item->setData(QStringLiteral("application"), TypeRole);
    item->setData(id, DesktopIdRole);
    item->setData(icon, IconNameRole);
    item->setData(comment, CommentRole);
    item->setData(genericName, GenericNameRole);
    item->setData(exec, ExecRole);
    item->setData(desktopFile, DesktopFilePathRole);
    item->setData(path, PathRole);
    item->setData(terminal, TerminalRole);
    item->setData(startupNotify, StartupNotifyRole);
    item->setEditable(false);

    parent->appendRow(item);
    m_totalApplications++;
}

void XdgMenuTreeModel::setError(const QString &error)
{
    m_lastError = error;
    Q_EMIT errorOccurred(error);
}

void XdgMenuTreeModel::clearError()
{
    m_lastError.clear();
}
