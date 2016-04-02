#ifndef CHATLINE_H
#define CHATLINE_H

#include "viewer.h"
#include <QUrl>
#include <QFileInfo>

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


    //! Returns richtext to be displayed in a view. @note Tipnotices don't contain the tipmessage, call getMessage() for that. @param resolve_images Decides if ":text" should be replaced with images.
    QString getLine(bool resolve_images)
    {
        if (!m_viewer)
        {
            if (m_visible_to == "roomsubject")
                return "room subject changed to \"" + colorizeTags(m_message) + "\"";
            if (m_visible_to == "cblog")
                return "Log: " + m_message;
            else
                return "Notice: " + resolve_images ? resolveImages(m_message) : m_message;
        }
        if (m_tip)
            return QString("<font color=\"%1\">%2</font> tipped %3 token%4").arg(m_viewer->getUserColor()).arg(m_viewer->getName()).arg(m_tip).arg(m_tip > 1 ? "s" : "");
        return QString("<font color=\"%1\">%2</font>: %3").arg(m_viewer->getUserColor()).arg(m_viewer->getName()).arg(resolve_images ? resolveImages(m_message) : m_message);
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

    //! Nests hashtags into a blue font tag
    QString colorizeTags(const QString& text)
    {
        QString tagged = text;
        QString replacement = "<font color=\"blue\">%1</font>";
        int pos = 0;
        QRegExp re("(#\\w+)");
        while ((pos = re.indexIn(tagged, pos)) != -1)
        {
            QString r = replacement.arg(re.cap(1));
            tagged.replace(re.cap(1), r);
            pos += r.length();
        }
        return tagged;
    }


    //! Nests ":text" into image tags when they can be found in the "Image" subfolder.
    QString resolveImages(const QString& text)
    {
        QString resolved = text;
        QString replacement = "<img src=\"%1\">";
        int pos = 0;
        QRegExp re("(?:\\s|^)(:\\w+)(?:\\s|$)");
        while ((pos = re.indexIn(resolved, pos)) != -1)
        {
            QString filename = "Images/" + re.cap(1).mid(1) + ".%1";
            QStringList sufs = QStringList() << "jpg" << "jpeg" << "png" << "gif";
            bool found = false;
            QFileInfo f;
            for (auto i = sufs.constBegin(); i != sufs.constEnd(); ++i)
            {
                f.setFile(filename.arg((*i)));
                if (f.exists())
                {
                    QString r = replacement.arg(QUrl::fromLocalFile(f.absoluteFilePath()).toString());
                    resolved.replace(re.cap(1), r);
                    pos += r.length();
                    found = true;
                    break;
                }
            }
            if (!found)
                pos += re.cap(1).length();
        }
        return resolved;
    }


};

#endif // CHATLINE_H
