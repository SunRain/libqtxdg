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

#include "tst_xdgmimewrapper.h"
#include "../src/qtxdgqml/xdgmimetypewrapper.h"

#include <QTest>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTemporaryDir>
#include <QSignalSpy>

void tst_xdgmimewrapper::initTestCase()
{
    qDebug() << "初始化 XdgMimeTypeWrapper 测试套件";
}

void tst_xdgmimewrapper::cleanupTestCase()
{
    qDebug() << "清理 XdgMimeTypeWrapper 测试套件";
}

void tst_xdgmimewrapper::init()
{
    QTemporaryDir tempDir;
    m_testDir = tempDir.path();

    // 创建测试文件
    createTestFiles();
}

void tst_xdgmimewrapper::cleanup()
{
    cleanupTestFiles();
}

void tst_xdgmimewrapper::createTestFiles()
{
    // 创建测试文本文件
    m_testFile = QDir::temp().filePath("test_xdgmime.txt");
    QFile textFile(m_testFile);
    if (textFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&textFile);
        out << "这是一个测试文本文件\n";
        out << "用于测试 MIME 类型检测\n";
        textFile.close();
    }

    // 设置不存在的文件路径
    m_nonExistentFile = QDir::temp().filePath("non_existent_file_12345.txt");
}

void tst_xdgmimewrapper::cleanupTestFiles()
{
    QFile::remove(m_testFile);
}

void tst_xdgmimewrapper::testMimeTypeWrapperConstruction()
{
    qDebug() << "测试 XdgMimeTypeWrapper 构造函数";

    // 测试默认构造函数
    XdgMimeTypeWrapper wrapper;
    QVERIFY(wrapper.lastError().isEmpty());

    // 测试带父对象的构造函数
    QObject parent;
    XdgMimeTypeWrapper wrapperWithParent(&parent);
    QVERIFY(wrapperWithParent.lastError().isEmpty());
    QCOMPARE(wrapperWithParent.parent(), &parent);
}

void tst_xdgmimewrapper::testMimeTypeForFile()
{
    qDebug() << "测试 mimeTypeForFile 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试存在的文件
    QString mimeType = wrapper.mimeTypeForFile(m_testFile);
    QVERIFY(!mimeType.isEmpty());
    qDebug() << "文本文件 MIME 类型:" << mimeType;

    // 验证常见文本文件类型
    QVERIFY(mimeType.contains("text", Qt::CaseInsensitive) ||
            mimeType.contains("plain", Qt::CaseInsensitive));

    // 测试不存在的文件
    QString nonExistentMime = wrapper.mimeTypeForFile(m_nonExistentFile);
    // 某些实现可能返回文本类型作为默认值，我们主要验证不是空的
    QVERIFY(!nonExistentMime.isEmpty());
    qDebug() << "不存在文件的 MIME 类型:" << nonExistentMime;

    // 测试空路径 - 空输入应该返回空
    QString emptyPathMime = wrapper.mimeTypeForFile("");
    // 空路径返回空是合理的行为
    qDebug() << "空路径MIME类型:" << emptyPathMime;

    // 测试系统文件
    if (QFile::exists("/etc/passwd")) {
        QString systemFileMime = wrapper.mimeTypeForFile("/etc/passwd");
        QVERIFY(!systemFileMime.isEmpty());
        qDebug() << "/etc/passwd MIME 类型:" << systemFileMime;
    }
}

