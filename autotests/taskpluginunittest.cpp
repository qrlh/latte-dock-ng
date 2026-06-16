/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "contextmenuactionsbackend.h"

#include <QTest>

class TaskPluginUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void contextMenuActionsRequireParent();
    void contextMenuActionsRejectInvalidLaunchers();
};

void TaskPluginUnitTest::contextMenuActionsRequireParent()
{
    Latte::Tasks::ContextMenuActionsBackend backend;

    QVERIFY(backend.jumpListActions(QVariant(), nullptr).isEmpty());
    QVERIFY(backend.placesActions(QVariant(), false, nullptr).isEmpty());
    QVERIFY(backend.recentDocumentActions(QVariant(), nullptr).isEmpty());
}

void TaskPluginUnitTest::contextMenuActionsRejectInvalidLaunchers()
{
    Latte::Tasks::ContextMenuActionsBackend backend;
    QObject parent;
    const QVariant invalidLauncher(QUrl(QStringLiteral("not-a-valid-launcher")));

    QVERIFY(backend.jumpListActions(invalidLauncher, &parent).isEmpty());
    QVERIFY(backend.placesActions(invalidLauncher, false, &parent).isEmpty());
    QVERIFY(backend.recentDocumentActions(invalidLauncher, &parent).isEmpty());
}

QTEST_GUILESS_MAIN(TaskPluginUnitTest)

#include "taskpluginunittest.moc"
