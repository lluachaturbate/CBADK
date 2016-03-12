import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4


Rectangle {
    border.color: "lightgrey";
    border.width: 5;
    radius: 5;

    ScrollView {
        id: sv;
        anchors {top: parent.top; left: parent.left; right: parent.right; bottom: chatline.top; margins: 10;}

        ListView {
            id: chatlv;
            model: backend.chat;

            delegate: Rectangle {
                id: del;
                width: t.contentWidth +5;
                color: model.background;
                height: t.height + 2;

                function boldy() {
                    if (model.weight === "bold") return Font.Bold;
                    if (model.weight === "bolder") return Font.Black;
                    return Font.Normal;
                }

                Text {
                    id: t;
                    y: 1;
                    width: chatlv.width;
                    wrapMode: Text.Wrap;
                    text: model.tip && message ? uiroot.currentViewer.moderator || uiroot.currentViewer.name === backend.cb.room_slug ? line + " -- " + message : line : line;
                    color: model.foreground;
                    font.weight: boldy();
                    font.letterSpacing: model.weight === "bolder" ? .5 : 0;
                    font.pixelSize: sizeslider.value;
                }
            }
            Component.onCompleted: backend.chat.rowsInserted.connect(positionViewAtEnd);
        }
    }

    Slider {
        id: sizeslider;
        anchors {top: parent.top; right: parent.right; margins: 0;}
        width: 120;
        height: 8;
        stepSize: 1.0;
        minimumValue: 12;
        maximumValue: 25;
        value: 15;
    }

    TextField {
        id: chatline;
        anchors {bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10;}
        height: 30;
        focus: true;

        function send() {
            if (chatline.text) {
                uiroot.currentViewer.chat(chatline.text);
                chatline.text = "";
            }
        }
        onAccepted: send();

        Rectangle {
            anchors {right: parent.right; verticalCenter: parent.verticalCenter; margins: 8;}
            height: 20;
            width: 42;
            radius: 5;
            color: "red";

            Text {anchors.centerIn: parent; verticalAlignment: Qt.AlignVCenter; text: "SEND"; color: "white";}
            MouseArea {anchors.fill: parent; onClicked: chatline.send();}
        }
    }
}
