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

#include "xdgmimeappswrapper.h"
#include "xdgmimeapps.h"
#include "xdgdesktopfile.h"

XdgMimeAppsWrapper::XdgMimeAppsWrapper(QObject *parent)
    : QObject(parent)
    , m_mimeApps(new XdgMimeApps(this))
{
}

XdgMimeAppsWrapper::~XdgMimeAppsWrapper() = default;

QVariantList XdgMimeAppsWrapper::appsForMimeType(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QVariantList();
    }
    
    const QList<XdgDesktopFile *> apps = m_mimeApps->apps(mimeType);
    
    QVariantList result;
    result.reserve(apps.size());
    
    for (const XdgDesktopFile *app : apps) {
        result.append(desktopFileToVariantMap(app));
    }
    
    return result;
}

QString XdgMimeAppsWrapper::defaultApp(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QString();
    }
    
    XdgDesktopFile *app = m_mimeApps->defaultApp(mimeType);
    if (!app) {
        return QString();
    }
    
    return app->fileName();
}

bool XdgMimeAppsWrapper::setDefaultApp(const QString &mimeType, const QString &desktopFile)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return false;
    }
    
    if (desktopFile.isEmpty()) {
        setError(QStringLiteral("Desktop file name is empty"));
        return false;
    }
    
    XdgDesktopFile app;
    if (!app.load(desktopFile)) {
        setError(QStringLiteral("Failed to load desktop file: %1").arg(desktopFile));
        return false;
    }
    
    const bool success = m_mimeApps->setDefaultApp(mimeType, app);
    
    if (success) {
        Q_EMIT associationsChanged();
    } else {
        setError(QStringLiteral("Failed to set default application"));
    }
    
    return success;
}

bool XdgMimeAppsWrapper::addAssociation(const QString &mimeType, const QString &desktopFile)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return false;
    }
    
    if (desktopFile.isEmpty()) {
        setError(QStringLiteral("Desktop file name is empty"));
        return false;
    }
    
    XdgDesktopFile app;
    if (!app.load(desktopFile)) {
        setError(QStringLiteral("Failed to load desktop file: %1").arg(desktopFile));
        return false;
    }
    
    const bool success = m_mimeApps->addSupport(mimeType, app);
    
    if (success) {
        Q_EMIT associationsChanged();
    } else {
        setError(QStringLiteral("Failed to add association"));
    }
    
    return success;
}

bool XdgMimeAppsWrapper::removeAssociation(const QString &mimeType, const QString &desktopFile)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return false;
    }
    
    if (desktopFile.isEmpty()) {
        setError(QStringLiteral("Desktop file name is empty"));
        return false;
    }
    
    XdgDesktopFile app;
    if (!app.load(desktopFile)) {
        setError(QStringLiteral("Failed to load desktop file: %1").arg(desktopFile));
        return false;
    }
    
    const bool success = m_mimeApps->removeSupport(mimeType, app);
    
    if (success) {
        Q_EMIT associationsChanged();
    } else {
        setError(QStringLiteral("Failed to remove association"));
    }
    
    return success;
}

QVariantList XdgMimeAppsWrapper::allApps()
{
    clearError();
    
    const QList<XdgDesktopFile *> apps = m_mimeApps->allApps();
    
    QVariantList result;
    result.reserve(apps.size());
    
    for (const XdgDesktopFile *app : apps) {
        result.append(desktopFileToVariantMap(app));
    }
    
    return result;
}

QVariantList XdgMimeAppsWrapper::categoryApps(const QString &category)
{
    clearError();
    
    if (category.isEmpty()) {
        setError(QStringLiteral("Category is empty"));
        return QVariantList();
    }
    
    const QList<XdgDesktopFile *> apps = m_mimeApps->categoryApps(category);
    
    QVariantList result;
    result.reserve(apps.size());
    
    for (const XdgDesktopFile *app : apps) {
        result.append(desktopFileToVariantMap(app));
    }
    
    return result;
}

QVariantList XdgMimeAppsWrapper::fallbackApps(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QVariantList();
    }
    
    const QList<XdgDesktopFile *> apps = m_mimeApps->fallbackApps(mimeType);
    
    QVariantList result;
    result.reserve(apps.size());
    
    for (const XdgDesktopFile *app : apps) {
        result.append(desktopFileToVariantMap(app));
    }
    
    return result;
}

QVariantList XdgMimeAppsWrapper::recommendedApps(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QVariantList();
    }
    
    const QList<XdgDesktopFile *> apps = m_mimeApps->recommendedApps(mimeType);
    
    QVariantList result;
    result.reserve(apps.size());
    
    for (const XdgDesktopFile *app : apps) {
        result.append(desktopFileToVariantMap(app));
    }
    
    return result;
}

QVariantMap XdgMimeAppsWrapper::desktopFileInfo(const QString &desktopFile)
{
    clearError();
    
    if (desktopFile.isEmpty()) {
        setError(QStringLiteral("Desktop file name is empty"));
        return QVariantMap();
    }
    
    XdgDesktopFile app;
    if (!app.load(desktopFile)) {
        setError(QStringLiteral("Failed to load desktop file: %1").arg(desktopFile));
        return QVariantMap();
    }
    
    return desktopFileToVariantMap(&app);
}

void XdgMimeAppsWrapper::reset()
{
    clearError();
    // Note: XdgMimeApps::reset() requires a mime type parameter
    // For a full reset, we need to recreate the object
    delete m_mimeApps;
    m_mimeApps = new XdgMimeApps(this);
    Q_EMIT associationsChanged();
}

void XdgMimeAppsWrapper::setError(const QString &error)
{
    m_lastError = error;
    Q_EMIT errorOccurred(error);
}

void XdgMimeAppsWrapper::clearError()
{
    m_lastError.clear();
}

QVariantMap XdgMimeAppsWrapper::desktopFileToVariantMap(const XdgDesktopFile *desktopFile)
{
    QVariantMap map;
    
    if (!desktopFile || !desktopFile->isValid()) {
        return map;
    }
    
    map[QStringLiteral("fileName")] = desktopFile->fileName();
    map[QStringLiteral("name")] = desktopFile->name();
    map[QStringLiteral("comment")] = desktopFile->comment();
    map[QStringLiteral("iconName")] = desktopFile->iconName();
    map[QStringLiteral("localizedValue")] = desktopFile->localizedValue(QStringLiteral("Name"));
    map[QStringLiteral("isValid")] = desktopFile->isValid();
    
    return map;
}
