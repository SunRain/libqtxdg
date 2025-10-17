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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef XDGAUTOSTARTMODEL_H
#define XDGAUTOSTARTMODEL_H

#include "xdgsimplelistmodel.h"
#include <QStringList>
#include <QSharedPointer>
#include <QFutureWatcher>

class XdgDesktopFile;

/*!
 * \brief QML model for managing autostart applications
 *
 * Provides a list of autostart entries with support for:
 * - Listing all autostart applications
 * - Enabling/disabling autostart entries
 * - Dynamic reloading
 *
 * Usage in QML:
 * \code
 * ListView {
 *     model: XdgAutoStartModel {}
 *     delegate: CheckBox {
 *         text: model.name
 *         checked: model.enabled
 *         onToggled: model.setEnabled(index, checked)
 *     }
 * }
 * \endcode
 */
class XdgAutoStartModel : public XdgSimpleListModel
{
    Q_OBJECT

public:
    enum Roles {
        DesktopIdRole = Qt::UserRole + 1,
        NameRole,
        EnabledRole,
        CommandRole,
        CommentRole,
        IconNameRole
    };
    Q_ENUM(Roles)

    explicit XdgAutoStartModel(QObject *parent = nullptr);
    ~XdgAutoStartModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // QML invokable methods
    Q_INVOKABLE bool setEnabled(int row, bool enabled);
    Q_INVOKABLE void reload();
    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE bool addAutoStart(const QString &desktopFile);
    Q_INVOKABLE bool removeAutoStart(int row);
    Q_INVOKABLE void loadAsync();

protected:
    QVariant dataForRole(int row, int role) const override;
    QHash<int, QByteArray> customRoleNames() const override;

private slots:
    void onLoadingFinished();

private:
    struct AutoStartEntry {
        QString desktopId;
        QString name;
        QString command;
        QString comment;
        QString iconName;
        bool enabled;
        QSharedPointer<XdgDesktopFile> desktopFile;
    };

    void loadAutoStartApps();
    bool isEnabled(const QString &desktopId) const;

    QList<QSharedPointer<AutoStartEntry>> m_entries;
    mutable bool m_loaded;
    QFutureWatcher<void> *m_watcher = nullptr;
};

#endif // XDGAUTOSTARTMODEL_H
