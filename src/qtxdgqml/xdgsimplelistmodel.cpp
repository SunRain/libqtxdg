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

#include "xdgsimplelistmodel.h"

XdgSimpleListModel::XdgSimpleListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant XdgSimpleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    if (row < 0 || row >= rowCount())
        return QVariant();

    return dataForRole(row, role);
}

QHash<int, QByteArray> XdgSimpleListModel::roleNames() const
{
    return customRoleNames();
}

void XdgSimpleListModel::setError(const QString &error)
{
    setError(ErrorCode::UnknownError, error);
}

void XdgSimpleListModel::setError(ErrorCode code, const QString &error)
{
    m_lastError = error;
    m_errorCode = code;
    Q_EMIT errorOccurred(error);
    Q_EMIT errorCodeChanged();
}

void XdgSimpleListModel::clearError()
{
    m_lastError.clear();
    if (m_errorCode != ErrorCode::NoError) {
        m_errorCode = ErrorCode::NoError;
        Q_EMIT errorCodeChanged();
    }
}

void XdgSimpleListModel::beginResetData()
{
    beginResetModel();
}

void XdgSimpleListModel::endResetData()
{
    endResetModel();
    Q_EMIT countChanged();
}

void XdgSimpleListModel::setLoading(bool loading)
{
    if (m_loading == loading)
        return;

    m_loading = loading;
    Q_EMIT loadingChanged();

    if (loading) {
        m_loadProgress = 0;
        Q_EMIT loadingStarted();
    } else {
        Q_EMIT loadingFinished();
    }
}

void XdgSimpleListModel::setLoadProgress(int current, int total)
{
    int progress = total > 0 ? (current * 100 / total) : 0;
    if (m_loadProgress != progress) {
        m_loadProgress = progress;
        Q_EMIT loadProgressChanged(current, total);
    }
}

void XdgSimpleListModel::recordLoadTime(qint64 ms)
{
    m_loadTimeMs = static_cast<int>(ms);
    Q_EMIT loadTimeChanged();
}
