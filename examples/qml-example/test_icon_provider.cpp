/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org/
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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <XdgIcon>

class XdgIconProvider : public QQuickImageProvider
{
public:
    XdgIconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        if (id.isEmpty()) {
            return QPixmap();
        }

        // Load the icon using QtXdg's icon loading system
        QPixmap pixmap = loadIcon(id, requestedSize);

        if (pixmap.isNull()) {
            // Return a fallback icon if the requested icon is not found
            pixmap = loadIcon(QStringLiteral("application-x-executable"), requestedSize);
        }

        if (size) {
            *size = pixmap.size();
        }

        return pixmap;
    }

private:
    QPixmap loadIcon(const QString &iconName, const QSize &requestedSize)
    {
        QIcon icon = getIcon(iconName);
        if (icon.isNull()) {
            return QPixmap();
        }

        // Determine the optimal size for the icon
        QSize iconSize = requestedSize;
        if (iconSize.isEmpty() || iconSize.width() <= 0 || iconSize.height() <= 0) {
            // Use a reasonable default size
            iconSize = QSize(32, 32);
        }

        return icon.pixmap(iconSize);
    }

    QIcon getIcon(const QString &iconName)
    {
        return XdgIcon::fromTheme(iconName);
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Register our custom icon provider
    engine.addImageProvider(QStringLiteral("theme"), new XdgIconProvider());

    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/QtXdgQmlExample/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}