import QtQuick 2.5
import QtQuick.Controls 1.4

ComboBox {
    height: 24;

    model: ListModel {id: lm;}

    function setInput() {
        input[index].value = textAt(currentIndex);
        input[index].ok = true;
    }
    onCurrentIndexChanged: setInput();

    Component.onCompleted: {
        var o = choices[index];
        var i = 1;
        while (o['choice' + i] != undefined)
        {
            lm.append({text: o['choice' + i]});
            i += 1;
        }

        if (o.defaultValue != undefined)
            currentIndex = find(o.defaultValue);
        else
            setInput();
    }
}
