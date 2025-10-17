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

#include "xdgmenuwrapper.h"
#include "xdgmenu.h"

#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>

XdgMenuWrapper::XdgMenuWrapper(QObject *parent)
    : QObject(parent)
    , m_menu(new XdgMenu(this))
    , m_isLoaded(false)
{
}

XdgMenuWrapper::~XdgMenuWrapper() = default;

QStringList XdgMenuWrapper::environments() const
{
    return m_menu->environments();
}

void XdgMenuWrapper::setEnvironments(const QStringList &envs)
{
    m_menu->setEnvironments(envs);
    Q_EMIT environmentsChanged();
    
    // Reload if already loaded
    if (m_isLoaded) {
        reload();
    }
}

bool XdgMenuWrapper::loadMenu(const QString &menuFile)
{
    clearError();
    
    QString file = menuFile;
    if (file.isEmpty()) {
        file = XdgMenu::getMenuFileName();
    }
    
    if (file.isEmpty()) {
        setError(QStringLiteral("No menu file specified and default menu file not found"));
        return false;
    }
    
    if (!QFileInfo::exists(file)) {
        setError(QStringLiteral("Menu file does not exist: %1").arg(file));
        return false;
    }
    
    const bool success = m_menu->read(file);
    
    if (!success) {
        setError(m_menu->errorString());
        m_isLoaded = false;
    } else {
        m_isLoaded = true;
        m_loadedMenuFile = file;
    }
    
    Q_EMIT loadedChanged();
    return success;
}

void XdgMenuWrapper::reload()
{
    if (m_loadedMenuFile.isEmpty()) {
        setError(QStringLiteral("No menu file loaded yet"));
        return;
    }
    
    loadMenu(m_loadedMenuFile);
}

QVariantMap XdgMenuWrapper::menuTree()
{
    clearError();
    
    if (!m_isLoaded) {
        setError(QStringLiteral("Menu not loaded. Call loadMenu() first"));
        return QVariantMap();
    }
    
    const QDomDocument xml = m_menu->xml();
    const QDomElement root = xml.documentElement();
    
    return domElementToVariantMap(root);
}

QVariantList XdgMenuWrapper::applications(const QString &category)
{
    clearError();
    
    if (!m_isLoaded) {
        setError(QStringLiteral("Menu not loaded. Call loadMenu() first"));
        return QVariantList();
    }
    
    const QDomDocument xml = m_menu->xml();
    const QDomElement root = xml.documentElement();
    
    return extractApplications(root, category);
}

QVariantList XdgMenuWrapper::allCategories()
{
    clearError();
    
    if (!m_isLoaded) {
        setError(QStringLiteral("Menu not loaded. Call loadMenu() first"));
        return QVariantList();
    }
    
    const QDomDocument xml = m_menu->xml();
    const QDomElement root = xml.documentElement();
    
    QStringList categories;
    QDomElement menuElement = root.firstChildElement(QStringLiteral("Menu"));
    
    while (!menuElement.isNull()) {
        const QString name = menuElement.firstChildElement(QStringLiteral("Name")).text();
        if (!name.isEmpty()) {
            categories.append(name);
        }
        
        menuElement = menuElement.nextSiblingElement(QStringLiteral("Menu"));
    }
    
    QVariantList result;
    for (const QString &cat : categories) {
        result.append(cat);
    }
    
    return result;
}

QString XdgMenuWrapper::defaultMenuFile()
{
    return XdgMenu::getMenuFileName();
}

void XdgMenuWrapper::setError(const QString &error)
{
    m_lastError = error;
    Q_EMIT errorOccurred(error);
}

void XdgMenuWrapper::clearError()
{
    m_lastError.clear();
}

QVariantMap XdgMenuWrapper::domElementToVariantMap(const QDomElement &element)
{
    QVariantMap map;
    
    if (element.isNull()) {
        return map;
    }
    
    map[QStringLiteral("tagName")] = element.tagName();
    map[QStringLiteral("text")] = element.text();
    
    // Add attributes
    const QDomNamedNodeMap attributes = element.attributes();
    QVariantMap attrs;
    for (int i = 0; i < attributes.count(); ++i) {
        const QDomNode node = attributes.item(i);
        attrs[node.nodeName()] = node.nodeValue();
    }
    if (!attrs.isEmpty()) {
        map[QStringLiteral("attributes")] = attrs;
    }
    
    // Add child elements
    QVariantList children;
    QDomElement child = element.firstChildElement();
    while (!child.isNull()) {
        children.append(domElementToVariantMap(child));
        child = child.nextSiblingElement();
    }
    if (!children.isEmpty()) {
        map[QStringLiteral("children")] = children;
    }
    
    return map;
}

QVariantList XdgMenuWrapper::extractApplications(const QDomElement &element, const QString &category)
{
    QVariantList result;
    
    if (element.isNull()) {
        return result;
    }
    
    // If category specified, find that menu first
    QDomElement searchElement = element;
    if (!category.isEmpty()) {
        QDomElement menuElement = element.firstChildElement(QStringLiteral("Menu"));
        while (!menuElement.isNull()) {
            const QString name = menuElement.firstChildElement(QStringLiteral("Name")).text();
            if (name == category) {
                searchElement = menuElement;
                break;
            }
            menuElement = menuElement.nextSiblingElement(QStringLiteral("Menu"));
        }
    }
    
    // Extract AppLink elements
    QDomElement appLinkElement = searchElement.firstChildElement(QStringLiteral("AppLink"));
    while (!appLinkElement.isNull()) {
        QVariantMap app;
        app[QStringLiteral("desktopFile")] = appLinkElement.attribute(QStringLiteral("desktopFile"));
        app[QStringLiteral("title")] = appLinkElement.attribute(QStringLiteral("title"));
        app[QStringLiteral("comment")] = appLinkElement.attribute(QStringLiteral("comment"));
        app[QStringLiteral("icon")] = appLinkElement.attribute(QStringLiteral("icon"));
        
        result.append(app);
        appLinkElement = appLinkElement.nextSiblingElement(QStringLiteral("AppLink"));
    }
    
    return result;
}
