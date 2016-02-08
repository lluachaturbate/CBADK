import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4


Rectangle {
    id: appsets;
    anchors {horizontalCenter: parent.horizontalCenter; top: parent.top; }
    width: parent.width * .7;
    z: 100;
    color: "lightgrey";
    border.color: "darkgrey";
    radius: 10;
    border.width: 2;

    property var ar;
    property url fileurl;

    Behavior on height {NumberAnimation {duration: 200;}}
    Timer {running: true; repeat: false; interval: 1; onTriggered: appsets.height = appsets.parent.height;}

    ScrollView {
        anchors {fill: parent; margins: 15; bottomMargin: 40;}

        ColumnLayout {
            Repeater {
                id: rep;
                model: ar;

                RowLayout {
                    width: parent.width;
                    Text {
                        Layout.minimumWidth: 250;
                        Layout.maximumWidth: 350;
                        wrapMode: Text.Wrap;
                        text: modelData.label ? modelData.label : modelData.name;
                    }

                    SpinBox {
                        Layout.fillWidth: true;
                        visible: modelData.type === "int" ? true : false;
                        onValueChanged: ar[index].val = value;
                        minimumValue: modelData.minValue ? modelData.minValue : -99999;
                        maximumValue: modelData.maxValue ? modelData.maxValue : 99999;
                        Component.onCompleted: {if (visible) value = modelData.defaultValue ? modelData.defaultValue : 0;}

                    }

                    ComboBox {
                        Layout.fillWidth: true;
                        visible: modelData.type === "choice" ? true: false;
                        property var m: [];
                        model: m;
                        onCurrentTextChanged: ar[index].val = currentText;
                        Component.onCompleted: {
                            if (visible)
                            {
                                var i = 1;
                                while (ar[index].hasOwnProperty("choice" + i))
                                {
                                    m.push(ar[index]["choice" +i].toString());
                                    i++;
                                }
                                model = m;
                                if (modelData.defaultValue)
                                    currentIndex = find(modelData.defaultValue);
                            }
                        }
                    }

                    TextField {
                        Layout.fillWidth: true;
                        visible: modelData.type === "str" ? true : false;
                        onTextChanged: ar[index].val = text;
                        textColor: length  >= (modelData.minLength ? modelData.minLength : 0) &&  length <= (modelData.maxLength ? modelData.maxLength : 1000) ? "black" : "red";
                        Component.onCompleted: {
                            if (visible)
                            {
                                text = modelData.defaultValue ? modelData.defaultValue : "";
                                ar[index].val = text;
                            }
                        }
                    }
                }
            }


        }
    }

    RowLayout {
        anchors {left: parent.horizontalCenter; right:parent.right; bottom: parent.bottom; margins: 8;}
        height: 25;

        Button {
            text: "Start App";
            onClicked: {
                var ob = {};
                for (var i = 0; i < ar.length; i++)
                    ob[ar[i].name] = ar[i].val;
                backend.startApp(fileurl, ob);
                appsets.destroy();
            }
        }

        Button {
            text: "Cancel";
            onClicked: {appsets.destroy();}
        }
    }
}

