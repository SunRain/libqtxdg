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

#ifndef TST_XDGMENUTREEMODEL_H
#define TST_XDGMENUTREEMODEL_H

#include <QObject>

class tst_xdgmenutreemodel : public QObject {
    Q_OBJECT

private Q_SLOTS:
    // 初始化和清理
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 基础功能测试
    void testConstruction();
    void testLoadMenuFile();
    void testAsyncLoading();
    void testInvalidMenuFile();

    // TreeModel特定测试
    void testTreeStructure();
    void testCategoryCount();
    void testApplicationCount();
    void testFindCategory();
    void testApplicationsInCategory();

    // Role数据测试
    void testApplicationRoles();
    void testCategoryRoles();

    // 性能测试
    void testLoadPerformance();

private:
    QString m_testMenuFile;
    bool m_hasValidMenu;
};

#endif // TST_XDGMENUTREEMODEL_H
