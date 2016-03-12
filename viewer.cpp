#include "viewer.h"

Viewer::Viewer(const QString &username, const int &tips, const bool &in_fanclub, const bool &is_mod, const bool &has_tokens, const char &gender, QObject *parent)
    :
      QObject(parent),
      m_name(username),
      m_moderator(is_mod),
      m_fanclubmember(in_fanclub),
      m_has_tokens(has_tokens),
      m_gender(gender),
      m_tipped(tips)
{
}

bool Viewer::canRead(const QString &addressed_to) const
{
    if (addressed_to.isEmpty()) return true;
    if (addressed_to == m_name) return true;
    if (addressed_to == "red") return m_moderator;
    if (addressed_to == "green") return m_fanclubmember;
    if (addressed_to == "lightblue") return m_has_tokens;
    if (addressed_to == "darkblue") return m_tipped >= TIPPED_RECENTLY ? true : false;
    if (addressed_to == "lightpurple") return m_tipped >= TIPPED_ALOT ? true : false;
    if (addressed_to == "darkpurple") return m_tipped >= TIPPED_TONS ? true : false;
    if (addressed_to == "cblog" && m_debug) return true;
    if (addressed_to == "roomsubject") return true;
    return false;
}

QString Viewer::getUserColor() const
{
    if (m_roomowner) return "#FF6347";
    if (m_moderator) return "#FF0000";
    if (m_fanclubmember) return "#00FF00";

    if (m_tipped > 0)
    {
        if (m_tipped < TIPPED_RECENTLY) return "#ADD8E6";
        if (m_tipped < TIPPED_ALOT) return "#0000CD";
        if (m_tipped < TIPPED_TONS) return "#D462FF";
        return "#842DCE";
    }

    if (m_has_tokens) return "#ADD8E6";
    return "#A9A9A9";
}

void Viewer::sendTip(const int &amount, const QString &message)
{
    m_tipped += amount;
    emit dataChanged();
    emit tip(amount, message);
}
