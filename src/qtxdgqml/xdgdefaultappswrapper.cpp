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

#include "xdgdefaultappswrapper.h"
#include <XdgDefaultApps>
#include <XdgDesktopFile>

XdgDefaultAppsWrapper::XdgDefaultAppsWrapper(QObject *parent)
    : QObject(parent)
{
}

QString XdgDefaultAppsWrapper::webBrowser()
{
    XdgDesktopFile *app = XdgDefaultApps::webBrowser();
    if (app) {
        QString fileName = app->fileName();
        delete app;
        return fileName;
    }
    return QString();
}

QStringList XdgDefaultAppsWrapper::webBrowsers()
{
    QList<XdgDesktopFile *> apps = XdgDefaultApps::webBrowsers();
    QStringList result;
    result.reserve(apps.size());
    for (XdgDesktopFile *app : apps) {
        result.append(app->fileName());
        delete app;
    }
    return result;
}

bool XdgDefaultAppsWrapper::setWebBrowser(const QString &desktopFileName)
{
    XdgDesktopFile app;
    if (app.load(desktopFileName)) {
        return XdgDefaultApps::setWebBrowser(app);
    }
    return false;
}

QString XdgDefaultAppsWrapper::emailClient()
{
    XdgDesktopFile *app = XdgDefaultApps::emailClient();
    if (app) {
        QString fileName = app->fileName();
        delete app;
        return fileName;
    }
    return QString();
}

QStringList XdgDefaultAppsWrapper::emailClients()
{
    QList<XdgDesktopFile *> apps = XdgDefaultApps::emailClients();
    QStringList result;
    result.reserve(apps.size());
    for (XdgDesktopFile *app : apps) {
        result.append(app->fileName());
        delete app;
    }
    return result;
}

bool XdgDefaultAppsWrapper::setEmailClient(const QString &desktopFileName)
{
    XdgDesktopFile app;
    if (app.load(desktopFileName)) {
        return XdgDefaultApps::setEmailClient(app);
    }
    return false;
}

QString XdgDefaultAppsWrapper::fileManager()
{
    XdgDesktopFile *app = XdgDefaultApps::fileManager();
    if (app) {
        QString fileName = app->fileName();
        delete app;
        return fileName;
    }
    return QString();
}

QStringList XdgDefaultAppsWrapper::fileManagers()
{
    QList<XdgDesktopFile *> apps = XdgDefaultApps::fileManagers();
    QStringList result;
    result.reserve(apps.size());
    for (XdgDesktopFile *app : apps) {
        result.append(app->fileName());
        delete app;
    }
    return result;
}

bool XdgDefaultAppsWrapper::setFileManager(const QString &desktopFileName)
{
    XdgDesktopFile app;
    if (app.load(desktopFileName)) {
        return XdgDefaultApps::setFileManager(app);
    }
    return false;
}

QString XdgDefaultAppsWrapper::terminal()
{
    XdgDesktopFile *app = XdgDefaultApps::terminal();
    if (app) {
        QString fileName = app->fileName();
        delete app;
        return fileName;
    }
    return QString();
}

QStringList XdgDefaultAppsWrapper::terminals()
{
    QList<XdgDesktopFile *> apps = XdgDefaultApps::terminals();
    QStringList result;
    result.reserve(apps.size());
    for (XdgDesktopFile *app : apps) {
        result.append(app->fileName());
        delete app;
    }
    return result;
}

bool XdgDefaultAppsWrapper::setTerminal(const QString &desktopFileName)
{
    XdgDesktopFile app;
    if (app.load(desktopFileName)) {
        return XdgDefaultApps::setTerminal(app);
    }
    return false;
}
