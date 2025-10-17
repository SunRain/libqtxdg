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

#ifndef TST_XDGMIMEAPPSWRAPPER_H
#define TST_XDGMIMEAPPSWRAPPER_H

#include <QObject>

class tst_xdgmimeappswrapper : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // XdgMimeAppsWrapper 测试
    void testMimeAppsWrapperConstruction();
    void testAppsForMimeType();
    void testDefaultApp();
    void testSetDefaultApp();
    void testAddAssociation();
    void testRemoveAssociation();
    void testAllApps();
    void testCategoryApps();
    void testFallbackApps();
    void testRecommendedApps();
    void testDesktopFileInfo();
    void testReset();
    void testErrorHandling();
    void testSignalEmission();

private:
    void createTestDesktopFiles();
    void cleanupTestDesktopFiles();
    QStringList findSystemApplications();

    QString m_testDir;
    QStringList m_testDesktopFiles;
    QString m_sampleMimeType;
};

#endif // TST_XDGMIMEAPPSWRAPPER_H