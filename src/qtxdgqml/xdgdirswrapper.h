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

#ifndef QTXDGQML_XDGDIRSWRAPPER_H
#define QTXDGQML_XDGDIRSWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>

/*!
 * \brief QML wrapper for XdgDirs static class
 *
 * This class provides QML access to XDG Base Directory Specification
 * functionality through a singleton interface.
 */
class XdgDirsWrapper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum UserDirectory {
        Desktop = 0,
        Download,
        Templates,
        PublicShare,
        Documents,
        Music,
        Pictures,
        Videos
    };
    Q_ENUM(UserDirectory)

    explicit XdgDirsWrapper(QObject *parent = nullptr);

    // User directories
    Q_INVOKABLE QString userDir(UserDirectory dir);
    Q_INVOKABLE QString userDirDefault(UserDirectory dir);
    Q_INVOKABLE bool setUserDir(UserDirectory dir, const QString &value, bool createDir = false);

    // XDG directories
    Q_INVOKABLE QString dataHome(bool createDir = true);
    Q_INVOKABLE QString configHome(bool createDir = true);
    Q_INVOKABLE QString cacheHome(bool createDir = true);
    Q_INVOKABLE QString runtimeDir();

    Q_INVOKABLE QStringList dataDirs(const QString &postfix = QString());
    Q_INVOKABLE QStringList configDirs(const QString &postfix = QString());

    // Autostart directories
    Q_INVOKABLE QString autostartHome(bool createDir = true);
    Q_INVOKABLE QStringList autostartDirs(const QString &postfix = QString());
};

#endif // QTXDGQML_XDGDIRSWRAPPER_H
