#include "chatmessage.h"

ChatMessage::ChatMessage(const QString& m, const QString& fcolor, const QString& bcolor, const QString& to_user, const QString& width) :
    msg(m),
    fg(fcolor),
    bg(bcolor),
    to(to_user),
    weight(width)
{
}

ChatMessage::ChatMessage(const QString& m, User* userp, const QString& bcolor) :
    msg(m),
    fg(userp->textcolor),
    bg(bcolor),
    user(userp)
{
}

