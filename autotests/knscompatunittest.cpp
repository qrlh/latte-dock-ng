/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knscompat.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>

class KnsCompatUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void latestStampStillRepairsMissingControlsOverride();
    void firstRunWritesPatchedOverridesAndStamp();

private:
    void isolateHomeAndData(QTemporaryDir &home, QTemporaryDir &data);
    QString qmlRoot() const;
};

void KnsCompatUnitTest::isolateHomeAndData(QTemporaryDir &home, QTemporaryDir &data)
{
    QVERIFY(home.isValid());
    QVERIFY(data.isValid());

    qputenv("HOME", QFile::encodeName(home.path()));
    qputenv("XDG_DATA_HOME", QFile::encodeName(data.path()));
    QStandardPaths::setTestModeEnabled(false);
}

QString KnsCompatUnitTest::qmlRoot() const
{
    return QDir::homePath() + QStringLiteral("/.local/lib64/qt6/qml");
}

void KnsCompatUnitTest::latestStampStillRepairsMissingControlsOverride()
{
    QTemporaryDir home;
    QTemporaryDir data;
    isolateHomeAndData(home, data);

    const QString stampDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                             + QStringLiteral("/latte-dock-ng");
    QVERIFY(QDir().mkpath(stampDir));
    QFile stamp(stampDir + QStringLiteral("/kns-compat.stamp"));
    QVERIFY(stamp.open(QFile::WriteOnly | QFile::Truncate));
    QCOMPARE(stamp.write("7"), qint64(1));
    stamp.close();

    const QString root = qmlRoot();
    QVERIFY(QDir().mkpath(root + QStringLiteral("/org/kde/kirigami/templates")));
    QVERIFY(QDir().mkpath(root + QStringLiteral("/org/kde/newstuff")));

    QFile templatesQmldir(root + QStringLiteral("/org/kde/kirigami/templates/qmldir"));
    QVERIFY(templatesQmldir.open(QFile::WriteOnly | QFile::Truncate));
    templatesQmldir.close();

    QFile newstuffQmldir(root + QStringLiteral("/org/kde/newstuff/qmldir"));
    QVERIFY(newstuffQmldir.open(QFile::WriteOnly | QFile::Truncate));
    newstuffQmldir.close();

    ensureKnsCompat();

    QVERIFY(QFile::exists(root + QStringLiteral("/org/kde/kirigami/controls/qmldir")));
    QVERIFY(QFile::exists(root + QStringLiteral("/org/kde/kirigami/controls/private/globaltoolbar/HandleButton.qml")));
}

void KnsCompatUnitTest::firstRunWritesPatchedOverridesAndStamp()
{
    QTemporaryDir home;
    QTemporaryDir data;
    isolateHomeAndData(home, data);

    ensureKnsCompat();

    const QString root = qmlRoot();
    QFile drawer(root + QStringLiteral("/org/kde/kirigami/templates/private/DrawerHandle.qml"));
    QVERIFY(drawer.open(QFile::ReadOnly));
    const QByteArray drawerText = drawer.readAll();
    QVERIFY(drawerText.contains("onActiveTranslationChanged"));
    QVERIFY(!QFileInfo(drawer).isSymLink());

    QFile handle(root + QStringLiteral("/org/kde/kirigami/controls/private/globaltoolbar/HandleButton.qml"));
    QVERIFY(handle.open(QFile::ReadOnly));
    const QByteArray handleText = handle.readAll();
    QVERIFY(handleText.contains("onActiveTranslationChanged"));
    QVERIFY(!QFileInfo(handle).isSymLink());

    QFile controlsQmldir(root + QStringLiteral("/org/kde/kirigami/controls/qmldir"));
    QVERIFY(controlsQmldir.open(QFile::ReadOnly));
    QVERIFY(!controlsQmldir.readAll().contains("prefer "));

    QFile stamp(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                + QStringLiteral("/latte-dock-ng/kns-compat.stamp"));
    QVERIFY(stamp.open(QFile::ReadOnly));
    QCOMPARE(stamp.readAll().trimmed(), QByteArray("7"));
}

QTEST_GUILESS_MAIN(KnsCompatUnitTest)

#include "knscompatunittest.moc"
