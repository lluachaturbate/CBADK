#ifndef CBOBJECT_H
#define CBOBJECT_H

#include <QObject>
#include <QJSValue>
#include <QJSEngine>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

#include "user.h"

//! cb object that is exposed to the JS engine. @note I put all the implementation in here since the CB API is unlikely to change.
class CBObject : public QObject
{
    Q_OBJECT

    //! cb.room_slug property read only from JS
    Q_PROPERTY(QString room_slug READ getRoomSlug CONSTANT)

    //! cb.settings_choices array
    Q_PROPERTY(QJsonArray settings_choices MEMBER m_settings_choices)

    //! cb.settings object.
    Q_PROPERTY(QJsonObject settings READ getSettings)

public:
    explicit CBObject(QJSEngine* eng, const QJsonObject& settings, QObject *parent = 0);

    //! CB: emits roomSubjectChanged()
    Q_INVOKABLE void changeRoomSubject(const QString& subject);

    //! CB: calls the drawPanel function defined by onDrawPanel() and emits drawPanelRequest()
    Q_INVOKABLE void drawPanel();


    //! CB: Sets m_limitcam_active to true and emits limitCamStatusChanged(), calls limitCam_addUsers() for the allowed_users
    Q_INVOKABLE void limitCam_start(const QString &message, const QJsonArray &allowed_users = QJsonArray());

    //! CB: Sets m_limitcam_active to false and emits limitCamStatusChanged()
    Q_INVOKABLE void limitCam_stop() {m_limitcam_active = false; emit limitCamStatusChanged(false);}

    //! CB: Adds them and emits limitCamUserAdded() for every user.
    Q_INVOKABLE void limitCam_addUsers(const QJsonArray &allowed_users);

    //! CB: Remove them and emits limitCamUserRemoved() for every user
    Q_INVOKABLE void limitCam_removeUsers(const QJsonArray &removed_users);

    //! CB: Clears m_limitcam_allowed and emits limitCamUserRemoved() for every user
    Q_INVOKABLE void limitCam_removeAllUsers();

    //! CB: Check if a particular username is in the list of those allowed to view the cam.
    Q_INVOKABLE bool limitCam_userHasAccess(const QString& user) const {return m_limitcam_allowed.contains(user);}

    //! CB: Returns m_limitcam_allowed as QJsonArray
    Q_INVOKABLE QJsonArray limitCam_allUsersWithAccess() const {return QJsonArray::fromStringList(m_limitcam_allowed.toList());}

    //! CB: Returns m_limitcam_active
    Q_INVOKABLE bool limitCam_isRunning() const {return m_limitcam_active;}


    //! CB: Calls .toString on the object and emits cbLog(). As convinience prints JSON of arrays and objects.
    Q_INVOKABLE void log(QJSValue val);

    //! CB: Sets m_drawpanelfunction
    Q_INVOKABLE void onDrawPanel(const QJSValue& f);

    //! CB: Sets m_enterfunction
    Q_INVOKABLE void onEnter(const QJSValue& f);

    //! CB: Sets m_leavefunction
    Q_INVOKABLE void onLeave(const QJSValue& f);

    //! CB: Sets m_messagefunction
    Q_INVOKABLE void onMessage(const QJSValue& f);

    //! CB: Sets m_tipfunction
    Q_INVOKABLE void onTip(const QJSValue& f);

    //! CB: Emits appNotice()
    Q_INVOKABLE void sendNotice(const QString &message, const QString &to_user ="", const QString &background = "#000000", const QString &foreground = "#FFFFFF", const QString &weight ="", const QString &to_group ="");

    //! CB: Creates a QTimer to call given function f after msecs ms. @sa onJSTimer()
    Q_INVOKABLE void setTimeout(const QJSValue& f, const int& msecs);

    //! CB: Sets m_tipoptionsfunction.
    Q_INVOKABLE void tipOptions(const QJSValue& f);


    //! Calls m_tipoptionsfunction. @returns Empty QJsonObject on failure.
    Q_INVOKABLE QJsonObject getTipOptions();

    //! Returns m_room_slug
    QString getRoomSlug() const {return m_room_slug;}
    //! Returns m_settings;
    QJsonObject getSettings() const {return m_settings;}
    //! Returns m_limitcam_message
    Q_INVOKABLE QString getLimitCamMessage() const {return m_limitcam_message;}


private:
    QJSValue m_tipfunction, m_messagefunction, m_enterfunction, m_leavefunction, m_drawpanelfunction, m_tipoptionsfunction;
    QString m_room_slug = "llua";
    QJsonObject m_settings;
    QJsonArray m_settings_choices;

    QSet<QString> m_limitcam_allowed;
    bool m_limitcam_active = false;
    QString m_limitcam_message;
    QJsonObject m_lastdraw;

    int m_hashidx = 0;
    QHash<int, QJSValue> m_timedfunctions;

    QJSEngine* m_engine;

signals:
    //! Emitted when drawPanel() is called from script
    void drawPanelRequest(QJsonObject obj);
    //! Emitted when changeRoomSubject() is called from script
    void roomSubjectChanged(QString subject);
    //! Emitted when log() is called from the script
    void cbLog(QString msg);
    //! Emitted when sendNotice() is called from the script
    void appNotice(QString message, QString to_user, QString background, QString foreground, QString weight, QString to_group);

    //! Emitted when limitCam is started or stopped
    void limitCamStatusChanged(bool activated);
    //! Emitted when user received access to limited cam
    void limitCamUserAdded(QString username);
    //! Emitted when user lost access to limited cam
    void limitCamUserRemoved(QString username);

    //! Emitted when any error occurs
    void error(QString msg);

    //! Emitted when a users message passed the messagefunction
    void newMessage(QString msg, User* u, QString background);

private slots:
    //! @warning Only to be connected to a QTimer created in setTimeout()
    void onJSTimer();

public slots:
    //! Creates a tip event and calls the function defined in the app. @warning Needs a User as sender.
    void receiveTip(const int &amount, const QString &message = "");

    //! Creates a message event and calls the function defined in the app. @warning Needs a User as sender.
    void receiveChat(const QString &msg);

    //! Creates an enter event and calls the function defined in the app. @warning Needs a User as sender.
    void onUserEnter();

    //! Creates a leave event and calls the function defined in the app. @warning Needs a User as sender.
    void onUserLeave();
};


//! CB: cbjs object. @note Why there are 2 different objects exposed to the JS engine idk. Why not just cb.arrayContains?
class CBJSObject : public QObject
{
    Q_OBJECT
public:
    explicit CBJSObject(QObject *parent = 0);

    //! CB: Returns true if array contains object
    Q_INVOKABLE bool arrayContains(const QJsonArray& array, const QJsonObject& object) const
    {
        if (array.contains(QJsonValue(object)))
            return true;
        return false;
    }

    //! CB: Removes all instances of object from array and returns the new array.
    Q_INVOKABLE QJsonArray arrayRemove(QJsonArray array, const QJsonObject& object) const
    {
        for (auto i = array.begin(); i != array.end(); ++i)
            while((*i).toObject() == object)
                i = array.erase(i) - 1;
        return array;
    }
};



#endif // CBOBJECT_H
