/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include <memory>

class QmlSmokeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void latteCoreQmlPluginLoadsFromBuildTree();
    void restoreAnimationLoadsFromSource();
};

class ParabolicTargetStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    qreal zoom() const
    {
        return m_zoom;
    }

    void setZoom(qreal zoom)
    {
        if (qFuzzyCompare(m_zoom, zoom)) {
            return;
        }

        m_zoom = zoom;
        Q_EMIT zoomChanged();
    }

Q_SIGNALS:
    void zoomChanged();

private:
    qreal m_zoom{1.5};
};

class AbilityItemStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *parabolicItem READ parabolicItem CONSTANT)
    Q_PROPERTY(int animationTime READ animationTime CONSTANT)

public:
    explicit AbilityItemStub(QObject *parabolicItem, QObject *parent = nullptr)
        : QObject(parent)
        , m_parabolicItem(parabolicItem)
    {
    }

    QObject *parabolicItem() const
    {
        return m_parabolicItem;
    }

    int animationTime() const
    {
        return 10;
    }

private:
    QObject *m_parabolicItem{nullptr};
};

void QmlSmokeTest::latteCoreQmlPluginLoadsFromBuildTree()
{
    QTemporaryDir importRoot;
    QVERIFY(importRoot.isValid());

    const QString modulePath = importRoot.path() + QStringLiteral("/org/kde/latte/core");
    QVERIFY(QDir().mkpath(modulePath));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_QMLDIR), modulePath + QStringLiteral("/qmldir")));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_PLUGIN), modulePath + QStringLiteral("/liblattecoreplugin.so")));

    QQmlEngine engine;
    engine.addImportPath(importRoot.path());

    QQmlComponent component(&engine);
    component.setData(R"(
import QtQml 2.15
import org.kde.latte.core 0.2

QtObject {
    property int separator: Environment.separatorLength
    property int version: Environment.makeVersion(1, 2, 3)
    property real brightness: Tools.colorBrightness("#ffffff")
    property real lumina: Tools.colorLumina("#000000")
    property bool hasCompositingProperty: WindowSystem.compositingActive === true || WindowSystem.compositingActive === false
}
)",
                      QUrl(QStringLiteral("qrc:/lattecore-smoke.qml")));

    std::unique_ptr<QObject> object(component.create());
    if (!object) {
        qWarning() << component.errors();
    }

    QVERIFY(object);
    QCOMPARE(object->property("separator").toInt(), 5);
    QCOMPARE(object->property("version").toInt(), 0x010203);
    QVERIFY(object->property("brightness").toReal() > 0.99);
    QCOMPARE(object->property("lumina").toReal(), 0.0);
    QCOMPARE(object->property("hasCompositingProperty").toBool(), true);
}

void QmlSmokeTest::restoreAnimationLoadsFromSource()
{
    QQmlEngine engine;
    QQmlContext context(engine.rootContext());
    ParabolicTargetStub parabolicTarget;
    AbilityItemStub abilityItem(&parabolicTarget);
    context.setContextProperty(QStringLiteral("abilityItem"), &abilityItem);

    QQmlComponent component(&engine, QUrl::fromLocalFile(QStringLiteral(LATTE_RESTORE_ANIMATION_QML)));
    std::unique_ptr<QObject> object(component.create(&context));
    if (!object) {
        qWarning() << component.errors();
    }

    QVERIFY(object);
    const auto animations = object->findChildren<QObject *>();
    QObject *zoomAnimation = nullptr;
    for (QObject *animation : animations) {
        if (animation->property("property").toString() == QStringLiteral("zoom")) {
            zoomAnimation = animation;
            break;
        }
    }

    QVERIFY(zoomAnimation);
    QCOMPARE(zoomAnimation->property("target").value<QObject *>(), &parabolicTarget);
    QCOMPARE(zoomAnimation->property("to").toReal(), 1.0);
}

QTEST_MAIN(QmlSmokeTest)

#include "qmlsmoketest.moc"
