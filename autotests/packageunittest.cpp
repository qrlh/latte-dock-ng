/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "lattepackage.h"

#include <KPackage/Package>

#include <QTest>

class PackageUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void lattePackageResolvesShellDefinitions();
};

void PackageUnitTest::lattePackageResolvesShellDefinitions()
{
    KPackage::Package package(new Latte::Package);
    package.setPath(QStringLiteral(LATTE_SOURCE_DIR "/shell/package"));

    QVERIFY(package.filePath("lattedockui").endsWith(QStringLiteral("views/Panel.qml")));
    QVERIFY(package.filePath("widgetexplorerui").endsWith(QStringLiteral("views/WidgetExplorer.qml")));
    QVERIFY(package.filePath("configmodel").endsWith(QStringLiteral("configuration/config.qml")));
    QVERIFY(package.filePath("templates").endsWith(QStringLiteral("templates")));
    QVERIFY(package.filePath("compactapplet").endsWith(QStringLiteral("applet/CompactApplet.qml")));
}

QTEST_GUILESS_MAIN(PackageUnitTest)

#include "packageunittest.moc"
