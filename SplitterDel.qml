import QtQuick 2.5

Rectangle {
    color: styleData.hovered === true ? "lightblue" : "lightgrey";
    width: parent.orientation === Qt.Vertical ? parent.width : 10;
    height: parent.orientation === Qt.Horizontal ? parent.height : 10;
    radius: 5;
}

