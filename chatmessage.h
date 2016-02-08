#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QObject>

#include "user.h"

//! Simple data holding class for the chatmodel. @todo Maybe a flag for appnotice/tipnote/userchat to avoid the mess of conditional expressions in QML.
class ChatMessage : public QObject
{
    Q_OBJECT
public:
    //! Constructor for appnotices
    ChatMessage(const QString& m, const QString& fcolor="#000000", const QString& bcolor="#FFFFFF", const QString& to_user="", const QString& width="normal");

    //! Constuctor for userchat. @param bgcolor undocumented option on CB message objects
    ChatMessage(const QString& m, User* userp, const QString& bcolor="#FFFFFF");

    QString msg, fg = "#000000", bg = "#FFFFFF", to = "", weight = "normal";

    //! If != 0 indicates that it is chat from user or a tipnote if bg = #ffff33, otherwise expected to be appnotice
    User* user = Q_NULLPTR;

signals:

public slots:
};

#endif // CHATMESSAGE_H