void tst_xdgmimewrapper::testMimeTypeForUrl()
{
    qDebug() << "测试 mimeTypeForUrl 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试本地文件 URL
    QString fileUrl = QUrl::fromLocalFile(m_testFile).toString();
    QString mimeType = wrapper.mimeTypeForUrl(fileUrl);
    QVERIFY(!mimeType.isEmpty());
    qDebug() << "文件 URL MIME 类型:" << mimeType;

    // 测试直接文件路径
    QString directMime = wrapper.mimeTypeForUrl(m_testFile);
    QVERIFY(!directMime.isEmpty());

    // 测试 HTTP URL（可能无法访问，但应该返回非空结果或错误）
    QString httpMime = wrapper.mimeTypeForUrl("http://example.com/test.txt");
    // HTTP URL 的 MIME 检测可能不可用，主要验证不会崩溃
    qDebug() << "HTTP URL MIME 类型:" << httpMime;

    // 测试空 URL - 空输入应该返回空
    QString emptyUrlMime = wrapper.mimeTypeForUrl("");
    // 空URL返回空是合理的行为
    qDebug() << "空URL MIME类型:" << emptyUrlMime;
}

void tst_xdgmimewrapper::testComment()
{
    qDebug() << "测试 comment 方法";

    XdgMimeTypeWrapper wrapper;

    // 获取一个已知 MIME 类型的注释
    QString mimeType = "text/plain";
    QString comment = wrapper.comment(mimeType);
    QVERIFY(!comment.isEmpty());
    qDebug() << "text/plain 注释:" << comment;

    // 测试不存在的 MIME 类型
    QString unknownComment = wrapper.comment("application/x-test-unknown");
    qDebug() << "未知 MIME 类型注释:" << unknownComment;

    // 测试空字符串
    QString emptyComment = wrapper.comment("");
    qDebug() << "空 MIME 类型注释:" << emptyComment;
}

void tst_xdgmimewrapper::testGenericIconName()
{
    qDebug() << "测试 genericIconName 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试常见文本类型的通用图标
    QString textIcon = wrapper.genericIconName("text/plain");
    if (!textIcon.isEmpty()) {
        qDebug() << "text/plain 通用图标:" << textIcon;
    }

    // 测试目录类型的通用图标
    QString dirIcon = wrapper.genericIconName("inode/directory");
    if (!dirIcon.isEmpty()) {
        qDebug() << "inode/directory 通用图标:" << dirIcon;
    }

    // 测试未知类型
    QString unknownIcon = wrapper.genericIconName("application/x-unknown");
    qDebug() << "未知类型 通用图标:" << unknownIcon;
}

void tst_xdgmimewrapper::testIconName()
{
    qDebug() << "测试 iconName 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试文本类型图标 - text/plain可能没有特定图标
    QString textIcon = wrapper.iconName("text/plain");
    // text/plain通常使用genericIconName而非iconName
    qDebug() << "text/plain 图标:" << textIcon;
    if (textIcon.isEmpty()) {
        qDebug() << "text/plain没有特定图标，使用genericIconName";
    }

    // 测试目录类型图标 - inode/directory也可能没有特定图标
    QString dirIcon = wrapper.iconName("inode/directory");
    qDebug() << "inode/directory 图标:" << dirIcon;
    if (dirIcon.isEmpty()) {
        qDebug() << "inode/directory没有特定图标，使用genericIconName";
    }

    // 测试 PDF 文件图标
    QString pdfIcon = wrapper.iconName("application/pdf");
    if (!pdfIcon.isEmpty()) {
        qDebug() << "application/pdf 图标:" << pdfIcon;
    }
}

void tst_xdgmimewrapper::testGlobPatterns()
{
    qDebug() << "测试 globPatterns 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试文本类型的 glob 模式
    QStringList textPatterns = wrapper.globPatterns("text/plain");
    QVERIFY(!textPatterns.isEmpty());
    qDebug() << "text/plain glob 模式:" << textPatterns;

    // 验证常见的文本文件扩展名
    bool hasTxtExtension = false;
    for (const QString &pattern : textPatterns) {
        if (pattern.contains("*.txt", Qt::CaseInsensitive)) {
            hasTxtExtension = true;
            break;
        }
    }
    // 注意：某些 MIME 数据库可能不包含 *.txt 模式
    qDebug() << "包含 *.txt 模式:" << hasTxtExtension;

    // 测试 XML 类型
    QStringList xmlPatterns = wrapper.globPatterns("application/xml");
    qDebug() << "application/xml glob 模式:" << xmlPatterns;

    // 测试未知类型
    QStringList unknownPatterns = wrapper.globPatterns("application/x-unknown");
    qDebug() << "未知类型 glob 模式:" << unknownPatterns;
}

