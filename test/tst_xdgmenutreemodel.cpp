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

#include "tst_xdgmenutreemodel.h"
#include "xdgmenutreemodel.h"

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QStandardItem>
#include <QFileInfo>
#include <QElapsedTimer>

void tst_xdgmenutreemodel::initTestCase()
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
            qDebug() << "Using menu file:" << menuFile;
            break;
        }
    }

    if (!m_hasValidMenu) {
        qWarning() << "No valid menu file found, some tests will be skipped";
    }
}

void tst_xdgmenutreemodel::cleanupTestCase()
{
}

void tst_xdgmenutreemodel::init()
{
}

void tst_xdgmenutreemodel::cleanup()
{
}

void tst_xdgmenutreemodel::testConstruction()
{
    XdgMenuTreeModel model;

    // 验证初始状态
    QVERIFY(!model.loaded());
    QVERIFY(!model.loading());
    QCOMPARE(model.totalApplications(), 0);
    QVERIFY(model.lastError().isEmpty());
    QCOMPARE(model.loadTimeMs(), 0);
    QCOMPARE(model.rowCount(), 0);
}

void tst_xdgmenutreemodel::testLoadMenuFile()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QCOMPARE(model.menuFile(), m_testMenuFile);

    // 使用异步加载并等待完成
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    QVERIFY(model.loaded());
    QVERIFY(model.totalApplications() > 0);
}

void tst_xdgmenutreemodel::testAsyncLoading()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingStartedSpy(&model, &XdgMenuTreeModel::loadingStarted);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    QSignalSpy loadingChangedSpy(&model, &XdgMenuTreeModel::loadingChanged);

    model.loadAsync();

    // 验证loading状态被设置
    QVERIFY(model.loading() || loadingChangedSpy.count() > 0);
    QCOMPARE(loadingStartedSpy.count(), 1);

    // 等待加载完成 (最多5秒)
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 验证加载完成
    QVERIFY(model.loaded());
    QVERIFY(!model.loading());
    QVERIFY(model.totalApplications() > 0);
    QVERIFY(model.loadTimeMs() >= 0);

    qDebug() << "Async load completed in" << model.loadTimeMs() << "ms";
    qDebug() << "Total applications:" << model.totalApplications();
}

void tst_xdgmenutreemodel::testInvalidMenuFile()
{
    XdgMenuTreeModel model;
    model.setMenuFile(QStringLiteral("/nonexistent/menu.menu"));

    QSignalSpy errorSpy(&model, &XdgMenuTreeModel::errorOccurred);
    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);

    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    QVERIFY(!model.loaded());
    QVERIFY(!model.lastError().isEmpty());
    QCOMPARE(model.totalApplications(), 0);
}

void tst_xdgmenutreemodel::testTreeStructure()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 验证树结构
    QVERIFY(model.rowCount() > 0);

    // 检查第一个分类
    QModelIndex firstCategory = model.index(0, 0);
    QVERIFY(firstCategory.isValid());

    // 验证类型是分类
    QVariant typeData = model.data(firstCategory, XdgMenuTreeModel::TypeRole);
    QCOMPARE(typeData.toString(), QStringLiteral("category"));

    // 验证分类有子项
    if (model.rowCount(firstCategory) > 0) {
        QModelIndex firstApp = model.index(0, 0, firstCategory);
        QVERIFY(firstApp.isValid());

        // 检查应用类型
        QVariant appTypeData = model.data(firstApp, XdgMenuTreeModel::TypeRole);
        QVERIFY(appTypeData.toString() == QStringLiteral("application") ||
                appTypeData.toString() == QStringLiteral("category"));
    }
}

void tst_xdgmenutreemodel::testCategoryCount()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    int categoryCount = model.categoryCount();
    QVERIFY(categoryCount > 0);
    QCOMPARE(categoryCount, model.rowCount());

    qDebug() << "Category count:" << categoryCount;
}

void tst_xdgmenutreemodel::testApplicationCount()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 应该有应用程序被加载
    QVERIFY(model.totalApplications() > 0);

    qDebug() << "Total applications:" << model.totalApplications();
}

void tst_xdgmenutreemodel::testFindCategory()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 获取第一个分类的名称
    if (model.rowCount() > 0) {
        QModelIndex firstCategory = model.index(0, 0);
        QString categoryName = model.data(firstCategory, XdgMenuTreeModel::CategoryNameRole).toString();

        // 查找该分类
        QModelIndex foundIndex = model.findCategory(categoryName);
        QVERIFY(foundIndex.isValid());
        QCOMPARE(foundIndex, firstCategory);

        qDebug() << "Found category:" << categoryName;
    }

    // 测试查找不存在的分类
    QModelIndex notFoundIndex = model.findCategory(QStringLiteral("NonExistentCategory12345"));
    QVERIFY(!notFoundIndex.isValid());
}

