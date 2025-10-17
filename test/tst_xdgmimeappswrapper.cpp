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

#include "tst_xdgmimeappswrapper.h"
#include "../src/qtxdgqml/xdgmimeappswrapper.h"

#include <QTest>
#include <QSignalSpy>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

void tst_xdgmimeappswrapper::initTestCase()
{
    qDebug() << "初始化 XdgMimeAppsWrapper 测试套件";
    m_sampleMimeType = QStringLiteral("text/plain");
}

void tst_xdgmimeappswrapper::cleanupTestCase()
{
    qDebug() << "清理 XdgMimeAppsWrapper 测试套件";
}

void tst_xdgmimeappswrapper::init()
{
    // 每个测试前的初始化
}

void tst_xdgmimeappswrapper::cleanup()
{
    // 每个测试后的清理
}

void tst_xdgmimeappswrapper::testMimeAppsWrapperConstruction()
{
    qDebug() << "测试 XdgMimeAppsWrapper 构造函数";

    // 测试默认构造
    XdgMimeAppsWrapper wrapper;
    QVERIFY(wrapper.lastError().isEmpty());

    // 测试带父对象的构造
    QObject parent;
    XdgMimeAppsWrapper wrapperWithParent(&parent);
    QVERIFY(wrapperWithParent.lastError().isEmpty());
    QCOMPARE(wrapperWithParent.parent(), &parent);
}

void tst_xdgmimeappswrapper::testAppsForMimeType()
{
    qDebug() << "测试 appsForMimeType 方法";

    XdgMimeAppsWrapper wrapper;

    // 测试获取text/plain的应用列表
    QVariantList apps = wrapper.appsForMimeType(m_sampleMimeType);
    // 应用列表可能为空（取决于系统配置），只验证方法不崩溃
    qDebug() << "text/plain 应用数:" << apps.size();

    // 验证返回的应用信息结构
    if (!apps.isEmpty()) {
        QVariantMap firstApp = apps.first().toMap();
        QVERIFY(firstApp.contains(QStringLiteral("name")));
        QVERIFY(firstApp.contains(QStringLiteral("fileName")));
        qDebug() << "第一个应用:" << firstApp[QStringLiteral("name")].toString();
    }

    // 测试不存在的MIME类型
    QVariantList emptyApps = wrapper.appsForMimeType(QStringLiteral("application/x-nonexistent-mime"));
    // 可能返回空列表或回退应用
    qDebug() << "不存在MIME类型的应用数:" << emptyApps.size();
}

void tst_xdgmimeappswrapper::testDefaultApp()
{
    qDebug() << "测试 defaultApp 方法";

    XdgMimeAppsWrapper wrapper;

    // 获取text/plain的默认应用
    QString defaultApp = wrapper.defaultApp(m_sampleMimeType);
    qDebug() << "text/plain 默认应用:" << defaultApp;

    // 默认应用可能为空（如果未设置）
    if (!defaultApp.isEmpty()) {
        QVERIFY(QFile::exists(defaultApp) || defaultApp.startsWith(QStringLiteral("/")));
    }
}

void tst_xdgmimeappswrapper::testSetDefaultApp()
{
    qDebug() << "测试 setDefaultApp 方法";

    XdgMimeAppsWrapper wrapper;

    // 注意：这个测试可能需要写权限，在某些环境可能失败
    // 我们主要测试接口存在和不会崩溃

    QString testMime = QStringLiteral("text/plain");
    QVariantList apps = wrapper.appsForMimeType(testMime);

    if (!apps.isEmpty()) {
        QString appFile = apps.first().toMap()[QStringLiteral("fileName")].toString();
        qDebug() << "尝试设置默认应用:" << appFile;

        // 测试设置（可能因权限失败）
        bool result = wrapper.setDefaultApp(testMime, appFile);
        qDebug() << "设置结果:" << result;
        // 不强制要求成功，因为可能没有写权限
    }
}

void tst_xdgmimeappswrapper::testAddAssociation()
{
    qDebug() << "测试 addAssociation 方法";

    XdgMimeAppsWrapper wrapper;

    // 测试添加关联（可能因权限失败）
    QString testMime = QStringLiteral("text/plain");
    QVariantList apps = wrapper.appsForMimeType(testMime);

    if (!apps.isEmpty()) {
        QString appFile = apps.first().toMap()[QStringLiteral("fileName")].toString();
        bool result = wrapper.addAssociation(testMime, appFile);
        qDebug() << "添加关联结果:" << result;
    }
}

void tst_xdgmimeappswrapper::testRemoveAssociation()
{
    qDebug() << "测试 removeAssociation 方法";

    XdgMimeAppsWrapper wrapper;

    // 测试移除关联（可能因权限失败）
    QString testMime = QStringLiteral("text/plain");
    QVariantList apps = wrapper.appsForMimeType(testMime);

    if (!apps.isEmpty()) {
        QString appFile = apps.first().toMap()[QStringLiteral("fileName")].toString();
        bool result = wrapper.removeAssociation(testMime, appFile);
        qDebug() << "移除关联结果:" << result;
    }
}

void tst_xdgmimeappswrapper::testAllApps()
{
    qDebug() << "测试 allApps 方法";

    XdgMimeAppsWrapper wrapper;

    QVariantList allApps = wrapper.allApps();
    QVERIFY(!allApps.isEmpty());
    qDebug() << "所有应用数:" << allApps.size();

    // 验证应用信息完整性
    if (!allApps.isEmpty()) {
        int validApps = 0;
        for (const QVariant &app : allApps) {
            QVariantMap appMap = app.toMap();
            if (appMap.contains(QStringLiteral("name")) &&
                appMap.contains(QStringLiteral("fileName"))) {
                validApps++;
            }
        }
        qDebug() << "有效应用数:" << validApps;
        QVERIFY(validApps > 0);
    }
}

