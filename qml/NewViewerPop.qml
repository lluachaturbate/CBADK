import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    height: 260;
    color: "#cdf7d4"
    border.color: "lightgreen";
    border.width: 5;
    radius: 5;
    z: 50;

    property int expandedWidth: 300;

    Behavior on width {NumberAnimation {duration: 200;}}

    function showHide() {
        width = width === expandedWidth ? 0 : expandedWidth;
        namefield.text = "";
        tippedspin.value = 0;
        genderbox.currentIndex = 0;
        cmod.checked = false;
        cfc.checked = false;
        cht.checked = true;
    }

    function create() {
        backend.addViewer(namefield.text, tippedspin.value, cfc.checked, cmod.checked, cht.checked, genderbox.currentText.charCodeAt(0));
        viewercombo.currentIndex = backend.viewers.rowCount() -1;
        showHide();
    }

    function randomInt(min, max) {return Math.floor(Math.random() * (max - min + 1)) + min;}
    function randomBool(chance) {return randomInt(1,100) <= chance;}
    function randomName(minLength, maxLength) {
        var cs = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789";
        var name = "";
        for (var x = 0; x < randomInt(minLength, maxLength); x++)
            name += cs.charAt(randomInt(0, cs.length -1));
        return name;
    }

    function createRandomViewers(amount) {
        var genders = "cfms";
        var name = "";
        for (var i = 0; i < amount; i++)
        {
            name = "";
            while (name.length === 0 || backend.viewers.getViewerByName(name))
                name = randomName(4, 25);

            var tipped = 0;
            if (randomBool(30))
            {
                var chance = randomInt(0,100);
                if (chance >= 0 && chance < 59) tipped = randomInt(1,49);
                if (chance > 60 && chance < 84) tipped = randomInt(50,249);
                if (chance > 85 && chance < 94) tipped = randomInt(250,1000);
                if (chance > 95 && chance <= 100) tipped = randomInt(1000,10000);
            }
            backend.addViewer(name, tipped, randomBool(1), randomBool(2), randomBool(30), genders.charCodeAt(randomInt(0,3)));
        }
        showHide();
    }

    MouseArea {anchors.fill: parent;}

    RowLayout {
        id: rl;
        anchors {left: parent.left; right: parent.right; top: parent.top; margins: 10;}

        SpinBox {
            id: rvspin;
            minimumValue: 1;
            maximumValue: 9999;
            suffix: "   Viewers";
            value: 15;
        }
        Button {
            Layout.fillWidth: true;
            text: "Create random";
            onClicked: createRandomViewers(rvspin.value);
        }
    }

    Rectangle {id: sep; color: "lightgreen"; height: 5; width: parent.width; anchors {top: rl.bottom; margins: 5;}}

    GridLayout {
        id: grid;
        anchors {left: parent.left; right: parent.right; top: sep.bottom; bottom: parent.bottom; margins: 10;}
        columns: 2;


        Text{text: "Name:";}
        TextField {id: namefield; Layout.fillWidth: true; onAccepted: create();}

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
            onClicked: create();
        }
    }

}
