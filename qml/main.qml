import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    height: 1004;
    width: 604;
    color: "darkgrey";

Rectangle {
    id: uiroot;
    anchors {fill: parent; margins: 2;}
    color: "lightgrey";

    property var currentViewer;
    property alias currentIndex: viewercombo.currentIndex;



    RowLayout {
        id: viewerrow;
        anchors {top: parent.top; left: parent.left; right: parent.right; margins: 5; rightMargin: 0;}
        spacing: 7;

        Text{text: "Tipped: " + uiroot.currentViewer.tipped;}
        Text {
            text: "Gender: " + uiroot.currentViewer.gender;
            MouseArea {
                anchors {fill: parent;}
                hoverEnabled: true;
                onEntered: parent.color = "blue";
                onExited: parent.color = "black";
                onClicked: {
                    var genders = ['f','m','s','c'];
                    uiroot.currentViewer.gender = genders[genders.indexOf(uiroot.currentViewer.gender) === 3 ? 0 : genders.indexOf(uiroot.currentViewer.gender) + 1];
                }
            }
        }
        CheckBox{id: cmod; text: "Moderator"; onClicked: uiroot.currentViewer.moderator = checked;}
        CheckBox{id: cfan; text: "In Fanclub"; onClicked: uiroot.currentViewer.fanclub = checked;}
        CheckBox{id: ctok; text: "Has tokens"; onClicked: uiroot.currentViewer.has_tokens = checked;}
        Button{text: "Enter"; onClicked: uiroot.currentViewer.enter();}
        Button{text: "Leave"; onClicked: uiroot.currentViewer.leave();}
        ComboBox {
            id: viewercombo;
            Layout.fillWidth: true;
            textRole: "name";
            model: backend.viewers;
            onCurrentIndexChanged: {
                uiroot.currentViewer = backend.viewers.getViewer(viewercombo.currentIndex);
                cmod.checked = uiroot.currentViewer.moderator;
                cfan.checked = uiroot.currentViewer.fanclub;
                ctok.checked = uiroot.currentViewer.has_tokens;
                backend.chat.setViewer(uiroot.currentViewer);
                camrect.apppanel.drawobject = backend.getViewerPanel(uiroot.currentViewer);
            }
            Connections {target: backend.viewers; onModelReset: {viewercombo.currentIndex = -1; viewercombo.currentIndex = viewercombo.currentIndex = 0;}}
        }
        Button{text: "+"; onClicked: newviewerpopup.showHide(); Layout.maximumWidth: 20;}
    }

    NewViewerPop {
        id: newviewerpopup;
        anchors {right: parent.right; top: viewerrow.bottom; topMargin: 5;}
    }

    Rectangle {
        anchors {top: viewerrow.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; topMargin: 5;}
        border.color: "lightgrey";
        border.width: 5;
        radius: 5;


        ChatRect {
            id: chatrect;
            anchors {top: parent.top; bottom: parent.bottom; left: camrect.right; right: parent.right;}
        }

        CamRect {
            id: camrect;
            width: 460
            anchors {top: parent.top; bottom: parent.bottom; left: parent.left;}
        }
    }
}
}
