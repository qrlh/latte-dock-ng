/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "interfaces.h"

#include <QSignalSpy>
#include <QTest>

class InterfacesUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void syncsObjectsFromPlainInterfaceProperties();
    void dynamicPropertyChangesUpdateSyncedObjects();
    void destroyedSyncedObjectsAreCleared();
};

void InterfacesUnitTest::syncsObjectsFromPlainInterfaceProperties()
{
    Latte::Interfaces interfaces;
    QObject iface;
    QObject shortcuts;
    QObject layouts;
    QObject theme;
    QObject settings;
    QObject view;

    iface.setProperty("_latte_globalShortcuts_object", QVariant::fromValue(&shortcuts));
    iface.setProperty("_latte_layoutsManager_object", QVariant::fromValue(&layouts));
    iface.setProperty("_latte_themeExtended_object", QVariant::fromValue(&theme));
    iface.setProperty("_latte_universalSettings_object", QVariant::fromValue(&settings));
    iface.setProperty("_latte_view_object", QVariant::fromValue(&view));

    interfaces.setPlasmoidInterface(&iface);

    QCOMPARE(interfaces.globalShortcuts(), &shortcuts);
    QCOMPARE(interfaces.layoutsManager(), &layouts);
    QCOMPARE(interfaces.themeExtended(), &theme);
    QCOMPARE(interfaces.universalSettings(), &settings);
    QCOMPARE(interfaces.view(), &view);
    QCOMPARE(interfaces.plasmoidInterface(), nullptr);
}

void InterfacesUnitTest::dynamicPropertyChangesUpdateSyncedObjects()
{
    Latte::Interfaces interfaces;
    QObject iface;
    QObject firstView;
    QObject secondView;
    QSignalSpy viewChangedSpy(&interfaces, &Latte::Interfaces::viewChanged);

    iface.setProperty("_latte_view_object", QVariant::fromValue(&firstView));
    interfaces.setPlasmoidInterface(&iface);
    QCOMPARE(interfaces.view(), &firstView);

    iface.setProperty("_latte_view_object", QVariant::fromValue(&secondView));

    QCOMPARE(interfaces.view(), &secondView);
    QVERIFY(viewChangedSpy.count() >= 2);
}

void InterfacesUnitTest::destroyedSyncedObjectsAreCleared()
{
    Latte::Interfaces interfaces;
    QObject iface;
    auto *view = new QObject;
    QSignalSpy viewChangedSpy(&interfaces, &Latte::Interfaces::viewChanged);

    iface.setProperty("_latte_view_object", QVariant::fromValue(view));
    interfaces.setPlasmoidInterface(&iface);
    QCOMPARE(interfaces.view(), view);

    delete view;

    QCOMPARE(interfaces.view(), nullptr);
    QVERIFY(viewChangedSpy.count() >= 2);
}

QTEST_GUILESS_MAIN(InterfacesUnitTest)

#include "interfacesunittest.moc"
