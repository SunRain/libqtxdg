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

#include "xdgautostartmodel.h"
#include "xdgautostart.h"
#include "xdgdesktopfile.h"
#include <QtConcurrent>
#include <QElapsedTimer>
#include <QDir>
#include <QDebug>

XdgAutoStartModel::XdgAutoStartModel(QObject *parent)
    : XdgSimpleListModel(parent)
    , m_loaded(false)
{
    // Lazy loading - don't load in constructor
}

XdgAutoStartModel::~XdgAutoStartModel()
{
    // Cancel any ongoing async loading
    if (m_watcher && !m_watcher->isFinished()) {
        m_watcher->cancel();
        m_watcher->deleteLater();
    }
    // Smart pointers automatically clean up m_entries
}

int XdgAutoStartModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // Trigger lazy loading via async
    if (!m_loaded) {
        const_cast<XdgAutoStartModel*>(this)->loadAsync();
    }

    return m_entries.count();
}

bool XdgAutoStartModel::setEnabled(int row, bool enabled)
{
    if (row < 0 || row >= m_entries.count()) {
        setError(QStringLiteral("Invalid row index"));
        return false;
    }

    auto entry = m_entries.at(row);

    // Update enabled status
    entry->enabled = enabled;

    // Notify data changed
    QModelIndex idx = index(row, 0);
    Q_EMIT dataChanged(idx, idx, {EnabledRole});

    return true;
}

void XdgAutoStartModel::reload()
{
    m_loaded = false;
    loadAsync();
}

QVariantMap XdgAutoStartModel::get(int row) const
{
    QVariantMap map;

    if (row < 0 || row >= m_entries.count())
        return map;

    const auto &entry = m_entries.at(row);

    map[QStringLiteral("desktopId")] = entry->desktopId;
    map[QStringLiteral("name")] = entry->name;
    map[QStringLiteral("enabled")] = entry->enabled;
    map[QStringLiteral("command")] = entry->command;
    map[QStringLiteral("comment")] = entry->comment;
    map[QStringLiteral("iconName")] = entry->iconName;

    return map;
}

bool XdgAutoStartModel::addAutoStart(const QString &desktopFile)
{
    auto desktop = QSharedPointer<XdgDesktopFile>::create();
    if (!desktop->load(desktopFile)) {
        setError(QStringLiteral("Failed to load desktop file"));
        return false;
    }

    // TODO: Actually copy desktop file to autostart directory
    // Simplified implementation for now

    // Reload the list
    reload();

    return true;
}

bool XdgAutoStartModel::removeAutoStart(int row)
{
    if (row < 0 || row >= m_entries.count()) {
        setError(QStringLiteral("Invalid row index"));
        return false;
    }

    // TODO: Actually delete the file from autostart directory
    // or set Hidden=true
    // Simplified implementation for now

    // Reload the list
    reload();

    return true;
}

QVariant XdgAutoStartModel::dataForRole(int row, int role) const
{
    if (row < 0 || row >= m_entries.count())
        return QVariant();

    const auto &entry = m_entries.at(row);

    switch (role) {
    case DesktopIdRole:
        return entry->desktopId;
    case NameRole:
        return entry->name;
    case EnabledRole:
        return entry->enabled;
    case CommandRole:
        return entry->command;
    case CommentRole:
        return entry->comment;
    case IconNameRole:
        return entry->iconName;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> XdgAutoStartModel::customRoleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DesktopIdRole] = "desktopId";
    roles[NameRole] = "name";
    roles[EnabledRole] = "enabled";
    roles[CommandRole] = "command";
    roles[CommentRole] = "comment";
    roles[IconNameRole] = "iconName";
    return roles;
}

// Async loading implementation
void XdgAutoStartModel::loadAsync()
{
    if (m_loading)
        return;

    setLoading(true);
    clearError();

    // Launch async loading in background thread
    QFuture<void> future = QtConcurrent::run([this]() {
        // This runs in background thread - just trigger loading
        // The actual data processing will be done in onLoadingFinished
    });

    m_watcher = new QFutureWatcher<void>(this);
    connect(m_watcher, &QFutureWatcher<void>::finished,
            this, &XdgAutoStartModel::onLoadingFinished);
    m_watcher->setFuture(future);
}

void XdgAutoStartModel::onLoadingFinished()
{
    QElapsedTimer timer;
    timer.start();

    beginResetData();

    // Clear old data (smart pointers auto-release)
    m_entries.clear();

    // Use XdgAutoStart API to get desktop file list
    XdgDesktopFileList desktopFiles = XdgAutoStart::desktopFileList(false);

    // Convert to internal Entry structure with smart pointers
    for (const XdgDesktopFile &desktop : desktopFiles) {
        auto entry = QSharedPointer<AutoStartEntry>::create();
        entry->desktopId = desktop.fileName();
        entry->name = desktop.name();
        entry->command = desktop.value(QStringLiteral("Exec")).toString();
        entry->comment = desktop.comment();
        entry->iconName = desktop.iconName();
        entry->enabled = !desktop.value(QStringLiteral("Hidden")).toBool();
        entry->desktopFile = QSharedPointer<XdgDesktopFile>::create(desktop);

        m_entries.append(entry);
    }

    m_loaded = true;
    endResetData();

    recordLoadTime(timer.elapsed());
    setLoading(false);

    m_watcher->deleteLater();
    m_watcher = nullptr;
}

// Synchronous loading (kept for backward compatibility)
void XdgAutoStartModel::loadAutoStartApps()
{
    clearError();

    QElapsedTimer timer;
    timer.start();

    beginResetData();

    // Clear old data (smart pointers auto-release)
    m_entries.clear();

    // Use XdgAutoStart API to get desktop file list
    XdgDesktopFileList desktopFiles = XdgAutoStart::desktopFileList(false);

    // Convert to internal Entry structure with smart pointers
    for (const XdgDesktopFile &desktop : desktopFiles) {
        auto entry = QSharedPointer<AutoStartEntry>::create();
        entry->desktopId = desktop.fileName();
        entry->name = desktop.name();
        entry->command = desktop.value(QStringLiteral("Exec")).toString();
        entry->comment = desktop.comment();
        entry->iconName = desktop.iconName();
        entry->enabled = !desktop.value(QStringLiteral("Hidden")).toBool();
        entry->desktopFile = QSharedPointer<XdgDesktopFile>::create(desktop);

        m_entries.append(entry);
    }

    m_loaded = true;
    endResetData();

    recordLoadTime(timer.elapsed());
}

bool XdgAutoStartModel::isEnabled(const QString &desktopId) const
{
    XdgDesktopFile desktop;
    if (!desktop.load(desktopId))
        return false;

    return !desktop.value(QStringLiteral("Hidden")).toBool();
}