void tst_xdgmimeappswrapper::testCategoryApps()
{
    qDebug() << "测试 categoryApps 方法";

    XdgMimeAppsWrapper wrapper;

    // 测试Development分类
    QVariantList devApps = wrapper.categoryApps(QStringLiteral("Development"));
    qDebug() << "Development 应用数:" << devApps.size();

    // 测试Graphics分类
    QVariantList graphicsApps = wrapper.categoryApps(QStringLiteral("Graphics"));
    qDebug() << "Graphics 应用数:" << graphicsApps.size();

    // 至少应该有一些应用
    QVERIFY(devApps.size() > 0 || graphicsApps.size() > 0);
}

void tst_xdgmimeappswrapper::testFallbackApps()
{
    qDebug() << "测试 fallbackApps 方法";

    XdgMimeAppsWrapper wrapper;

    QVariantList fallbackApps = wrapper.fallbackApps(m_sampleMimeType);
    qDebug() << "text/plain 回退应用数:" << fallbackApps.size();

    // 回退应用可能为空
    for (const QVariant &app : fallbackApps) {
        QVariantMap appMap = app.toMap();
        qDebug() << "  -" << appMap[QStringLiteral("name")].toString();
    }
}

void tst_xdgmimeappswrapper::testRecommendedApps()
{
    qDebug() << "测试 recommendedApps 方法";

    XdgMimeAppsWrapper wrapper;

    QVariantList recommended = wrapper.recommendedApps(m_sampleMimeType);
    qDebug() << "text/plain 推荐应用数:" << recommended.size();

    // 推荐应用列表可能为空（取决于系统配置）
    // 只验证方法正常工作，不强制非空
}

void tst_xdgmimeappswrapper::testDesktopFileInfo()
{
    qDebug() << "测试 desktopFileInfo 方法";

    XdgMimeAppsWrapper wrapper;

    // 获取一个已知的桌面文件
    QVariantList apps = wrapper.appsForMimeType(m_sampleMimeType);
    if (!apps.isEmpty()) {
        QString desktopFile = apps.first().toMap()[QStringLiteral("fileName")].toString();
        QVariantMap info = wrapper.desktopFileInfo(desktopFile);

        QVERIFY(!info.isEmpty());
        QVERIFY(info.contains(QStringLiteral("name")));
        QVERIFY(info.contains(QStringLiteral("fileName")));

        qDebug() << "应用名称:" << info[QStringLiteral("name")].toString();
        qDebug() << "图标名称:" << info[QStringLiteral("iconName")].toString();
        qDebug() << "是否有效:" << info[QStringLiteral("isValid")].toBool();
    }

    // 测试不存在的文件
    QVariantMap emptyInfo = wrapper.desktopFileInfo(QStringLiteral("/nonexistent.desktop"));
    // 应该返回空map或isValid=false
    if (!emptyInfo.isEmpty()) {
        QVERIFY(!emptyInfo[QStringLiteral("isValid")].toBool());
    }
}

void tst_xdgmimeappswrapper::testReset()
{
    qDebug() << "测试 reset 方法";

    XdgMimeAppsWrapper wrapper;

    // 获取初始应用数
    QVariantList appsBefore = wrapper.allApps();
    int countBefore = appsBefore.size();

    // 调用reset
    wrapper.reset();

    // 重新获取应用数
    QVariantList appsAfter = wrapper.allApps();
    int countAfter = appsAfter.size();

    qDebug() << "重置前应用数:" << countBefore;
    qDebug() << "重置后应用数:" << countAfter;

    // 应用数应该保持一致
    QCOMPARE(countBefore, countAfter);
}

void tst_xdgmimeappswrapper::testErrorHandling()
{
    qDebug() << "测试错误处理机制";

    XdgMimeAppsWrapper wrapper;

    // 初始应该无错误
    QVERIFY(wrapper.lastError().isEmpty());

    // 测试各种边界条件
    QVariantList emptyMimeApps = wrapper.appsForMimeType(QString());
    QString emptyDefault = wrapper.defaultApp(QString());

    qDebug() << "空MIME类型测试完成";
}

void tst_xdgmimeappswrapper::testSignalEmission()
{
    qDebug() << "测试信号发射";

    XdgMimeAppsWrapper wrapper;
    QSignalSpy errorSpy(&wrapper, &XdgMimeAppsWrapper::errorOccurred);

    // 初始状态
    QCOMPARE(errorSpy.count(), 0);

    // 执行正常操作
    wrapper.allApps();

    // 正常操作不应该产生错误信号
    QCOMPARE(errorSpy.count(), 0);

    qDebug() << "信号测试完成";
}

// 辅助方法实现
void tst_xdgmimeappswrapper::createTestDesktopFiles()
{
    // 预留：如果需要创建测试桌面文件
}

void tst_xdgmimeappswrapper::cleanupTestDesktopFiles()
{
    // 预留：清理测试桌面文件
}

QStringList tst_xdgmimeappswrapper::findSystemApplications()
{
    // 预留：查找系统应用
    return QStringList();
}

QTEST_APPLESS_MAIN(tst_xdgmimeappswrapper)
#include "tst_xdgmimeappswrapper.moc"
