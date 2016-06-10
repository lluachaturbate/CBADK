import QtQuick 2.5
import QtQuick.Layouts 1.1

Rectangle {
    id: apppanel;
    height: 90;
    width: 330;

    property var drawobject: ({});

    Connections {target: backend; onUpdatePanel: drawobject = object[uiroot.currentViewer.name];}


    ColumnLayout {
        id: applines;
        anchors {fill: parent;}
        spacing: 0;
        state: drawobject ? drawobject.template ? drawobject.template : "" : "";

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
            color: drawobject ? drawobject.row1_value ? "#d5ebf8" : "white" : "white";
            width: parent.width;
            height: parent.height / 3;

            Text {
                id: t11;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t12.left; rightMargin: 5;}
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignRight;
                text: drawobject ? drawobject.row1_label ? drawobject.row1_label : "" : "";
            }
            Text {
                id: t12;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                verticalAlignment: Text.AlignVCenter;
                text: drawobject ? drawobject.row1_value ? drawobject.row1_value : "" : "";
            }
        }

        Rectangle {
            id: line2;
            color: drawobject ? drawobject.row2_value ? "#f2f9fd" : "white" : "white";
            width: parent.width;
            height: parent.height / 3;

            Text {
                id: t21;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t22.left; rightMargin: 5;}
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignRight;
                text: drawobject ? drawobject.row2_label ? drawobject.row2_label : "" : "";
            }
            Text {
                id: t22;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                verticalAlignment: Text.AlignVCenter;
                text: drawobject ? drawobject.row2_value ? drawobject.row2_value : "" : "";
            }
        }

        Rectangle {
            id: line3;
            color: drawobject ? drawobject.row3_value ? "#d5ebf8" : "white" : "white";
            width: parent.width;
            height: parent.height / 3;

            Text {
                id: t31;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: t32.left; rightMargin: 5;}
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignRight;
                text: drawobject ? drawobject.row3_label ? drawobject.row3_label : "" : "";
            }
            Text {
                id: t32;
                anchors {top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;}
                verticalAlignment: Text.AlignVCenter;
                text: drawobject ? drawobject.row3_value ? drawobject.row3_value : "" : "";
            }
        }
    }
}
