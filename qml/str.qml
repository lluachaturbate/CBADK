import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    height: 26;

    property bool required: true;
    property int minLength: 0;

    style: TextFieldStyle {
        textColor: "black"
        background: Rectangle {
            radius: 3;
            border.color: isOk() ? "lightgrey" : root.requiredcolor;
            border.width: 1;
        }
    }

    function isOk() {
        if ((required) && (text.length >= Math.max(1,minLength)))
            return 1;
        if ((!required && text === "") || (!required && text !== "" && text.length >= minLength))
            return 1;
        return 0;
    }

    function setInput() {
        input[index].value = text;
        input[index].ok = isOk() ? true : false;
        root.checkOk();
    }
    onTextChanged: setInput();

    Component.onCompleted: {
        var o = choices[index];
        if (o["maxLength"] != undefined)
            maximumLength = o.maxLength;
        if (o["minLength"] != undefined)
            minLength = o.minLength;
        if (o["required"] != undefined)
            required = o.required;
        if (o["defaultValue"] != undefined)
            text = o.defaultValue.toString();
        else
            setInput();
    }
}
