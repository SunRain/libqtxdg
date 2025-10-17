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

#ifndef QTXDGQML_XDGICONWRAPPER_H
#define QTXDGQML_XDGICONWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QUrl>

/*!
 * \brief QML wrapper for XdgIcon static class
 *
 * This class provides QML access to XDG Icon Theme Specification
 * functionality through a singleton interface.
 */
class XdgIconWrapper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(bool followColorScheme READ followColorScheme WRITE setFollowColorScheme NOTIFY followColorSchemeChanged)

public:
    explicit XdgIconWrapper(QObject *parent = nullptr);

    // Icon lookup - returns icon URL for QML Image component
    Q_INVOKABLE QUrl fromTheme(const QString &iconName);
    Q_INVOKABLE QUrl fromThemeWithFallback(const QString &iconName, const QString &fallbackIcon);
    Q_INVOKABLE QUrl fromThemeList(const QStringList &iconNames);

    // Default application icon
    Q_INVOKABLE QUrl defaultApplicationIcon();
    Q_INVOKABLE QString defaultApplicationIconName();

    // Theme management
    QString themeName() const;
    void setThemeName(const QString &themeName);

    // Color scheme following (KDE extension)
    bool followColorScheme() const;
    void setFollowColorScheme(bool enable);

    // Helper method to convert icon name to URL for QML Image component
    // Returns "image://theme/iconName" format for Qt's builtin theme provider
    Q_INVOKABLE QString toImageUrl(const QString &iconName);

Q_SIGNALS:
    void themeNameChanged();
    void followColorSchemeChanged();
};

#endif // QTXDGQML_XDGICONWRAPPER_H
