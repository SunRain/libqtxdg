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

#include "qtxdgqmlplugin.h"
#include "xdgdirswrapper.h"
#include "xdgiconwrapper.h"
#include "xdgdesktopfilewrapper.h"
#include "xdgautostartwrapper.h"
#include "xdgdefaultappswrapper.h"
#include "xdgmimetypewrapper.h"
#include "xdgmimeappswrapper.h"
#include "xdgmenuwrapper.h"
#include "xdgapplicationsmodel.h"
#include "xdgautostartmodel.h"
#include "xdgmenutreemodel.h"
#include "xdgiconprovider.h"
#include "xdgiconengineinitializer.h"
#include "fasticonprovider.h"
#include "fasticonstats.h"

#include <QtQml/qqml.h>
#include <QQmlEngine>

void QtXdgQmlPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.lxqt.qtxdg"));

    // Register singleton wrappers
    qmlRegisterSingletonType<XdgDirsWrapper>(uri, 6, 0, "XdgDirs",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgDirsWrapper(engine);
        });

    qmlRegisterSingletonType<XdgIconWrapper>(uri, 6, 0, "XdgIcon",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgIconWrapper(engine);
        });

    qmlRegisterSingletonType<XdgAutoStartWrapper>(uri, 6, 0, "XdgAutoStart",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgAutoStartWrapper(engine);
        });

    qmlRegisterSingletonType<XdgDefaultAppsWrapper>(uri, 6, 0, "XdgDefaultApps",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgDefaultAppsWrapper(engine);
        });

    qmlRegisterSingletonType<XdgMimeTypeWrapper>(uri, 6, 0, "XdgMimeType",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgMimeTypeWrapper(engine);
        });

    qmlRegisterSingletonType<XdgMimeAppsWrapper>(uri, 6, 0, "XdgMimeApps",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgMimeAppsWrapper(engine);
        });

    qmlRegisterSingletonType<XdgMenuWrapper>(uri, 6, 0, "XdgMenu",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new XdgMenuWrapper(engine);
        });

    qmlRegisterSingletonType<FastIconStats>(uri, 6, 0, "FastIconStats",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            return new FastIconStats(engine);
        });

    // Register instantiable types
    qmlRegisterType<XdgDesktopFileWrapper>(uri, 6, 0, "XdgDesktopFile");

    // Phase 2: Register data models
    qmlRegisterType<XdgApplicationsModel>(uri, 6, 0, "XdgApplicationsModel");
    qmlRegisterType<XdgAutoStartModel>(uri, 6, 0, "XdgAutoStartModel");

    // Register enums from XdgApplicationsModel
    qmlRegisterUncreatableMetaObject(
        XdgApplicationsModel::staticMetaObject,
        uri, 6, 0,
        "XdgApplicationsModelEnums",
        QStringLiteral("Error: XdgApplicationsModelEnums is a namespace for enums")
    );

    // Phase 3: Register tree models
    qmlRegisterType<XdgMenuTreeModel>(uri, 6, 0, "XdgMenuTreeModel");

    // 注册模块
    qmlRegisterModule(uri, 6, 0);
}

void QtXdgQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    // 注册图像提供者 "theme" (legacy, synchronous)
    engine->addImageProvider(QStringLiteral("theme"), new XdgIconProvider());

    // 注册新的异步图像提供者 "fasticon" (v2.0, async with caching)
    FastIconProvider *fastProvider = new FastIconProvider();
    engine->addImageProvider(QStringLiteral("fasticon"), fastProvider);

    // Set provider for FastIconStats singleton
    FastIconStats::setProvider(fastProvider);
}
