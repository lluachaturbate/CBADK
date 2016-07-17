import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


TextField {
    id: tf;
    height: 22;

    property bool required: true;
    property int maxV: 999999;
    property int minV: -999999;

    validator: IntValidator {bottom: minV; top: maxV;}

    style: TextFieldStyle {
        textColor: "black"
        background: Rectangle {
            radius: 3;
            border.color: isOk() ? "lightgrey" : root.requiredcolor;
            border.width: 1;
        }
    }

    function isOk() {
        if (required)
            return acceptableInput;
        else
            if ((text === "") || (parseInt(text) >= minV && parseInt(text) <= maxV))
                return 1;
        return 0;
    }

    function setInput() {
        input[index].value = parseInt(text);
        input[index].ok = isOk() ? true : false;
        root.checkOk();
    }
    onTextChanged: setInput();


    Rectangle {
        anchors {top: parent.top; bottom: parent.verticalCenter; right: parent.right; topMargin: 1; rightMargin: 3;}
        width: 15;
        color: mau.containsMouse ? "lightblue" : "white";
        Text {anchors.centerIn: parent; text: "\u25B4"; verticalAlignment: Text.AlignVCenter}
        MouseArea {id: mau; anchors.fill: parent; hoverEnabled: true; onReleased: tmu.first = true;
            function increment() {
                tf.text = tf.text ? parseInt(tf.text) +1 : 1;
                if (!tf.acceptableInput)
                    tf.text = tf.maxV;
            }
        }
        Timer {id: tmu; interval: 400; triggeredOnStart: true; running: mau.containsPress; property bool first: true;
            onTriggered: {mau.increment(); interval = first ? 400 : Math.max(1,interval*.9); first = false;} repeat: true;}
    }

    Rectangle {
        anchors {top: parent.verticalCenter; bottom: parent.bottom; right: parent.right; bottomMargin: 1; rightMargin: 3;}
        width: 15;
        color: mad.containsMouse ? "lightblue" : "white";
        Text {anchors.centerIn: parent; text: "\u25BE"; verticalAlignment: Text.AlignVCenter}
        MouseArea {id: mad; anchors.fill: parent; hoverEnabled: true; onReleased: tmd.first = true;
            function decrement() {
                tf.text = tf.text ? parseInt(tf.text) -1 : -1;
                if (!tf.acceptableInput)
                    tf.text = tf.minV;
            }
        }
        Timer {id: tmd; interval: 400; triggeredOnStart: true; running: mad.containsPress; property bool first: true;
            onTriggered: {mad.decrement(); interval = first ? 400 : Math.max(1,interval*.9); first = false;} repeat: true;}
    }

    Component.onCompleted: {
        var o = choices[index];
        if (o["maxValue"] != undefined)
            maxV = o.maxValue;
        if (o["minValue"] != undefined)
            minV = o.minValue;
        if (o["required"] != undefined)
            required = o.required;
        if (o["defaultValue"] != undefined)
            text = o.defaultValue;
        else
            setInput();
    }
}

