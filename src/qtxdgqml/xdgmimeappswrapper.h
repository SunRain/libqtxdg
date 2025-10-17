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

#ifndef XDGMIMEAPPSWRAPPER_H
#define XDGMIMEAPPSWRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

class XdgMimeApps;

class XdgMimeAppsWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)

public:
    explicit XdgMimeAppsWrapper(QObject *parent = nullptr);
    ~XdgMimeAppsWrapper() override;

    // Properties
    QString lastError() const { return m_lastError; }

    // QML API - MIME type associations
    Q_INVOKABLE QVariantList appsForMimeType(const QString &mimeType);
    Q_INVOKABLE QString defaultApp(const QString &mimeType);
    Q_INVOKABLE bool setDefaultApp(const QString &mimeType, const QString &desktopFile);
    Q_INVOKABLE bool addAssociation(const QString &mimeType, const QString &desktopFile);
    Q_INVOKABLE bool removeAssociation(const QString &mimeType, const QString &desktopFile);
    
    // QML API - Application queries
    Q_INVOKABLE QVariantList allApps();
    Q_INVOKABLE QVariantList categoryApps(const QString &category);
    Q_INVOKABLE QVariantList fallbackApps(const QString &mimeType);
    Q_INVOKABLE QVariantList recommendedApps(const QString &mimeType);
    
    // QML API - Desktop file info
    Q_INVOKABLE QVariantMap desktopFileInfo(const QString &desktopFile);

    // API别名方法 - 提高易用性
    Q_INVOKABLE QVariantMap appInfo(const QString &desktopFile) {
        return desktopFileInfo(desktopFile);
    }

    // QML API - Utility
    Q_INVOKABLE void reset();

Q_SIGNALS:
    void errorOccurred(const QString &error);
    void associationsChanged();

protected:
    void setError(const QString &error);
    void clearError();
    QVariantMap desktopFileToVariantMap(const class XdgDesktopFile *desktopFile);

private:
    XdgMimeApps *m_mimeApps;
    QString m_lastError;
};

#endif // XDGMIMEAPPSWRAPPER_H
