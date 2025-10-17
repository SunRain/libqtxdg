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

#include "xdgdesktopfilewrapper.h"

XdgDesktopFileWrapper::XdgDesktopFileWrapper(QObject *parent)
    : QObject(parent)
{
}

XdgDesktopFileWrapper::~XdgDesktopFileWrapper() = default;

bool XdgDesktopFileWrapper::load(const QString &fileName)
{
    bool result = m_desktopFile.load(fileName);
    if (result)
        emitAllChanged();
    return result;
}

bool XdgDesktopFileWrapper::save(const QString &fileName)
{
    return m_desktopFile.save(fileName);
}

QVariant XdgDesktopFileWrapper::value(const QString &key, const QVariant &defaultValue) const
{
    return m_desktopFile.value(key, defaultValue);
}

QVariant XdgDesktopFileWrapper::localizedValue(const QString &key, const QVariant &defaultValue) const
{
    return m_desktopFile.localizedValue(key, defaultValue);
}

void XdgDesktopFileWrapper::setValue(const QString &key, const QVariant &value)
{
    m_desktopFile.setValue(key, value);
    // Emit specific property changes if applicable
    if (key == QLatin1String("Name"))
        Q_EMIT nameChanged();
    else if (key == QLatin1String("Comment"))
        Q_EMIT commentChanged();
    else if (key == QLatin1String("Icon")) {
        Q_EMIT iconNameChanged();
        Q_EMIT iconUrlChanged();
    }
}

void XdgDesktopFileWrapper::setLocalizedValue(const QString &key, const QVariant &value)
{
    m_desktopFile.setLocalizedValue(key, value);
    if (key == QLatin1String("Name"))
        Q_EMIT nameChanged();
    else if (key == QLatin1String("Comment"))
        Q_EMIT commentChanged();
}

void XdgDesktopFileWrapper::removeEntry(const QString &key)
{
    m_desktopFile.removeEntry(key);
}

bool XdgDesktopFileWrapper::contains(const QString &key) const
{
    return m_desktopFile.contains(key);
}

bool XdgDesktopFileWrapper::startDetached(const QStringList &urls)
{
    return m_desktopFile.startDetached(urls);
}

bool XdgDesktopFileWrapper::startDetachedUrl(const QString &url)
{
    return m_desktopFile.startDetached(url);
}

bool XdgDesktopFileWrapper::actionActivate(const QString &action, const QStringList &urls)
{
    return m_desktopFile.actionActivate(action, urls);
}

QStringList XdgDesktopFileWrapper::expandExecString(const QStringList &urls) const
{
    return m_desktopFile.expandExecString(urls);
}

QString XdgDesktopFileWrapper::actionName(const QString &action) const
{
    return m_desktopFile.actionName(action);
}

QUrl XdgDesktopFileWrapper::actionIconUrl(const QString &action) const
{
    QString iconName = m_desktopFile.actionIconName(action);
    if (iconName.isEmpty())
        return QUrl();
    return QUrl(QStringLiteral("image://theme/") + iconName);
}

QString XdgDesktopFileWrapper::actionIconName(const QString &action) const
{
    return m_desktopFile.actionIconName(action);
}

bool XdgDesktopFileWrapper::isShown(const QString &environment) const
{
    return m_desktopFile.isShown(environment);
}

bool XdgDesktopFileWrapper::isSuitable(bool excludeHidden, const QString &environment) const
{
    return m_desktopFile.isSuitable(excludeHidden, environment);
}

bool XdgDesktopFileWrapper::tryExec() const
{
    return m_desktopFile.tryExec();
}

QString XdgDesktopFileWrapper::computeId(const QString &fileName, bool checkFileExists)
{
    return XdgDesktopFile::id(fileName, checkFileExists);
}

XdgDesktopFileWrapper::Type XdgDesktopFileWrapper::type() const
{
    return static_cast<Type>(m_desktopFile.type());
}

QString XdgDesktopFileWrapper::name() const
{
    return m_desktopFile.name();
}

QString XdgDesktopFileWrapper::comment() const
{
    return m_desktopFile.comment();
}

QUrl XdgDesktopFileWrapper::iconUrl() const
{
    QString iconName = m_desktopFile.iconName();
    if (iconName.isEmpty())
        return QUrl();
    return QUrl(QStringLiteral("image://theme/") + iconName);
}

QString XdgDesktopFileWrapper::iconName() const
{
    return m_desktopFile.iconName();
}

QString XdgDesktopFileWrapper::fileName() const
{
    return m_desktopFile.fileName();
}

bool XdgDesktopFileWrapper::isValid() const
{
    return m_desktopFile.isValid();
}

QStringList XdgDesktopFileWrapper::mimeTypes() const
{
    return m_desktopFile.mimeTypes();
}

QStringList XdgDesktopFileWrapper::categories() const
{
    return m_desktopFile.categories();
}

QStringList XdgDesktopFileWrapper::actions() const
{
    return m_desktopFile.actions();
}

QString XdgDesktopFileWrapper::url() const
{
    return m_desktopFile.url();
}

void XdgDesktopFileWrapper::emitAllChanged()
{
    Q_EMIT typeChanged();
    Q_EMIT nameChanged();
    Q_EMIT commentChanged();
    Q_EMIT iconUrlChanged();
    Q_EMIT iconNameChanged();
    Q_EMIT fileNameChanged();
    Q_EMIT isValidChanged();
    Q_EMIT mimeTypesChanged();
    Q_EMIT categoriesChanged();
    Q_EMIT actionsChanged();
    Q_EMIT urlChanged();
}
