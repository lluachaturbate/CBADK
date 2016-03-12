#ifndef VIEWER_H
#define VIEWER_H

#include <QObject>

//! Holds info about the viewer and emits action signals. @todo Font and textcolor are pretty much dummys since the lack of CB documentation for it.
class Viewer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(bool moderator READ isModerator WRITE setModerator NOTIFY dataChanged)
    Q_PROPERTY(bool fanclub READ isFanclubmember WRITE setFanclubmember NOTIFY dataChanged)
    Q_PROPERTY(bool has_tokens READ hasTokens WRITE setHasTokens NOTIFY dataChanged)
    Q_PROPERTY(QString gender READ getGender WRITE setGender NOTIFY dataChanged)
    Q_PROPERTY(int tipped READ getTipped NOTIFY dataChanged)
    Q_PROPERTY(bool limitcamaccess READ hasLimitCamAccess NOTIFY dataChanged)
public:
    explicit Viewer(const QString& username, const int& tips=0, const bool& in_fanclub=0, const bool& is_mod=0, const bool& has_tokens=0, const char& gender='m', QObject *parent = 0);


    //! Values for the different usercolors based on recent tips
    enum TIPVALUES {TIPPED_RECENTLY = 50, TIPPED_ALOT = 250, TIPPED_TONS = 1000};

    //! Returns true if user has rights to see a messages addressed to grp or name.
    bool canRead(const QString& addressed_to) const;

    //! Returns the users color based on recent tips. @sa TIPVALUES, tips @note Those are not the CB colors.\n Check canRead() for those
    QString getUserColor() const;

    //! Adds the new amount and emits tip().
    Q_INVOKABLE void sendTip(const int& amount, const QString& message);


    QString getName() const {return m_name;}
    QString getFont() const {return m_font;}
    QString getTextcolor() const {return m_textcolor;}
    bool isModerator() const {return m_moderator;}
    bool isFanclubmember() const {return m_fanclubmember;}
    bool hasTokens() const {return m_has_tokens;}
    QString getGender() const {return QString::fromLatin1(&m_gender, 1);}
    int getTipped() const {return m_tipped;}
    bool getDebugReadable() const {return m_debug;}
    bool hasLimitCamAccess() const {return m_limitcam;}

    void setModerator(const bool& is_moderator) {if (m_moderator != is_moderator) {m_moderator = is_moderator; emit dataChanged();}}
    void setFanclubmember(const bool& in_fanclub) {if (m_fanclubmember != in_fanclub) {m_fanclubmember = in_fanclub; emit dataChanged();}}
    void setHasTokens(const bool& has_tokens) {if (m_has_tokens != has_tokens) {m_has_tokens = has_tokens; emit dataChanged();}}
    void setGender(const QString& gender) {char g = gender.toLatin1().at(0); if (m_gender != g) {m_gender = g; emit dataChanged();}}
    void setDebugReadable(const bool& on) {if (m_debug != on) {m_debug = on; emit dataChanged();}}
    void setLimitCamAccess(const bool& allow) {if (m_limitcam != allow) {m_limitcam = allow; emit dataChanged();}}
    void setRoomOwner(const bool& owns) {m_roomowner = owns;}

private:
    QString m_name, m_font = "default", m_textcolor = "#000000";
    bool m_roomowner = 0, m_moderator, m_fanclubmember, m_has_tokens, m_debug = 0, m_limitcam = 0;
    char m_gender;
    int m_tipped;

signals:
    //! Emitted when any data is changed.
    void dataChanged();
    //! Emitted after a call to sendTip().
    void tip(int amount, QString message);
    //! Signal to be directly invoked
    void chat(QString message);
    //! Signal to be directly invoked
    void enter();
    //! Signal to be directly invoked
    void leave();
};

#endif // VIEWER_H
