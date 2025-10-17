/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2025 LXQt team
 * Authors:
 *   LXQt team
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

#include "fasticonresponse.h"
#include "fasticonprovider.h"
#include "cachedtexturefactory.h"
#include "diskiconcache.h"
#include "iconusagetracker.h"
#include <XdgIcon>
#include <QtConcurrent>
#include <QIcon>
#include <QPixmap>
#include <QDebug>

FastIconResponse::FastIconResponse(
    const QString &iconName,
    const QSize &size,
    const QString &fallback,
    IconState state,
    FastIconProvider *provider)
    : QQuickImageResponse()
    , m_iconName(iconName)
    , m_fallbackName(fallback)
    , m_requestedSize(size)
    , m_state(state)
    , m_provider(provider)
{
    // Check L2 cache first (on main thread, should be very fast)
    QString key = m_provider->cacheKey(m_iconName, m_requestedSize, static_cast<int>(m_state));
    QImage *cachedImage = m_provider->getCachedImage(key);

    if (cachedImage) {
        // Cache hit! Use cached image immediately
        m_result = *cachedImage;
        delete cachedImage;

        // Emit finished signal asynchronously (Qt requirement)
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
    } else {
        // Cache miss - start async loading
        m_watcher = new QFutureWatcher<QImage>(this);
        connect(m_watcher, &QFutureWatcher<QImage>::finished,
                this, &FastIconResponse::onLoadFinished);

        // Launch background loading task
        m_future = QtConcurrent::run(&m_provider->m_loaderPool,
                                     [this]() { return loadIconInBackground(); });
        m_watcher->setFuture(m_future);
    }
}

FastIconResponse::~FastIconResponse()
{
    // Future watcher is deleted by Qt parent system
}

QImage FastIconResponse::loadIconInBackground() const
{
    // This function runs in background thread

    // Generate cache key for L3 disk cache
    QString cacheKey = m_provider->cacheKey(m_iconName, m_requestedSize, static_cast<int>(m_state));

    // Record usage for smart preloading (Stage 4.1.4)
    IconUsageTracker::instance()->recordAccess(m_iconName, m_requestedSize.width(), static_cast<int>(m_state));

    // 1. Check L3 disk cache first (Stage 4.1)
    QImage diskImage = DiskIconCache::instance()->loadFromDisk(cacheKey);
    if (!diskImage.isNull()) {
        qDebug() << "L3 Disk cache HIT:" << m_iconName;
        return diskImage;  // Fast path: disk cache hit
    }

    qDebug() << "L3 Disk cache MISS:" << m_iconName << "- loading from XdgIcon";

    // 2. L3 Miss - Load from XdgIcon (original path)

    // Convert icon state to QIcon::Mode
    QIcon::Mode mode = QIcon::Normal;
    switch (m_state) {
        case Disabled:
            mode = QIcon::Disabled;
            break;
        case Pressed:
        case Hover:
            mode = QIcon::Active;  // Use Active for pressed/hover states
            break;
        default:
            mode = QIcon::Normal;
            break;
    }

    // Load icon using XdgIcon
    QIcon icon = XdgIcon::fromTheme(m_iconName);

    // If icon not found and fallback specified, try fallback
    if (icon.isNull() && !m_fallbackName.isEmpty()) {
        icon = XdgIcon::fromTheme(m_fallbackName);
    }

    // If still not found, use default application icon
    if (icon.isNull()) {
        icon = XdgIcon::fromTheme(XdgIcon::defaultApplicationIconName());
    }

    // Convert QIcon to QImage
    QImage result;
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(m_requestedSize, mode, QIcon::Off);
        if (!pixmap.isNull()) {
            result = pixmap.toImage();
        }
    }

    // 3. Save to L3 disk cache for next time (Stage 4.1)
    if (!result.isNull()) {
        DiskIconCache::instance()->saveToDisk(cacheKey, result);
    }

    return result;
}

void FastIconResponse::onLoadFinished()
{
    // This runs on main thread when background loading completes

    m_result = m_future.result();

    if (m_result.isNull()) {
        m_error = QStringLiteral("Failed to load icon: %1").arg(m_iconName);
        qWarning() << m_error;
    } else {
        // Store in L2 cache for future use
        QString key = m_provider->cacheKey(m_iconName, m_requestedSize, static_cast<int>(m_state));
        m_provider->putCachedImage(key, m_result);
    }

    // Notify QML Image component that loading is complete
    Q_EMIT finished();
}

QQuickTextureFactory* FastIconResponse::textureFactory() const
{
    if (m_result.isNull()) {
        return nullptr;
    }

    // Generate cache key for GPU texture cache
    // Format: "iconName@WxH_state"
    QString cacheKey = QString::fromLatin1("%1@%2x%3_%4")
        .arg(m_iconName)
        .arg(m_requestedSize.width())
        .arg(m_requestedSize.height())
        .arg(static_cast<int>(m_state));

    // Create CachedTextureFactory for L1 GPU texture caching
    // This enables texture reuse across multiple Image components
    return new CachedTextureFactory(m_result, cacheKey, true);
}

QString FastIconResponse::errorString() const
{
    return m_error;
}
