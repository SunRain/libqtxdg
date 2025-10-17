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

#include "xdgiconwrapper.h"
#include <XdgIcon>
#include <QIcon>

XdgIconWrapper::XdgIconWrapper(QObject *parent)
    : QObject(parent)
{
}

QUrl XdgIconWrapper::fromTheme(const QString &iconName)
{
    if (iconName.isEmpty())
        return QUrl();

    QIcon icon = XdgIcon::fromTheme(iconName);
    if (icon.isNull())
        return QUrl();

    // For QML, we return image:// URL scheme with the icon name
    // QML's IconImage can handle this natively with Qt 6
    return QUrl(QStringLiteral("image://theme/") + iconName);
}

QUrl XdgIconWrapper::fromThemeWithFallback(const QString &iconName, const QString &fallbackIcon)
{
    if (iconName.isEmpty())
        return fromTheme(fallbackIcon);

    QIcon icon = XdgIcon::fromTheme(iconName);
    if (icon.isNull())
        return fromTheme(fallbackIcon);

    return QUrl(QStringLiteral("image://theme/") + iconName);
}

QUrl XdgIconWrapper::fromThemeList(const QStringList &iconNames)
{
    for (const QString &name : iconNames) {
        QIcon icon = XdgIcon::fromTheme(name);
        if (!icon.isNull())
            return QUrl(QStringLiteral("image://theme/") + name);
    }
    return QUrl();
}

QUrl XdgIconWrapper::defaultApplicationIcon()
{
    return QUrl(QStringLiteral("image://theme/") + XdgIcon::defaultApplicationIconName());
}

QString XdgIconWrapper::defaultApplicationIconName()
{
    return XdgIcon::defaultApplicationIconName();
}

QString XdgIconWrapper::themeName() const
{
    return XdgIcon::themeName();
}

void XdgIconWrapper::setThemeName(const QString &themeName)
{
    if (XdgIcon::themeName() != themeName) {
        XdgIcon::setThemeName(themeName);
        Q_EMIT themeNameChanged();
    }
}

bool XdgIconWrapper::followColorScheme() const
{
    return XdgIcon::followColorScheme();
}

void XdgIconWrapper::setFollowColorScheme(bool enable)
{
    if (XdgIcon::followColorScheme() != enable) {
        XdgIcon::setFollowColorScheme(enable);
        Q_EMIT followColorSchemeChanged();
    }
}

QString XdgIconWrapper::toImageUrl(const QString &iconName)
{
    if (iconName.isEmpty())
        return QString();
    
    return QStringLiteral("image://theme/") + iconName;
}
