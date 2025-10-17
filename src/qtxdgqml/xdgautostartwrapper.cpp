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

#include "xdgautostartwrapper.h"
#include <XdgAutoStart>
#include <XdgDesktopFile>

XdgAutoStartWrapper::XdgAutoStartWrapper(QObject *parent)
    : QObject(parent)
{
}

QStringList XdgAutoStartWrapper::desktopFileList(bool excludeHidden)
{
    XdgDesktopFileList files = XdgAutoStart::desktopFileList(excludeHidden);
    QStringList result;
    result.reserve(files.size());
    for (const XdgDesktopFile &file : files) {
        result.append(file.fileName());
    }
    return result;
}

QStringList XdgAutoStartWrapper::desktopFileListFromDirs(const QStringList &dirs, bool excludeHidden)
{
    XdgDesktopFileList files = XdgAutoStart::desktopFileList(dirs, excludeHidden);
    QStringList result;
    result.reserve(files.size());
    for (const XdgDesktopFile &file : files) {
        result.append(file.fileName());
    }
    return result;
}

QString XdgAutoStartWrapper::localPath(const QString &desktopFileName)
{
    XdgDesktopFile file;
    if (file.load(desktopFileName)) {
        return XdgAutoStart::localPath(file);
    }
    return QString();
}
