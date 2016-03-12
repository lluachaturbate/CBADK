#ifndef CHATLINE_H
#define CHATLINE_H

#include "viewer.h"

//! Data holder for a line in the chat. \n getVisibleTo() and getTipAmount() are is used to identify the type of message:\n Cases where getViewer() == 0:\li getVisibleTo() == "roomsubject": Roomsubject change notice\li getVisibleTo() == "cblog": Logoutput\li Everything else is treated as a normal notice.\n \n  When getViewer() != 0:\li getTipAmount() != 0: Tipnotice\li getTipAmount() == 0: Userchat.\n For tipnotes the tipmessage can be retrieved with getMessage while getLine only returns the tipnotice.
class ChatLine
{
public:
    explicit ChatLine(const QString &message, Viewer* viewer = Q_NULLPTR, const QString &frontcolor = "#000000", const QString &backcolor = "#FFFFFF", const int& tip = 0, const QString &to = "", QString weight = "normal")
        : m_viewer(viewer),
          m_message(message),
          m_foregroundcolor(frontcolor),
          m_backgroundcolor(backcolor),
          m_visible_to(to),
          m_weight(weight),
          m_tip(tip)  {}

    //! Returns richtext to be displayed in a view. @note Tipnotices don't contain the tipmessage, call getMessage() for that.
    QString getLine() const
    {
        if (!m_viewer)
        {
            if (m_visible_to == "roomsubject")
            {
                QString tagged = m_message;
                QString replacement = "<font color=\"blue\">%1</font>";
                int pos = 0;
                QRegExp re("(#\\w+)");
                while ((pos = re.indexIn(tagged, pos)) != -1)
                {
                    QString r = replacement.arg(re.cap(1));
                    tagged.replace(re.cap(1), r);
                    pos += r.length();
                }
                return "room subject changed to \"" + tagged + "\"";
            }
            if (m_visible_to == "cblog")
                return "Log: " + m_message;
            else
                return "Notice: " + m_message;
        }
        if (m_tip)
            return QString("<font color=\"%1\">%2</font> tipped %3 token%4").arg(m_viewer->getUserColor()).arg(m_viewer->getName()).arg(m_tip).arg(m_tip > 1 ? "s" : "");
        return QString("<font color=\"%1\">%2</font>: %3").arg(m_viewer->getUserColor()).arg(m_viewer->getName()).arg(m_message);
    }

    Viewer* getViewer() const {return m_viewer;}
    QString getMessage() const {return m_message;}
    QString getForegroundColor() const {return m_foregroundcolor;}
    QString getBackgroundColor() const {return m_backgroundcolor;}
    QString getVisibleTo() const {return m_visible_to;}
    QString getWeight() const {return m_weight;}
    int getTipAmount() const {return m_tip;}

private:
    Viewer* m_viewer;
    QString m_message, m_foregroundcolor, m_backgroundcolor, m_visible_to, m_weight;
    int m_tip;
};

#endif // CHATLINE_H
