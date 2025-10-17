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

#ifndef QTXDGQML_XDGDEFAULTAPPSWRAPPER_H
#define QTXDGQML_XDGDEFAULTAPPSWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QStringList>

class XdgDesktopFileWrapper;

/*!
 * \brief QML wrapper for XdgDefaultApps static class
 *
 * This class provides QML access to default application management
 * functionality through a singleton interface.
 */
class XdgDefaultAppsWrapper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit XdgDefaultAppsWrapper(QObject *parent = nullptr);

    // Web Browser
    Q_INVOKABLE QString webBrowser();
    Q_INVOKABLE QStringList webBrowsers();
    Q_INVOKABLE bool setWebBrowser(const QString &desktopFileName);

    // Email Client
    Q_INVOKABLE QString emailClient();
    Q_INVOKABLE QStringList emailClients();
    Q_INVOKABLE bool setEmailClient(const QString &desktopFileName);

    // File Manager
    Q_INVOKABLE QString fileManager();
    Q_INVOKABLE QStringList fileManagers();
    Q_INVOKABLE bool setFileManager(const QString &desktopFileName);

    // Terminal
    Q_INVOKABLE QString terminal();
    Q_INVOKABLE QStringList terminals();
    Q_INVOKABLE bool setTerminal(const QString &desktopFileName);
};

#endif // QTXDGQML_XDGDEFAULTAPPSWRAPPER_H
