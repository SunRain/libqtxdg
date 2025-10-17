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

#ifndef XDGSIMPLELISTMODEL_H
#define XDGSIMPLELISTMODEL_H

#include <QAbstractListModel>
#include <QString>

/*!
 * \brief Base class for simple list models in Qt6XdgQml
 *
 * Provides common functionality for QML list models including:
 * - Unified error handling
 * - Role name management
 * - Data update notifications
 *
 * Subclasses must implement:
 * - dataForRole() to provide data for each role
 * - customRoleNames() to define role name mappings
 * - rowCount() to return the number of items
 */
class XdgSimpleListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)
    Q_PROPERTY(ErrorCode errorCode READ errorCode NOTIFY errorCodeChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged)
    Q_PROPERTY(int loadTimeMs READ loadTimeMs NOTIFY loadTimeChanged)

public:
    enum class ErrorCode {
        NoError = 0,
        FileNotFound,
        PermissionDenied,
        ParseError,
        InvalidParameter,
        LoadingFailed,
        CacheError,
        UnknownError
    };
    Q_ENUM(ErrorCode)

    explicit XdgSimpleListModel(QObject *parent = nullptr);
    ~XdgSimpleListModel() override = default;

    // QAbstractListModel interface
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Error handling
    QString lastError() const { return m_lastError; }
    ErrorCode errorCode() const { return m_errorCode; }

    // Async loading properties
    bool loading() const { return m_loading; }
    int loadProgress() const { return m_loadProgress; }
    int loadTimeMs() const { return m_loadTimeMs; }

Q_SIGNALS:
    void countChanged();
    void errorOccurred(const QString &error);
    void errorCodeChanged();
    void loadingChanged();
    void loadingStarted();
    void loadingFinished();
    void loadProgressChanged(int current, int total);
    void loadTimeChanged();

protected:
    /*!
     * \brief Provide data for a specific row and role
     * \param row Row index
     * \param role Data role
     * \return Data for the specified role, or QVariant() if invalid
     *
     * This pure virtual function must be implemented by subclasses.
     */
    virtual QVariant dataForRole(int row, int role) const = 0;

    /*!
     * \brief Provide custom role name mappings
     * \return Hash map of role ID to role name
     *
     * This pure virtual function must be implemented by subclasses.
     * Example: { {NameRole, "name"}, {IconRole, "icon"} }
     */
    virtual QHash<int, QByteArray> customRoleNames() const = 0;

    // Helper methods for subclasses
    void setError(const QString &error);
    void setError(ErrorCode code, const QString &error);
    void clearError();
    void beginResetData();
    void endResetData();

    // Async loading helper methods
    void setLoading(bool loading);
    void setLoadProgress(int current, int total);
    void recordLoadTime(qint64 ms);

    QString m_lastError;
    ErrorCode m_errorCode = ErrorCode::NoError;
    bool m_loading = false;
    int m_loadProgress = 0;
    int m_loadTimeMs = 0;
};

#endif // XDGSIMPLELISTMODEL_H
