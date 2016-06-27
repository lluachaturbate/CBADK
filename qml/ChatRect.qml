import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4


Rectangle {
    color: "lightgrey";
    border.color: "lightgrey";
    border.width: 5;
    radius: 5;


    TabView {
        anchors {fill: parent; margins: 5; topMargin: 2.5;}

        Tab {
            anchors {fill:parent;}
            title: "Chat";

            Rectangle {
                anchors {fill: parent;}

                ScrollView {
                    id: sv;
                    anchors {top: parent.top; left: parent.left; right: parent.right; bottom: chatline.top; margins: 10; rightMargin: 0;}

                    MouseArea {
                        anchors.fill: parent;
                        acceptedButtons: Qt.RightButton;
                        onClicked: popmenu.popup();
                    }

                    ListView {
                        id: chatlv;
                        model: backend.chat;
                        cacheBuffer: 0;

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
                                text: model.tip && message ? uiroot.currentViewer.moderator || uiroot.currentViewer.roomowner ? line + " -- " + message : line : line;
                                color: model.foreground;
                                font.weight: boldy();
                                font.letterSpacing: model.weight === "bolder" ? .5 : 0;
                                font.pixelSize: sizeslider.value;
                            }

                            Menu {
                                id: delpop;
                                MenuItem {text: "Copy message"; onTriggered: backend.chat.copyToClipboard(model.message);}
                                MenuItem {text: "Clear Chat"; onTriggered: backend.chat.clearChat();}
                            }
                            MouseArea {anchors.fill: parent; acceptedButtons: Qt.RightButton; onClicked: delpop.popup();}
                        }

                        Menu {
                            id: popmenu;
                            MenuItem {text: "Clear Chat"; onTriggered: backend.chat.clearChat();}
                        }


                        Component.onCompleted: backend.chat.rowsInserted.connect(positionViewAtEnd);
                    }
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

        }

        Tab {
            id: userstab;
            title: "Users (" + backend.viewers.rowCount() + ")";

            TableView {
                id: usertable;
                anchors {fill: parent;}
                model: backend.viewers;

                function resetIndexHack(idx) {
                    if (idx === uiroot.currentIndex)
                    {
                        uiroot.currentIndex = -1;
                        uiroot.currentIndex = idx;
                    }
                }

                rowDelegate: Rectangle {
                    height: sizeslider.value +6;
                    color: styleData.alternate ? "#f0f0f0" : "white";
                }

                TableViewColumn {
                    role: "name";
                    title: "Name";
                    width: parent.width - 18 - 3 * 75;
                    delegate: Rectangle {
                        color: namemouse.containsMouse ? "lightblue" : styleData.row % 2 ? "#f0f0f0" : "white";
                        Text {
                            anchors {fill: parent; leftMargin: 3;}
                            verticalAlignment: Qt.AlignVCenter;
                            text: styleData.value;
                            font.pixelSize: sizeslider.value;
                            color: namemouse.containsMouse ? "black" : backend.viewers.getViewer(styleData.row).usercolor;
                        }
                        MouseArea {
                            id: namemouse;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onClicked: uiroot.currentIndex = styleData.row;
                        }
                        Behavior on color {ColorAnimation {duration: 100;}}
                    }
                }

                TableViewColumn {
                    role: "moderator";
                    title: "Moderator";
                    resizable: false;
                    horizontalAlignment: Text.AlignHCenter;
                    width: 75;
                    delegate:
                        Rectangle {
                        color: modmouse.containsMouse ? "lightblue" : styleData.row % 2 ? "#f0f0f0" : "white";
                        Text {anchors.centerIn: parent; text: styleData.value; color: styleData.value ? "green" : "red"; font.pixelSize: sizeslider.value;}
                        MouseArea {
                            id: modmouse;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onClicked: {backend.viewers.getViewer(styleData.row).moderator = !backend.viewers.getViewer(styleData.row).moderator; usertable.resetIndexHack(styleData.row);}
                        }
                        Behavior on color {ColorAnimation {duration: 100;}}
                    }
                }

                TableViewColumn {
                    role: "fanclub";
                    title: "Fanclub";
                    resizable: false;
                    horizontalAlignment: Text.AlignHCenter;
                    width: 75;
                    delegate:
                        Rectangle {
                        color: fanmouse.containsMouse ? "lightblue" : styleData.row % 2 ? "#f0f0f0" : "white";
                        Text {anchors.centerIn: parent; text: styleData.value; color: styleData.value ? "green" : "red"; font.pixelSize: sizeslider.value;}
                        MouseArea {
                            id: fanmouse;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onClicked: {backend.viewers.getViewer(styleData.row).fanclub = !backend.viewers.getViewer(styleData.row).fanclub; usertable.resetIndexHack(styleData.row);}
                        }
                        Behavior on color {ColorAnimation {duration: 100;}}
                    }
                }

                TableViewColumn {
                    role: "has_tokens";
                    title: "Has Tokens";
                    resizable: false;
                    horizontalAlignment: Text.AlignHCenter;
                    width: 75;
                    delegate:
                        Rectangle {
                        color: tokmouse.containsMouse ? "lightblue" : styleData.row % 2 ? "#f0f0f0" : "white";
                        Text {anchors.centerIn: parent; text: styleData.value; color: styleData.value ? "green" : "red"; font.pixelSize: sizeslider.value;}
                        MouseArea {
                            id: tokmouse;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            onClicked: {backend.viewers.getViewer(styleData.row).has_tokens = !backend.viewers.getViewer(styleData.row).has_tokens; usertable.resetIndexHack(styleData.row);}
                        }
                        Behavior on color {ColorAnimation {duration: 100;}}
                    }
                }
            }
        }
    }

    Slider {
        id: sizeslider;
        anchors {top: parent.top; right: parent.right; margins: 15;}
        width: 120;
        height: 8;
        stepSize: 1.0;
        minimumValue: 12;
        maximumValue: 25;
        value: 15;
    }


    Connections {target: backend.viewers;onRowsInserted: userstab.title = "Users (" + backend.viewers.rowCount() + ")";}
}