void tst_xdgmenutreemodel::testApplicationsInCategory()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 获取第一个分类的应用列表
    if (model.rowCount() > 0) {
        QModelIndex firstCategory = model.index(0, 0);
        QString categoryName = model.data(firstCategory, XdgMenuTreeModel::CategoryNameRole).toString();

        QVariantList apps = model.applicationsInCategory(categoryName);

        // 如果分类有应用,验证数据结构
        if (!apps.isEmpty()) {
            QVariantMap firstApp = apps.first().toMap();
            QVERIFY(firstApp.contains(QStringLiteral("name")));
            QVERIFY(firstApp.contains(QStringLiteral("desktopId")));
            QVERIFY(firstApp.contains(QStringLiteral("iconName")));
            QVERIFY(firstApp.contains(QStringLiteral("comment")));

            qDebug() << "Category" << categoryName << "has" << apps.count() << "applications";
            qDebug() << "First app:" << firstApp[QStringLiteral("name")].toString();
        }
    }
}

void tst_xdgmenutreemodel::testApplicationRoles()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 查找第一个应用
    bool foundApp = false;
    for (int i = 0; i < model.rowCount() && !foundApp; ++i) {
        QModelIndex category = model.index(i, 0);
        for (int j = 0; j < model.rowCount(category); ++j) {
            QModelIndex app = model.index(j, 0, category);
            if (model.data(app, XdgMenuTreeModel::TypeRole).toString() == QStringLiteral("application")) {
                foundApp = true;

                // 验证所有应用Role
                QString type = model.data(app, XdgMenuTreeModel::TypeRole).toString();
                QString desktopId = model.data(app, XdgMenuTreeModel::DesktopIdRole).toString();
                QString iconName = model.data(app, XdgMenuTreeModel::IconNameRole).toString();
                QString comment = model.data(app, XdgMenuTreeModel::CommentRole).toString();
                QString exec = model.data(app, XdgMenuTreeModel::ExecRole).toString();
                QString genericName = model.data(app, XdgMenuTreeModel::GenericNameRole).toString();
                QString desktopFilePath = model.data(app, XdgMenuTreeModel::DesktopFilePathRole).toString();
                QString path = model.data(app, XdgMenuTreeModel::PathRole).toString();
                bool terminal = model.data(app, XdgMenuTreeModel::TerminalRole).toBool();
                bool startupNotify = model.data(app, XdgMenuTreeModel::StartupNotifyRole).toBool();

                QCOMPARE(type, QStringLiteral("application"));
                QVERIFY(!desktopId.isEmpty());
                // exec可能为空(某些特殊应用)

                qDebug() << "Application roles:";
                qDebug() << "  Desktop ID:" << desktopId;
                qDebug() << "  Icon:" << iconName;
                qDebug() << "  Comment:" << comment;
                qDebug() << "  Exec:" << exec;
                qDebug() << "  Generic Name:" << genericName;
                qDebug() << "  Desktop File:" << desktopFilePath;
                qDebug() << "  Path:" << path;
                qDebug() << "  Terminal:" << terminal;
                qDebug() << "  Startup Notify:" << startupNotify;

                break;
            }
        }
    }

    QVERIFY(foundApp);
}

void tst_xdgmenutreemodel::testCategoryRoles()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(5000));

    // 验证第一个分类的Role
    if (model.rowCount() > 0) {
        QModelIndex category = model.index(0, 0);

        QString type = model.data(category, XdgMenuTreeModel::TypeRole).toString();
        QString categoryName = model.data(category, XdgMenuTreeModel::CategoryNameRole).toString();
        QString iconName = model.data(category, XdgMenuTreeModel::IconNameRole).toString();
        QString display = model.data(category, Qt::DisplayRole).toString();

        QCOMPARE(type, QStringLiteral("category"));
        QVERIFY(!categoryName.isEmpty());
        QVERIFY(!display.isEmpty());

        qDebug() << "Category roles:";
        qDebug() << "  Name:" << categoryName;
        qDebug() << "  Display:" << display;
        qDebug() << "  Icon:" << iconName;
    }
}

void tst_xdgmenutreemodel::testLoadPerformance()
{
    if (!m_hasValidMenu) {
        QSKIP("No valid menu file available");
    }

    XdgMenuTreeModel model;
    model.setMenuFile(m_testMenuFile);

    QElapsedTimer timer;
    timer.start();

    QSignalSpy loadingFinishedSpy(&model, &XdgMenuTreeModel::loadingFinished);
    model.loadAsync();
    QVERIFY(loadingFinishedSpy.wait(10000));  // 最多等待10秒

    qint64 totalElapsed = timer.elapsed();
    int modelReportedTime = model.loadTimeMs();

    qDebug() << "Performance metrics:";
    qDebug() << "  Total time (including async):" << totalElapsed << "ms";
    qDebug() << "  Tree building time:" << modelReportedTime << "ms";
    qDebug() << "  Applications loaded:" << model.totalApplications();
    qDebug() << "  Categories:" << model.categoryCount();

    // 树构建时间应该很快 (< 100ms)
    QVERIFY(modelReportedTime < 100);

    // 总时间取决于菜单文件大小,但应该在合理范围内 (< 2秒)
    QVERIFY(totalElapsed < 2000);
}

QTEST_MAIN(tst_xdgmenutreemodel)
#include "tst_xdgmenutreemodel.moc"
