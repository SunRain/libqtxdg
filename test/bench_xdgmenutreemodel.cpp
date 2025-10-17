/*
 * libqtxdg - An Qt implementation of freedesktop.org xdg specs.
 * Copyright (C) 2025 LXQt team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "bench_xdgmenutreemodel.h"
#include "xdgmenutreemodel.h"

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QFileInfo>

void bench_xdgmenutreemodel::initTestCase()
{
    // 查找可用的菜单文件
    QStringList possibleMenuFiles = {
        QStringLiteral("/etc/xdg/menus/lxqt-applications.menu"),
        QStringLiteral("/etc/xdg/menus/applications.menu"),
        QStringLiteral("/usr/share/menus/applications.menu")
    };

    m_hasValidMenu = false;
    for (const QString &menuFile : possibleMenuFiles) {
        if (QFileInfo::exists(menuFile)) {
            m_testMenuFile = menuFile;
            m_hasValidMenu = true;
            qDebug() << "Using menu file for benchmarks:" << menuFile;
            break;
        }
    }

    if (!m_hasValidMenu) {
        qWarning() << "No valid menu file found, benchmarks will be skipped";
    }
}

void bench_xdgmenutreemodel::cleanupTestCase()
{
}

void bench_xdgmenutreemodel::benchmarkAsyncLoad()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    QBENCHMARK {
        XdgMenuTreeModel model;
        model.setMenuFile(m_testMenuFile);

        QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
        model.loadAsync();
        loadingFinishedSpy.wait(10000);
    }
}

void bench_xdgmenutreemodel::benchmarkTreeTraversal()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    // 预加载模型
    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    loadingFinishedSpy.wait(10000);

    QBENCHMARK {
        int totalItems = 0;

        // 遍历所有分类
        for (int i = 0; i < model.rowCount(); ++i) {
            QModelIndex category = model.index(i, 0);
            totalItems++;

            // 遍历分类中的所有应用
            for (int j = 0; j < model.rowCount(category); ++j) {
                QModelIndex app = model.index(j, 0, category);
                totalItems++;
            }
        }

        Q_UNUSED(totalItems);
    }
}

void bench_xdgmenutreemodel::benchmarkCategorySearch()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    loadingFinishedSpy.wait(10000);

    // 获取一个存在的分类名
    QString categoryName;
    if (model.rowCount() > 0) {
        QModelIndex firstCategory = model.index(0, 0);
        categoryName = model.data(firstCategory, XdgMenuTreeModel::CategoryNameRole).toString();
    }

    QBENCHMARK {
        QModelIndex found = model.findCategory(categoryName);
        Q_UNUSED(found);
    }
}

void bench_xdgmenutreemodel::benchmarkApplicationSearch()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    loadingFinishedSpy.wait(10000);

    // 获取一个存在的分类名
    QString categoryName;
    if (model.rowCount() > 0) {
        QModelIndex firstCategory = model.index(0, 0);
        categoryName = model.data(firstCategory, XdgMenuTreeModel::CategoryNameRole).toString();
    }

    QBENCHMARK {
        QVariantList apps = model.applicationsInCategory(categoryName);
        Q_UNUSED(apps);
    }
}

void bench_xdgmenutreemodel::benchmarkRoleDataAccess()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    loadingFinishedSpy.wait(10000);

    // 找到第一个应用
    QModelIndex appIndex;
    for (int i = 0; i < model.rowCount() && !appIndex.isValid(); ++i) {
        QModelIndex category = model.index(i, 0);
        for (int j = 0; j < model.rowCount(category); ++j) {
            QModelIndex app = model.index(j, 0, category);
            if (model.data(app, XdgMenuTreeModel::TypeRole).toString() == QStringLiteral("application")) {
                appIndex = app;
                break;
            }
        }
    }

    QBENCHMARK {
        // 访问所有Role
        QString type = model.data(appIndex, XdgMenuTreeModel::TypeRole).toString();
        QString desktopId = model.data(appIndex, XdgMenuTreeModel::DesktopIdRole).toString();
        QString iconName = model.data(appIndex, XdgMenuTreeModel::IconNameRole).toString();
        QString comment = model.data(appIndex, XdgMenuTreeModel::CommentRole).toString();
        QString exec = model.data(appIndex, XdgMenuTreeModel::ExecRole).toString();
        QString genericName = model.data(appIndex, XdgMenuTreeModel::GenericNameRole).toString();
        QString desktopFilePath = model.data(appIndex, XdgMenuTreeModel::DesktopFilePathRole).toString();
        QString path = model.data(appIndex, XdgMenuTreeModel::PathRole).toString();
        bool terminal = model.data(appIndex, XdgMenuTreeModel::TerminalRole).toBool();
        bool startupNotify = model.data(appIndex, XdgMenuTreeModel::StartupNotifyRole).toBool();

        Q_UNUSED(type);
        Q_UNUSED(desktopId);
        Q_UNUSED(iconName);
        Q_UNUSED(comment);
        Q_UNUSED(exec);
        Q_UNUSED(genericName);
        Q_UNUSED(desktopFilePath);
        Q_UNUSED(path);
        Q_UNUSED(terminal);
        Q_UNUSED(startupNotify);
    }
}

void bench_xdgmenutreemodel::benchmarkReload()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    // 预加载一次
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    loadingFinishedSpy.wait(10000);

    QBENCHMARK {
        model.reload();
        loadingFinishedSpy.wait(10000);
    }
}

QTEST_MAIN(bench_xdgmenutreemodel)
#include "bench_xdgmenutreemodel.moc"
