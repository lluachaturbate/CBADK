import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: approot;
    visible: true;
    width: 1000;
    height: 650;
    title: "CBDS";

    property string lastfileurl;

    FileDialog {
        id: opendiag;
        title: "Choose a File";
        nameFilters: ["JS Files (*.js)", "All files (*)"];
        folder: appPath;
        onAccepted: {
            approot.lastfileurl = fileUrl;
            folder = approot.lastfileurl.substring(0, approot.lastfileurl.lastIndexOf('/'));
            backend.load(fileUrl);
        }
    }

    Connections {
        target: backend.cbobj;
        onCbLog: log.append(/\[(Array|Object)\s\(\d+\)\]:/.test(msg) ? "<font color=\"green\">" + msg + "</font>" : msg);
        onError: log.append("<font color=\"red\">[Error] " + msg + "</font>");
    }
    Connections {
        target: backend;
        onError: log.append("<font color=\"red\">[Error] " + msg + "</font>");
        onCbObjChanged: log.invalidateOld();
        onUiRequest: {
            var comp = Qt.createComponent("Appsets.qml");
            var nui = comp.createObject(approot.contentItem, {"ar": settings_choices, "fileurl": approot.lastfileurl});
            if (nui == null)
            {
                log.append("<font color=\"red\">[Error] Unable to create UI for cb.setting_choices</font>");
            }
        }
    }


    menuBar: MenuBar {
        Menu {
            title: "App";
            MenuItem {
                text: "Load App";
                onTriggered: opendiag.open();
            }
            MenuItem {
                visible: approot.lastfileurl ? true : false;
                text: "Reload App: " + approot.lastfileurl.substring(approot.lastfileurl.lastIndexOf('/') +1);
                onTriggered: backend.load(approot.lastfileurl);
            }

            MenuItem {
                text: "Exit";
                onTriggered: Qt.quit();
            }
        }
    }

    SplitView {
        id: vsplitter;
        anchors {fill:parent;}
        orientation: Qt.Vertical;
        handleDelegate: SplitterDel {}


        SplitView {
            id: topsplitter;
            Layout.minimumHeight: implicitHeight;
            Layout.minimumWidth: implicitWidth;
            handleDelegate: SplitterDel {}

            CamRect {
                id: camr;
            }

            ChatRect {
                id: chatr;
            }
        }

        Rectangle {
            id: botrect;

            ActionRect {
                id: actionr;
                anchors {right: parent.right; top: parent.top; margins: 5;}
            }

            TextArea {
                id: log;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: actionr.left; margins: 5;}
                wrapMode: TextEdit.Wrap;
                textFormat: TextEdit.RichText;

                function invalidateOld() {
                    var old = log.getText(0, log.length);
                    if (old)
                    {
                        log.remove(0, log.length);
                        log.append("<font color=\"grey\">" + old + "</font>");
                        log.append("-----------------------------------------------------------------------------------------------------------");
                    }
                }
            }
        }
    }
}
