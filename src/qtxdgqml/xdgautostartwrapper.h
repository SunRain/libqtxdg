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

#ifndef QTXDGQML_XDGAUTOSTARTWRAPPER_H
#define QTXDGQML_XDGAUTOSTARTWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QStringList>
#include <QVariantList>

/*!
 * \brief QML wrapper for XdgAutoStart static class
 *
 * This class provides QML access to Desktop Application Autostart Specification
 * functionality through a singleton interface.
 */
class XdgAutoStartWrapper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit XdgAutoStartWrapper(QObject *parent = nullptr);

    /*!
     * \brief Get list of autostart desktop files
     * \param excludeHidden Whether to exclude hidden entries
     * \return List of desktop file paths
     */
    Q_INVOKABLE QStringList desktopFileList(bool excludeHidden = true);

    /*!
     * \brief Get list of autostart desktop files from specific directories
     * \param dirs Directories to search in
     * \param excludeHidden Whether to exclude hidden entries
     * \return List of desktop file paths
     */
    Q_INVOKABLE QStringList desktopFileListFromDirs(const QStringList &dirs, bool excludeHidden = true);

    /*!
     * \brief Get local autostart path for a desktop file
     * \param desktopFileName Name of the desktop file
     * \return Path in user's autostart directory
     */
    Q_INVOKABLE QString localPath(const QString &desktopFileName);
};

#endif // QTXDGQML_XDGAUTOSTARTWRAPPER_H
