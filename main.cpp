#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQml>

#include "cbds.h"

//Qt seems to have problems with plugin integration if statically compiled.
//I didn't look into the issue but grabbed this somewhere from the web, hope that fixes it
#ifdef QT_STATIC
#include <QtPlugin>
#include <QQmlExtensionPlugin>

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
Q_IMPORT_PLUGIN(QmlFolderListModelPlugin)
Q_IMPORT_PLUGIN(QmlSettingsPlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qmlRegisterType<User>();

#ifdef QT_STATIC
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->registerTypes("QtQuick");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Styles");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->registerTypes("QtQuick.Layouts");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->registerTypes("QtQuick.Window");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->registerTypes("QtQuick.Dialogs");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->registerTypes("QtQuick.Dialogs.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QmlSettingsPlugin().instance())->registerTypes("Qt.labs.settings");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QmlFolderListModelPlugin().instance())->registerTypes("Qt.labs.folderlistmodel");
#endif

    QQmlApplicationEngine engine;

#ifdef QT_STATIC
    QQmlEngine *e = qobject_cast<QQmlEngine *>(&engine);
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->initializeEngine(e, "QtQuick");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Controls");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Styles");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->initializeEngine(e, "QtQuick.Layouts");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->initializeEngine(e, "QtQuick.Window");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->initializeEngine(e, "QtQuick.Dialogs");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2DialogsPlugin().instance())->initializeEngine(e, "QtQuick.Dialogs.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QmlSettingsPlugin().instance())->initializeEngine(e, "Qt.labs.settings");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QmlFolderListModelPlugin().instance())->initializeEngine(e, "Qt.labs.folderlistmodel");
#endif


    CBDS c;
    engine.rootContext()->setContextProperty("backend", &c);
    engine.rootContext()->setContextProperty("appPath", QApplication::applicationDirPath());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

