import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Rectangle {
    Layout.minimumWidth: 460;
    Layout.minimumHeight: 195;
    height: 480;
    border.color: "lightgrey";
    border.width: 5;
    radius: 5;


    Rectangle {
        id: roomsubjectrect;
        anchors {top: parent.top; left: parent.left; right: parent.right; margins: 5;}
        height: 35;

        Text {
            id: roomsubjecttext;
            anchors {fill: parent; margins: 10;}
            verticalAlignment: Text.AlignVCenter;
            clip: true;
            text: backend.cb.room_slug + "'s room";
            Connections {target: backend.cb; onChangeRoomSubject: roomsubjecttext.text = new_subject ? new_subject : backend.cb.room_slug + "'s room";}
        }
        MouseArea {
            id: roomsubjectmouse;
            anchors {fill: parent;}
            hoverEnabled: true;
            onClicked: {roomsubjecttextfield.visible = true; roomsubjecttextfield.focus = true;}
            onEntered: {
                if (roomsubjecttext.contentWidth > roomsubjecttext.width)
                {
                    roomsubjecttext.wrapMode = Text.Wrap;
                    roomsubjectrect.height = roomsubjecttext.contentHeight + roomsubjecttext.anchors.margins *2;
                }
            }
            onExited: {roomsubjecttext.wrapMode = Text.NoWrap; roomsubjectrect.height = 35;}
        }
        TextField {
            id: roomsubjecttextfield;
            visible: false;
            anchors {fill: parent; margins: 5;}
            text: roomsubjecttext.text;
            onAccepted: {visible = false; backend.cb.changeRoomSubject(text);}
        }
    }

    Rectangle {
        id: cam;
        anchors {top: roomsubjectrect.bottom; left: parent.left; right: parent.right; bottom: panel.top; leftMargin: 5; rightMargin: 5;}
        color: "black";
        state: limitcamactive ? uiroot.currentViewer.limitcamaccess  || uiroot.currentViewer.name === backend.cb.room_slug ? "" : "hidden" : "";

        property bool limitcamactive: false;

        Connections {
            target: backend.cb;
            onLimitCamStatusChanged: {cam.limitcamactive = active; limitcamtext.text = message;}
        }

        states: [
            State {
                name: "hidden";
                PropertyChanges {target: cistext; visible: true;}
                PropertyChanges {target: limitcamtext; visible: true;}
                PropertyChanges {target: camimg; visible: false;}
            }
        ]

        Text {
            id: limitcamtext;
            anchors {fill: parent;}
            visible: false;
            color: "white";
            wrapMode: Text.WordWrap;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
        }
        Text {
            id: cistext;
            anchors {horizontalCenter: parent.horizontalCenter;}
            visible: false;
            y: parent.height / 4;
            text: "Cam is Hidden";
            color: "#77c6ff";
            font.pixelSize: 25;
        }

        Image {
            id: camimg;
            anchors {fill: parent}
            mipmap: true;
            fillMode: Image.PreserveAspectFit;
            source: CamImagePath;
        }
    }

    AppPanel {
        id: panel;
        anchors {left: parent.left; bottom: parent.bottom; margins: 5;}
    }

    TipPopup {
        id: tippopup;
        anchors {bottom: tipButton.top; right: tipButton.right; bottomMargin: 5;}
    }

    Rectangle {
        id: tipButton;
        anchors {right: parent.right; verticalCenter: panel.verticalCenter; rightMargin: 20;}
        height: 30;
        width: 100;
        radius: 5;

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#7db262";}
            GradientStop { position: 0.5; color: "#7eb363";}
            GradientStop { position: 1.0; color: "#568b3b";}
        }

        Text {
            anchors {centerIn: parent;}
            color: "white";
            text: "SEND TIP";
        }

        MouseArea {
            anchors {fill: parent;}
            onClicked:  {tippopup.showHide();}
        }
    }
}
