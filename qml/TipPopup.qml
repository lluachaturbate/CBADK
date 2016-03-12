import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Rectangle {
    id: tippopup;
    radius: 5;
    border.width: 4;
    border.color: "lightgrey";
    height: 120;
    width: 300;
    visible: false;

    function showHide() {
        if (visible)
        {
            tf.text = "";
            visible = false;
            return;
        }

        tokenspin.focus = true;
        var o = backend.cb.getTipOptions();
        if (o)
        {
            var a = [' -- Select One -- '];
            for (var i = 0; i < o.options.length; i++)
                a.push(o.options[i].label);
            cbchoices.model = a;
            textlabel = o.label;
            optionsactive = true;
        }
        else
            optionsactive = false;
        visible = true;
    }

    function accept() {
        tippopup.visible = false;
        uiroot.currentViewer.sendTip(tokenspin.value, tippopup.optionsactive ? cbchoices.currentIndex > 0 ? cbchoices.currentText : "" : tf.text);
    }

    property string textlabel;
    property bool optionsactive;

    MouseArea {anchors.fill: parent;}
    Text {color: "red"; text: "X"; anchors {top: parent.top; right: parent.right; margins: 5;} MouseArea{anchors.fill: parent; onClicked: tippopup.showHide();}}
    GridLayout {
        anchors {fill: parent; margins: 10;}
        columns: 2;


        Text {text:"Enter tip amount:";}
        SpinBox {
            id: tokenspin;
            minimumValue: 1;
            maximumValue: 99999;
            Keys.onReturnPressed: tippopup.accept();
        }

        Text {text: tippopup.optionsactive ? tippopup.textlabel : "Include an optional message:"; Layout.columnSpan: 2;}
        TextField {
            id: tf;
            Layout.columnSpan: 2;
            Layout.fillWidth: true;
            visible: tippopup.optionsactive ? false : true;
            onAccepted: tippopup.accept();
        }

        ComboBox {id: cbchoices; visible: tippopup.optionsactive ? true : false; Layout.columnSpan: 2; Layout.fillWidth: true;}

        Rectangle {
            Layout.columnSpan: 2;
            Layout.alignment: Qt.AlignRight;
            width: t.contentWidth + 20;
            height: 25;
            color: "#f47321";
            radius: 3;

            Text {
                id: t;
                anchors.centerIn: parent;
                color: "white";
                text: "Send tip to broadcaster   \u25B6";
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: tippopup.accept();
            }
        }
    }
}
