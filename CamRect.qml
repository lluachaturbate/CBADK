import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Rectangle {
    id: root;
    Layout.minimumHeight: 350;
    Layout.minimumWidth: 330;
    color: "black";


    property bool limitcamactive: false;

    property var drawobject: ({});

    function drawPanel(obj) {
        drawobject = obj;
        applines.state = drawobject.template ? drawobject.template : "";
    }

    Connections {target: backend; onCbObjChanged: {limitcamactive = false; applines.state = ""; drawPanel({}); cam.setState();}}
    Connections {target: chatr; onCurrentUserChanged: cam.setState();}
    Connections {
        target: backend.cbobj;
        onDrawPanelRequest: drawPanel(obj);
        onRoomSubjectChanged: subjecttext.text = subject ? subject : "Llua's room";
        onLimitCamUserAdded: if (username === chatr.currentUserName) cam.setState();
        onLimitCamUserRemoved: if (username === chatr.currentUserName) cam.setState();
        onLimitCamStatusChanged: {if (activated) limitcamtext.text = backend.cbobj.getLimitCamMessage(); limitcamactive = activated; cam.setState();}
    }

    Rectangle {
        id: cam;
        anchors {top: roomsubjectpanel.bottom; left: parent.left; right: parent.right; bottom: panel.top;}
        color: "black";


        function setState() {
            state = limitcamactive ? (chatr.currentUserName === "llua" || backend.cbobj.limitCam_userHasAccess(chatr.currentUserName)) ? "" : "hidden" : "";
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
            source: "images.duckduckgo.com.jpeg";
        }
    }

    Rectangle {
        id: roomsubjectpanel;
        anchors {top: parent.top; left: parent.left; right: parent.right;}
        height: 40;
        color: "white"

        Text {
            id: subjecttext;
            anchors {fill: parent; margins: 7;}
            verticalAlignment: Text.AlignVCenter;
            text: "Llua's room";
        }
    }

    Rectangle {
        id: panel;
        anchors {bottom: parent.bottom; left: parent.left; right: parent.right;}
        height: 90;
        color: "white";

        Rectangle {
            id: tipButton;
            anchors {right: parent.right; verticalCenter: parent.verticalCenter; margins: 10;}
            height: parent.height / 3;
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
                onClicked: { tippopup.visible ? tippopup.visible = false : tippopup.pupUp(backend.cbobj.getTipOptions()); }
            }
        }

        Rectangle {
            id: tippopup;
            anchors {bottom: tipButton.top; right: tipButton.right; bottomMargin: 5;}
            radius: 5;
            border.width: 4;
            border.color: "lightgrey";
            height: 120;
            width: 300;
            z: 50;
            visible: false;

            function pupUp(o) {
                if (Object.getOwnPropertyNames(o).length > 0)
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
            property string textlabel;
            property bool optionsactive;

            GridLayout {
                anchors {fill: parent; margins: 10;}
                columns: 2;


                Text {text:"Enter tip amount:";}
                SpinBox {
                    id: tokenspin;
                    minimumValue: 1;
                    maximumValue: 99999;
                }

                Text {text: tippopup.optionsactive ? tippopup.textlabel : "Include an optional message:"; Layout.columnSpan: 2;}
                TextField {id: tf; visible: tippopup.optionsactive ? false : true; Layout.columnSpan: 2; Layout.fillWidth: true;}
                ComboBox {id: cbchoices; visible: tippopup.optionsactive ? true : false; Layout.columnSpan: 2; Layout.fillWidth: true;}

                Button {
                    Layout.columnSpan: 2;
                    Layout.alignment: Qt.AlignRight;
                    text: "Send tip to broadcaster";
                    onClicked: {
                        tippopup.visible = false;
                        backend.users.getUser(chatr.currentUserIdx).sendTip(tokenspin.value, tippopup.optionsactive ? cbchoices.currentIndex > 0 ? cbchoices.currentText : "" : tf.text);
                    }
                }
            }
        }


        ColumnLayout {
            id: applines;
            anchors { left: parent.left; top: parent.top; bottom: parent.bottom;}
            width: 330;
            spacing: 0;

            states: [
                State {
                    name: "3_rows_of_labels";
                    AnchorChanges {target: t12; anchors.left: parent.horizontalCenter;}
                    AnchorChanges {target: t22; anchors.left: parent.horizontalCenter;}
                    AnchorChanges {target: t32; anchors.left: parent.horizontalCenter;}
                    PropertyChanges {target: t11; color: "#057205";}
                },
                State {
                    name: "3_rows_11_21_31";
                    PropertyChanges {target: t12; horizontalAlignment: Text.AlignHCenter;}
                    PropertyChanges {target: t22; horizontalAlignment: Text.AlignHCenter;}
                    PropertyChanges {target: t32; horizontalAlignment: Text.AlignHCenter;}
                },
                State {
                    name: "3_rows_12_21_31";
                    AnchorChanges {target: t12; anchors.left: parent.horizontalCenter;}
                    PropertyChanges {target: t22; horizontalAlignment: Text.AlignHCenter;}
                    PropertyChanges {target: t32; horizontalAlignment: Text.AlignHCenter;}
                },
                State {
                    name: "3_rows_12_22_31";
                    AnchorChanges {target: t12; anchors.left: parent.horizontalCenter;}
                    AnchorChanges {target: t22; anchors.left: parent.horizontalCenter;}
                    PropertyChanges {target: t32; horizontalAlignment: Text.AlignHCenter;}
                }

            ]

            Rectangle {
                id: line1;
                color: drawobject.row1_value ? "#d5ebf8" : "white";
                width: parent.width;
                height: parent.height / 3;

                Text {
                    id: t11;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t12.left; rightMargin: 5;}
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignRight;
                    text: drawobject.row1_label ? drawobject.row1_label : ""
                }
                Text {
                    id: t12;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                    verticalAlignment: Text.AlignVCenter;
                    text: drawobject.row1_value ? drawobject.row1_value : ""
                }
            }

            Rectangle {
                id: line2;
                color: drawobject.row2_value ? "#f2f9fd" : "white";
                width: parent.width;
                height: parent.height / 3;

                Text {
                    id: t21;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t22.left; rightMargin: 5;}
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignRight;
                    text: drawobject.row2_label ? drawobject.row2_label : ""
                }
                Text {
                    id: t22;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                    verticalAlignment: Text.AlignVCenter;
                    text: drawobject.row2_value ? drawobject.row2_value : ""
                }
            }

            Rectangle {
                id: line3;
                color: drawobject.row3_value ? "#d5ebf8" : "white";
                width: parent.width;
                height: parent.height / 3;

                Text {
                    id: t31;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t32.left; rightMargin: 5;}
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignRight;
                    text: drawobject.row3_label ? drawobject.row3_label : ""
                }
                Text {
                    id: t32;
                    anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                    verticalAlignment: Text.AlignVCenter;
                    text: drawobject.row3_value ? drawobject.row3_value : ""
                }
            }
        }

    }
}
