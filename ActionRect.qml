import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4


Rectangle {
    id: root;
    color: "green";
    height: 230;
    width: 235;

    property bool addvis: false;

    Rectangle {
        id: addr;
        anchors {top: parent.top; left: parent.left; right: parent.right;}
        height: 0;
        z: 100;

        Behavior on height {NumberAnimation {duration: 200;}}

        GridLayout {
            anchors {top: parent.top; left: parent.left; right: parent.right; margins: 5;}
            id: addg;
            visible: addvis;
            columns: 2;
            Text {text: "Name:";}
            TextField {
                id: namefield;
                Layout.fillWidth: true;
            }

            Text {text: "Tipped:";}
            SpinBox {
                id: tokenspin
                Layout.fillWidth: true;
                minimumValue: 0;
                maximumValue: 9999999;
            }

            Text{ text: "Gender:";}
            ComboBox {id: cbg; model: ['m', 'f', 'c', 's'];}

            CheckBox {id: cmod; text: "Moderator";}
            CheckBox {id: cfc; text: "In Fanclub";}

            CheckBox {id: cht; text: "Has Tokens"; checked: true; Layout.columnSpan: 2;}

            Button {text: "Cancel"; onClicked: {root.addvis = false; addr.height = 0;}}
            Button {
                Layout.fillWidth: true;
                text: "Create";
                onClicked: {
                    root.addvis = false;
                    addr.height = 0;
                    backend.addUser(namefield.text, tokenspin.value, cfc.checked, cmod.checked, cht.checked, cbg.currentText);
                }
            }
        }
    }

    GridLayout {
        anchors {fill: parent; margins: 10; }
        columns: 2;

        Button {
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            text: "Create user";
            onClicked: {addr.height = root.height; root.addvis = true;}
        }

        Text { text: "User:"; }
        ComboBox {
            id: usercomb;
            Layout.fillWidth: true;
            textRole: "display";
            model: backend.users;
        }

        Text {text: "Tipped:"}
        Text {
            id: tiptext;
            text: backend.users.getUser(usercomb.currentIndex).tips;
        }

        SpinBox {
            id: tipspin;
            width: 80;
            maximumValue: 99999;
            minimumValue: 1;
            prefix: "Tkns  ";
        }
        TextField {
            id: tipmessage;
            placeholderText: "Tipmessage";
            onAccepted: {backend.users.getUser(usercomb.currentIndex).sendTip(tipspin.value, tipmessage.text); tipmessage.text = "";}
        }

        Button {
            text: "Tip";
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            onClicked: {backend.users.getUser(usercomb.currentIndex).sendTip(tipspin.value, tipmessage.text); tipmessage.text = "";}
        }

        Button {text: "Enter"; Layout.preferredWidth: 80; onClicked: backend.users.getUser(usercomb.currentIndex).enter();}
        Button {text: "Leave"; Layout.preferredWidth: 80; onClicked: backend.users.getUser(usercomb.currentIndex).leave();}

        TextField {
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            placeholderText: "Chat";

            onAccepted: {
                backend.users.getUser(usercomb.currentIndex).sendMessage(text);
                text = "";
            }


        }
    }
}
