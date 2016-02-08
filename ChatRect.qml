import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1


Rectangle {
    id: root;
    Layout.minimumHeight: 350;
    Layout.minimumWidth: 100;
    color: "white";

    signal currentUserChanged(int idx);
    property string currentUserName: cbusers.currentText;
    property int currentUserIdx: cbusers.currentIndex;

    ComboBox {
        id: cbusers;
        anchors {top: parent.top; left: parent.left; right: parent.right;}
        textRole: "display";
        model: backend.users;

        onActivated: root.currentUserChanged(index);
    }

    ScrollView {
        id: sv;
        anchors {top: cbusers.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10;}

        ListView {
            id: chatlv;
            model: backend.chat;

            delegate: Rectangle {
                id: del;
                width: t.contentWidth +5;
                color: model.background;
                visible: show;
                height: show ? t.height + 2 : 0;
                property bool show: to ? backend.users.getUser(cbusers.currentIndex).canRead(to) ? true : false : true;
                property bool isTipnote: model.background === "#ffff33" && /^\w+\stipped\s\d+\stokens.*/i.test(message) ? true : false;
                property string prefix: isTipnote ? "<font color=\"" + user.usercolor + "\">" + user.name + "</font> " + /^\w+\s(tipped\s\d+\stokens)/i.exec(message)[1] : user ? "<font color=\"" + user.usercolor + "\">" + user.name + "</font>: " : "Notice: ";

                function fM() {
                    if (isTipnote)
                    {
                        if (!/^\w+\stipped\s\d+\stokens$/i.test(message))
                            if (cbusers.currentText === backend.cbobj.room_slug || cbusers.currentText === /^(\w+)/i.exec(message)[1] || backend.users.getUser(cbusers.currentIndex).is_mod)
                                return prefix + /^\w+\stipped\s\d+\stokens(.+)/i.exec(message)[1];
                        return prefix;
                    }
                    return prefix + message;
                }

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
                    text: del.fM();
                    color: foreground;
                    font.weight: boldy();
                    font.letterSpacing: model.weight === "bolder" ? .5 : 0;
                    font.pixelSize: sizeslider.value;
                }
            }
            Component.onCompleted: backend.chat.rowsInserted.connect(positionViewAtEnd);
        }
    }

    Slider {
        id: sizeslider;
        anchors {top: cbusers.bottom; right: parent.right; margins: 1;}
        width: 70;
        height: 10;
        stepSize: 1.0;
        minimumValue: 12;
        maximumValue: 25;
        value: 15;
    }
}

