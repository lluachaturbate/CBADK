import QtQuick 2.5
import QtQuick.Controls 2.0


Rectangle {
    id: root;
    width: 698;
    height: 690;

    property var input: [];
    property string requiredcolor: "lightgrey";

    function checkOk() {
        var ok = true;
        for (var i in input)
        {
            if (!input[i].ok)
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            fixtext.opacity = 0;
            startbutton.color = "#f47321";
        }
        else
        {
            startbutton.color = "lightgrey";
            fixtext.opacity = 1;
        }

        return ok;
    }

    function makeText(idx) {
        var t = "";
        if (choices[idx]['label'] != undefined)
            t += choices[idx].label;
        else
            t += choices[idx].name;
        if (choices[idx]['required'] == undefined)
            t += " *";
        else if (choices[idx].required === true)
            t += " *";
        return t;
    }

    ListModel {id: jsonModel;}

    ListView {
        anchors {top: parent.top; left: parent.left; right: parent.right; bottom: bottomrect.top;}
        id: lv;
        model: jsonModel;
        cacheBuffer: 50000;
        spacing: 20;
        delegate: Rectangle {
            width: parent.width;
            height: t.contentHeight > 26 ? t.contentHeight : 26;
            Text {
                id: t;
                width: 435;
                height: parent.height;
                font.pixelSize: 13;
                horizontalAlignment: Text.AlignRight;
                verticalAlignment: Text.AlignVCenter;
                wrapMode: Text.WordWrap;
                text: makeText(index);

                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    ToolTip.text: JSON.stringify(choices[index], null, 2);
                    ToolTip.visible: containsMouse;
                    ToolTip.delay: 1000;
                    ToolTip.timeout: 10000;
                }

            }

            Loader {
                anchors { left: t.right; leftMargin: 5; right: parent.right; rightMargin: 5; verticalCenter: parent.verticalCenter;}
                source: type + ".qml"
            }
        }
    }


    Rectangle {
        id: bottomrect;
        anchors {bottom: parent.bottom; left: parent.left; right: parent.right;}
        height: 50;

        Rectangle {anchors.top: parent.top; height: 1; width: parent.width; color: "lightgrey";}

        Rectangle {
            id: startbutton;
            anchors {verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter;}
            width: 150;
            height: 30;
            radius: 4;
            border.width: 1;
            border.color: "grey";
            color: "#f47321";
            Text {
                anchors {centerIn: parent;}
                color: "white";
                text: "Start App  \u25B6";
                font.pixelSize: 16;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if (!checkOk())
                        root.requiredcolor = "red";
                    else
                    {
                        var sets = {};
                        for (var i in input)
                            sets[input[i].name] = input[i].value;
                        backend.setInput(sets);
                        backend.accept();
                    }
                }
            }
        }

        Rectangle {
            anchors {left: startbutton.right; right: parent.right; top: parent.top; bottom: parent.bottom; margins: 2;}
            Text {
                id: fixtext;
                anchors {centerIn: parent;}
                text: "Please fix the required fields";
                font.bold: true;
                color: "red";
                visible: root.requiredcolor === "red" ? true : false;
            }
        }


        ToolTip {
        }
    }


    Component.onCompleted: {
        for (var i in choices)
        {
            var o = choices[i];
            jsonModel.append(o);
            input.push({name: choices[i].name});
        };
    }
}