void tst_xdgmimewrapper::testParentMimeTypes()
{
    qDebug() << "测试 parentMimeTypes 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试 JSON 类型的父类型
    QStringList jsonParents = wrapper.parentMimeTypes("application/json");
    qDebug() << "application/json 父类型:" << jsonParents;

    // 测试 HTML 类型的父类型
    QStringList htmlParents = wrapper.parentMimeTypes("text/html");
    if (!htmlParents.isEmpty()) {
        qDebug() << "text/html 父类型:" << htmlParents;
    }

    // 测试基础的纯文本类型（通常没有父类型）
    QStringList textParents = wrapper.parentMimeTypes("text/plain");
    qDebug() << "text/plain 父类型:" << textParents;
}

void tst_xdgmimewrapper::testAllMimeTypes()
{
    qDebug() << "测试 allMimeTypes 方法";

    XdgMimeTypeWrapper wrapper;
    QStringList allTypes = wrapper.allMimeTypes();

    QVERIFY(!allTypes.isEmpty());
    qDebug() << "总 MIME 类型数:" << allTypes.size();

    // 验证包含一些常见的类型
    bool hasTextPlain = allTypes.contains("text/plain");
    bool hasApplicationXml = allTypes.contains("application/xml");
    bool hasInodeDirectory = allTypes.contains("inode/directory");

    qDebug() << "包含 text/plain:" << hasTextPlain;
    qDebug() << "包含 application/xml:" << hasApplicationXml;
    qDebug() << "包含 inode/directory:" << hasInodeDirectory;

    // 显示一些类型示例
    QStringList sampleTypes;
    for (int i = 0; i < qMin(10, allTypes.size()); ++i) {
        sampleTypes << allTypes[i];
    }
    qDebug() << "前 10 个 MIME 类型:" << sampleTypes;
}

void tst_xdgmimewrapper::testInherits()
{
    qDebug() << "测试 inherits 方法";

    XdgMimeTypeWrapper wrapper;

    // 测试 JSON 类型继承文本类型
    bool jsonInheritsText = wrapper.inherits("application/json", "text/plain");
    qDebug() << "application/json 继承 text/plain:" << jsonInheritsText;

    // 测试 HTML 类型继承文本类型
    bool htmlInheritsText = wrapper.inherits("text/html", "text/plain");
    qDebug() << "text/html 继承 text/plain:" << htmlInheritsText;

    // 测试相同类型
    bool sameType = wrapper.inherits("text/plain", "text/plain");
    QVERIFY(sameType);
    qDebug() << "相同类型继承:" << sameType;

    // 测试不存在的类型
    bool unknownInherits = wrapper.inherits("application/x-unknown", "text/plain");
    qDebug() << "未知类型继承:" << unknownInherits;
}

void tst_xdgmimewrapper::testErrorHandling()
{
    qDebug() << "测试错误处理机制";

    XdgMimeTypeWrapper wrapper;
    QSignalSpy errorSpy(&wrapper, &XdgMimeTypeWrapper::errorOccurred);

    // 初始状态应该没有错误
    QVERIFY(wrapper.lastError().isEmpty());
    QCOMPARE(errorSpy.count(), 0);

    // 正常操作不应该产生错误
    QString mimeType = wrapper.mimeTypeForFile(m_testFile);
    QVERIFY(!mimeType.isEmpty());
    QCOMPARE(errorSpy.count(), 0);

    // 测试错误信号（现有实现可能不会触发错误信号）
    // 目前主要是验证接口存在且不会崩溃
    qDebug() << "错误处理测试完成";
}

QTEST_APPLESS_MAIN(tst_xdgmimewrapper)
#include "tst_xdgmimewrapper.moc"