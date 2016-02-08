#include "user.h"

User::User(const QString &username, const int &tips, const bool &fc, const bool &mod, const bool &hast, const char &gen) :
    name(username),
    mod(mod),
    fanclub(fc),
    has_tokens(hast),
    gender(gen),
    tipped(tips)
{
}

bool User::canRead(const QString &addressed_to) const
{
    if (addressed_to.isEmpty()) return true;
    if (addressed_to == name) return true;
    if (addressed_to == "red") return mod ? true : false;
    if (addressed_to == "green") return fanclub ? true : false;
    if (addressed_to == "lightblue") return has_tokens ? true : false;
    if (addressed_to == "darkblue") return tipped >= TIPPED_RECENTLY ? true : false;
    if (addressed_to == "lightpurple") return tipped >= TIPPED_ALOT ? true : false;
    if (addressed_to == "darkpurple") return tipped >= TIPPED_TONS ? true : false;
    return false;
}

QString User::getUserColor() const
{
    if (broadcaster) return "orange";
    if (fanclub) return "green";
    if (mod) return "red";

    if (tipped > 0)
    {
        if (tipped < TIPPED_RECENTLY) return "lightblue";
        if (tipped < TIPPED_ALOT) return "darkblue";
        if (tipped < TIPPED_TONS) return "#D462FF";
        return "#842DCE";
    }

    if (has_tokens) return "lightblue";
    return "grey";
}

void User::sendMessage(const QString& msg)
{
        emit chat(msg);
}

void User::sendTip(const int& amount, const QString& msg)
{
    tipped += amount;
    emit tip(amount, msg);
}

