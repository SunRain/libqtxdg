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

#ifndef XDGMENUWRAPPER_H
#define XDGMENUWRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>

class XdgMenu;

class XdgMenuWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)
    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QStringList environments READ environments WRITE setEnvironments NOTIFY environmentsChanged)

public:
    explicit XdgMenuWrapper(QObject *parent = nullptr);
    ~XdgMenuWrapper() override;

    // Properties
    QString lastError() const { return m_lastError; }
    bool isLoaded() const { return m_isLoaded; }
    QStringList environments() const;
    void setEnvironments(const QStringList &envs);

    // QML API - Menu loading
    Q_INVOKABLE bool loadMenu(const QString &menuFile = QString());
    Q_INVOKABLE void reload();
    
    // QML API - Data access (returns nested QVariantMap structure)
    Q_INVOKABLE QVariantMap menuTree();
    Q_INVOKABLE QVariantList applications(const QString &category = QString());
    Q_INVOKABLE QVariantList allCategories();
    
    // QML API - Utility
    Q_INVOKABLE static QString defaultMenuFile();

Q_SIGNALS:
    void errorOccurred(const QString &error);
    void loadedChanged();
    void environmentsChanged();

protected:
    void setError(const QString &error);
    void clearError();
    QVariantMap domElementToVariantMap(const class QDomElement &element);
    QVariantList extractApplications(const QDomElement &element, const QString &category = QString());

private:
    XdgMenu *m_menu;
    QString m_lastError;
    bool m_isLoaded;
    QString m_loadedMenuFile;
};

#endif // XDGMENUWRAPPER_H
