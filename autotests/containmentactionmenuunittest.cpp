/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "menu.h"

#include <QAction>
#include <QTest>

class ContainmentActionMenuUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void menuCanBeDestroyedBeforeRestore();
    void actionLookupIsEmptyBeforeRestore();
};

void ContainmentActionMenuUnitTest::menuCanBeDestroyedBeforeRestore()
{
    auto *menu = new Menu(nullptr, QVariantList());
    delete menu;
}

void ContainmentActionMenuUnitTest::actionLookupIsEmptyBeforeRestore()
{
    Menu menu(nullptr, QVariantList());
    QVERIFY(menu.action(QStringLiteral("missing")) == nullptr);
}

QTEST_MAIN(ContainmentActionMenuUnitTest)

#include "containmentactionmenuunittest.moc"
