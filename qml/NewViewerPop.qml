import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    height: 220;
    color: "#cdf7d4"
    border.color: "lightgreen";
    border.width: 5;
    radius: 5;
    z: 50;

    Behavior on width {NumberAnimation {duration: 200;}}

    function showHide() {
        width = width === 220 ? 0 : 220;
        namefield.text = "";
        tippedspin.value = 0;
        genderbox.currentIndex = 0;
        cmod.checked = false;
        cfc.checked = false;
        cht.checked = true;
    }

    MouseArea {anchors.fill: parent;}
    GridLayout {
        id: grid;
        anchors {fill: parent; margins: 10;}
        columns: 2;

        Text{text: "Name:";}
        TextField {id: namefield; Layout.fillWidth: true;}

        Text{text: "Tipped:";}
        SpinBox {id: tippedspin; minimumValue: 0; maximumValue: 99999; Layout.fillWidth: true;}

        Text{text: "Gender:";}
        ComboBox {id: genderbox; model: ['m', 'f', 'c', 's']; Layout.fillWidth: true;}

        CheckBox{id: cmod; text: "Moderator"; Layout.columnSpan: 2;}
        CheckBox{id: cfc; text: "In Fanclub"; Layout.columnSpan: 2;}
        CheckBox{id: cht; text: "Has Tokens"; checked: true; Layout.columnSpan: 2;}

        Button {text: "Cancel"; onClicked: showHide();}
        Button {
            Layout.fillWidth: true;
            text: "Create";
            onClicked: {
                backend.addViewer(namefield.text, tippedspin.value, cfc.checked, cmod.checked, cht.checked, genderbox.currentText.charCodeAt(0));
                viewercombo.currentIndex = backend.viewers.rowCount() -1;
                showHide();
            }
        }
    }

}
