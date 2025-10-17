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

#ifndef FASTICONRESPONSE_H
#define FASTICONRESPONSE_H

#include <QQuickImageResponse>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QString>
#include <QSize>

class FastIconProvider;

/*!
 * \brief Async response for icon loading requests
 *
 * This class handles the asynchronous loading of a single icon.
 * The actual loading happens in a background thread via QtConcurrent.
 *
 * Lifecycle:
 * 1. Created by FastIconProvider::requestImageResponse()
 * 2. Starts background loading immediately in constructor
 * 3. Emits finished() signal when loading completes
 * 4. QML Image component calls textureFactory() to get the result
 * 5. Deleted by Qt's QML engine when no longer needed
 */
class FastIconResponse : public QQuickImageResponse
{
    Q_OBJECT

public:
    /*!
     * \brief Icon state enum matching QIcon::Mode and QIcon::State
     */
    enum IconState {
        Normal = 0,
        Disabled = 1,
        Pressed = 2,
        Hover = 3
    };

    /*!
     * \brief Construct response and start async loading
     *
     * \param iconName XDG icon name (e.g., "document-open")
     * \param size Requested icon size
     * \param fallback Fallback icon name (optional)
     * \param state Icon state (Normal/Disabled/Pressed/Hover)
     * \param provider Parent provider for cache access
     */
    FastIconResponse(const QString &iconName,
                     const QSize &size,
                     const QString &fallback,
                     IconState state,
                     FastIconProvider *provider);

    ~FastIconResponse() override;

    /*!
     * \brief Returns texture factory for the loaded icon
     *
     * Called by QML Image component after finished() signal.
     * Returns nullptr if loading failed.
     */
    QQuickTextureFactory* textureFactory() const override;

    /*!
     * \brief Returns error string if loading failed
     */
    QString errorString() const override;

private Q_SLOTS:
    /*!
     * \brief Called when background loading completes
     */
    void onLoadFinished();

private:
    /*!
     * \brief Parse URL parameters from icon ID
     *
     * Parses format: iconName?size=24x24&fallback=default&state=0
     */
    static void parseIconId(const QString &id,
                           QString &iconName,
                           QString &fallback,
                           IconState &state);

    /*!
     * \brief Background loading function (runs in thread pool)
     */
    QImage loadIconInBackground() const;

    QString m_iconName;
    QString m_fallbackName;
    QSize m_requestedSize;
    IconState m_state;

    QImage m_result;
    QString m_error;

    QFuture<QImage> m_future;
    QFutureWatcher<QImage> *m_watcher;

    FastIconProvider *m_provider;
};

#endif // FASTICONRESPONSE_H
