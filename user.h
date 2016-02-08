#ifndef USER_H
#define USER_H

#include <QObject>

//! Data holder with some properties for qml integration. @note font is only a dummy for the cb msg event. @todo users own font not supported since there is no documentation of it at all.
class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int tips MEMBER tipped NOTIFY tip) //!< How many tokens the user tipped recently
    Q_PROPERTY(QString usercolor READ getUserColor NOTIFY tip) //!< Usercolor based on tips. @sa tips
    Q_PROPERTY(QString name MEMBER name CONSTANT) //!< Captain obvious
    Q_PROPERTY(bool is_mod MEMBER mod CONSTANT)
public:
    explicit User(const QString& username, const int& tips=0, const bool& fc=0, const bool& mod=0, const bool& hast=0, const char& gen='m');

    //! Values for the different usercolors based on recent tips
    enum TIPVALUES {
        TIPPED_RECENTLY = 50,
        TIPPED_ALOT = 250,
        TIPPED_TONS = 1000
    };

    QString name, font = "default", textcolor = "#000000";
    bool broadcaster = 0, mod, fanclub, has_tokens;
    char gender;
    int tipped;

    //! Returns true if user has rights to see a messages addressed to grp or name.
    Q_INVOKABLE bool canRead(const QString& addressed_to) const;

    //! Returns the users color based on recent tips. @sa TIPVALUES, tips @note Those are not the CB colors.\n Check canRead() for those
    QString getUserColor() const;


signals:
    void tip(int amount, QString msg); //!< Emitted when user tips. @sa sendTip()
    void chat(QString message); //!< Emitted when user chats. @sa sendMessage()
    void enter(); //!< Emitted when User enters room. @note Just invoke it to create the event in CBObject::receiveTip()
    void leave(); //!< User leave room

public slots:
    void sendMessage(const QString &msg); //!< Emits chat().
    void sendTip(const int &amount, const QString &msg = QString()); //!< Adds amount to tipped and emits tip().
};

Q_DECLARE_METATYPE(User *)

#endif // USER_H
