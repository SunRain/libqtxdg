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

#include "xdgmimetypewrapper.h"
#include "xdgmimetype.h"

#include <QUrl>
#include <QMimeDatabase>

XdgMimeTypeWrapper::XdgMimeTypeWrapper(QObject *parent)
    : QObject(parent)
{
}

XdgMimeTypeWrapper::~XdgMimeTypeWrapper() = default;

QString XdgMimeTypeWrapper::mimeTypeForFile(const QString &fileName)
{
    clearError();
    
    if (fileName.isEmpty()) {
        setError(QStringLiteral("File name is empty"));
        return QString();
    }
    
    QMimeDatabase db;
    const QMimeType mimeType = db.mimeTypeForFile(fileName);
    if (!mimeType.isValid()) {
        setError(QStringLiteral("Failed to determine MIME type for file: %1").arg(fileName));
        return QString();
    }
    
    return mimeType.name();
}

QString XdgMimeTypeWrapper::mimeTypeForUrl(const QString &url)
{
    clearError();
    
    if (url.isEmpty()) {
        setError(QStringLiteral("URL is empty"));
        return QString();
    }
    
    const QUrl qurl(url);
    if (!qurl.isValid()) {
        setError(QStringLiteral("Invalid URL: %1").arg(url));
        return QString();
    }
    
    // If it's a local file, use file-based detection
    if (qurl.isLocalFile()) {
        return mimeTypeForFile(qurl.toLocalFile());
    }
    
    // Otherwise use URL-based detection
    QMimeDatabase db;
    const QMimeType mimeType = db.mimeTypeForUrl(qurl);
    
    if (!mimeType.isValid()) {
        setError(QStringLiteral("Failed to determine MIME type for URL: %1").arg(url));
        return QString();
    }
    
    return mimeType.name();
}

QString XdgMimeTypeWrapper::comment(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QString();
    }
    
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return QString();
    }
    
    return mime.comment();
}

QString XdgMimeTypeWrapper::genericIconName(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QString();
    }
    
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return QString();
    }
    
    return mime.genericIconName();
}

QString XdgMimeTypeWrapper::iconName(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QString();
    }
    
    QMimeDatabase db;
    const XdgMimeType mime(db.mimeTypeForName(mimeType));
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return QString();
    }
    
    return mime.iconName();
}

QStringList XdgMimeTypeWrapper::globPatterns(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QStringList();
    }
    
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return QStringList();
    }
    
    return mime.globPatterns();
}

QStringList XdgMimeTypeWrapper::parentMimeTypes(const QString &mimeType)
{
    clearError();
    
    if (mimeType.isEmpty()) {
        setError(QStringLiteral("MIME type is empty"));
        return QStringList();
    }
    
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return QStringList();
    }
    
    return mime.parentMimeTypes();
}

QStringList XdgMimeTypeWrapper::allMimeTypes()
{
    clearError();
    
    QMimeDatabase db;
    const QList<QMimeType> mimeTypes = db.allMimeTypes();
    
    QStringList result;
    result.reserve(mimeTypes.size());
    
    for (const QMimeType &mimeType : mimeTypes) {
        result.append(mimeType.name());
    }
    
    return result;
}

bool XdgMimeTypeWrapper::inherits(const QString &mimeType, const QString &parentMimeType)
{
    clearError();
    
    if (mimeType.isEmpty() || parentMimeType.isEmpty()) {
        setError(QStringLiteral("MIME type name is empty"));
        return false;
    }
    
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForName(mimeType);
    if (!mime.isValid()) {
        setError(QStringLiteral("Invalid MIME type: %1").arg(mimeType));
        return false;
    }
    
    return mime.inherits(parentMimeType);
}

void XdgMimeTypeWrapper::setError(const QString &error)
{
    m_lastError = error;
    Q_EMIT errorOccurred(error);
}

void XdgMimeTypeWrapper::clearError()
{
    m_lastError.clear();
}
