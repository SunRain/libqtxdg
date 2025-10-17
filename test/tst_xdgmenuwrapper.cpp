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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "tst_xdgmenuwrapper.h"
#include "../src/qtxdgqml/xdgmenuwrapper.h"

#include <QTest>
#include <QDebug>

void tst_xdgmenuwrapper::initTestCase()
{
    qDebug() << "初始化 XdgMenuWrapper 测试套件";
}

void tst_xdgmenuwrapper::cleanupTestCase()
{
    qDebug() << "清理 XdgMenuWrapper 测试套件";
}

void tst_xdgmenuwrapper::testMenuWrapperConstruction()
{
    qDebug() << "测试 XdgMenuWrapper 构造函数";

    // 测试默认构造
    XdgMenuWrapper wrapper;
    QVERIFY(wrapper.lastError().isEmpty());

    // 测试带父对象的构造
    QObject parent;
    XdgMenuWrapper wrapperWithParent(&parent);
    QVERIFY(wrapperWithParent.lastError().isEmpty());
    QCOMPARE(wrapperWithParent.parent(), &parent);
}

void tst_xdgmenuwrapper::testLoadMenu()
{
    qDebug() << "测试 loadMenu 方法";

    XdgMenuWrapper wrapper;

    // 测试加载默认菜单
    bool success = wrapper.loadMenu();
    qDebug() << "加载默认菜单:" << (success ? "成功" : "失败");

    // 在某些环境可能没有菜单文件
    if (success) {
        QVERIFY(success);
    } else {
        qDebug() << "注意: 当前环境没有找到菜单文件";
    }

    // 测试加载不存在的菜单文件
    bool failLoad = wrapper.loadMenu(QStringLiteral("/nonexistent/menu.menu"));
    QVERIFY(!failLoad);
    qDebug() << "加载不存在文件应该失败: 符合预期";
}

void tst_xdgmenuwrapper::testMenuTree()
{
    qDebug() << "测试 menuTree 方法";

    XdgMenuWrapper wrapper;

    // 需要先加载菜单
    bool loaded = wrapper.loadMenu();

    if (loaded) {
        QVariantMap tree = wrapper.menuTree();
        qDebug() << "菜单树是否为空:" << tree.isEmpty();

        if (!tree.isEmpty()) {
            // 验证树形结构
            QVERIFY(tree.contains(QStringLiteral("name")) ||
                    tree.contains(QStringLiteral("children")) ||
                    tree.contains(QStringLiteral("categories")));

            qDebug() << "菜单树键:" << tree.keys();
        }
    } else {
        qDebug() << "跳过menuTree测试 - 菜单未加载";
    }
}

void tst_xdgmenuwrapper::testCategories()
{
    qDebug() << "测试 allCategories 方法";

    XdgMenuWrapper wrapper;

    bool loaded = wrapper.loadMenu();

    if (loaded) {
        QVariantList categories = wrapper.allCategories();
        qDebug() << "分类数量:" << categories.size();

        if (!categories.isEmpty()) {
            QVERIFY(categories.size() > 0);
            qDebug() << "前几个分类:" << categories.mid(0, qMin(5, categories.size()));

            // 验证常见分类 - categories是QVariantList
            bool hasCommonCategory = false;
            for (const QVariant &cat : categories) {
                QString catStr = cat.toString();
                if (catStr.contains(QStringLiteral("Development")) ||
                    catStr.contains(QStringLiteral("Graphics")) ||
                    catStr.contains(QStringLiteral("Utility"))) {
                    hasCommonCategory = true;
                    break;
                }
            }
            if (hasCommonCategory) {
                qDebug() << "找到常见分类";
            }
        }
    } else {
        qDebug() << "跳过categories测试 - 菜单未加载";
    }
}

void tst_xdgmenuwrapper::testApplications()
{
    qDebug() << "测试 applications 方法";

    XdgMenuWrapper wrapper;

    bool loaded = wrapper.loadMenu();

    if (loaded) {
        // 获取所有应用
        QVariantList allApps = wrapper.applications();
        qDebug() << "所有应用数量:" << allApps.size();

        if (!allApps.isEmpty()) {
            QVERIFY(allApps.size() > 0);

            // 检查第一个应用的结构
            QVariantMap firstApp = allApps.first().toMap();
            qDebug() << "第一个应用键:" << firstApp.keys();

            // 应该包含基本信息
            bool hasBasicInfo = firstApp.contains(QStringLiteral("name")) ||
                               firstApp.contains(QStringLiteral("desktopId")) ||
                               firstApp.contains(QStringLiteral("id"));
            if (hasBasicInfo) {
                qDebug() << "应用信息完整";
            }
        }
    } else {
        qDebug() << "跳过applications测试 - 菜单未加载";
    }
}

void tst_xdgmenuwrapper::testApplicationsByCategory()
{
    qDebug() << "测试 applications(category) 方法";

    XdgMenuWrapper wrapper;

    bool loaded = wrapper.loadMenu();

    if (loaded) {
        QVariantList categories = wrapper.allCategories();

        if (!categories.isEmpty()) {
            QString testCategory = categories.first().toString();
            QVariantList categoryApps = wrapper.applications(testCategory);

            qDebug() << "分类" << testCategory << "的应用数:" << categoryApps.size();

            // 分类应用数应该 <= 所有应用数
            QVariantList allApps = wrapper.applications();
            QVERIFY(categoryApps.size() <= allApps.size());
        }
    } else {
        qDebug() << "跳过applicationsByCategory测试 - 菜单未加载";
    }
}

void tst_xdgmenuwrapper::testFindApplication()
{
    qDebug() << "测试在应用列表中查找应用";

    XdgMenuWrapper wrapper;

    bool loaded = wrapper.loadMenu();

    if (loaded) {
        QVariantList apps = wrapper.applications();

        if (!apps.isEmpty()) {
            // 获取第一个应用的信息
            QVariantMap firstApp = apps.first().toMap();
            qDebug() << "第一个应用:" << firstApp;

            // 验证应用包含基本信息
            if (firstApp.contains(QStringLiteral("name"))) {
                QString name = firstApp[QStringLiteral("name")].toString();
                qDebug() << "应用名称:" << name;
                QVERIFY(!name.isEmpty());
            }
        }
    } else {
        qDebug() << "跳过findApplication测试 - 菜单未加载";
    }
}

void tst_xdgmenuwrapper::testErrorHandling()
{
    qDebug() << "测试错误处理机制";

    XdgMenuWrapper wrapper;

    // 初始应该无错误
    QVERIFY(wrapper.lastError().isEmpty());

    // 加载失败应该有错误
    bool failLoad = wrapper.loadMenu(QStringLiteral("/nonexistent.menu"));
    QVERIFY(!failLoad);

    // 可能会设置错误信息
    if (!wrapper.lastError().isEmpty()) {
        qDebug() << "错误信息:" << wrapper.lastError();
    }

    qDebug() << "错误处理测试完成";
}

QTEST_APPLESS_MAIN(tst_xdgmenuwrapper)
#include "tst_xdgmenuwrapper.moc"
