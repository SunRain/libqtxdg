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

#ifndef XDGMIMETYPEWRAPPER_H
#define XDGMIMETYPEWRAPPER_H

#include <QObject>
#include <QString>
#include <QStringList>

class XdgMimeTypeWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)

public:
    explicit XdgMimeTypeWrapper(QObject *parent = nullptr);
    ~XdgMimeTypeWrapper() override;

    // Properties
    QString lastError() const { return m_lastError; }

    // QML API
    Q_INVOKABLE QString mimeTypeForFile(const QString &fileName);
    Q_INVOKABLE QString mimeTypeForUrl(const QString &url);
    Q_INVOKABLE QString comment(const QString &mimeType);
    Q_INVOKABLE QString genericIconName(const QString &mimeType);
    Q_INVOKABLE QString iconName(const QString &mimeType);
    Q_INVOKABLE QStringList globPatterns(const QString &mimeType);
    Q_INVOKABLE QStringList parentMimeTypes(const QString &mimeType);
    Q_INVOKABLE QStringList allMimeTypes();
    Q_INVOKABLE bool inherits(const QString &mimeType, const QString &parentMimeType);

    // API别名方法 - 提高易用性
    Q_INVOKABLE QStringList allParentMimeTypes(const QString &mimeType) {
        return parentMimeTypes(mimeType);
    }

Q_SIGNALS:
    void errorOccurred(const QString &error);

protected:
    void setError(const QString &error);
    void clearError();

private:
    QString m_lastError;
};

#endif // XDGMIMETYPEWRAPPER_H
