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

#include "xdgdirswrapper.h"
#include <XdgDirs>

XdgDirsWrapper::XdgDirsWrapper(QObject *parent)
    : QObject(parent)
{
}

QString XdgDirsWrapper::userDir(UserDirectory dir)
{
    return XdgDirs::userDir(static_cast<XdgDirs::UserDirectory>(dir));
}

QString XdgDirsWrapper::userDirDefault(UserDirectory dir)
{
    return XdgDirs::userDirDefault(static_cast<XdgDirs::UserDirectory>(dir));
}

bool XdgDirsWrapper::setUserDir(UserDirectory dir, const QString &value, bool createDir)
{
    return XdgDirs::setUserDir(static_cast<XdgDirs::UserDirectory>(dir), value, createDir);
}

QString XdgDirsWrapper::dataHome(bool createDir)
{
    return XdgDirs::dataHome(createDir);
}

QString XdgDirsWrapper::configHome(bool createDir)
{
    return XdgDirs::configHome(createDir);
}

QString XdgDirsWrapper::cacheHome(bool createDir)
{
    return XdgDirs::cacheHome(createDir);
}

QString XdgDirsWrapper::runtimeDir()
{
    return XdgDirs::runtimeDir();
}

QStringList XdgDirsWrapper::dataDirs(const QString &postfix)
{
    return XdgDirs::dataDirs(postfix);
}

QStringList XdgDirsWrapper::configDirs(const QString &postfix)
{
    return XdgDirs::configDirs(postfix);
}

QString XdgDirsWrapper::autostartHome(bool createDir)
{
    return XdgDirs::autostartHome(createDir);
}

QStringList XdgDirsWrapper::autostartDirs(const QString &postfix)
{
    return XdgDirs::autostartDirs(postfix);
}
