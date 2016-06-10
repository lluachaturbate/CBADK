#ifndef CBOBJECTIMPL_H
#define CBOBJECTIMPL_H

#include <QJsonDocument>
#include <QTimer>
#include <QSet>
#include "cbobjectbase.h"
#include "viewer.h"


//! The cb object with implementation. @sa CBObjectBase and CBJSObject
class CBObjectImpl : public CBObjectBase
{
    Q_OBJECT
public:
    //! cTor. @note QScriptEngine* needed to initialize and reset private variables.
    explicit CBObjectImpl(QScriptEngine *e, QObject *parent = 0);

    //! Calls the scriptfunction defined in onDrawPanel() and emits drawPanelRequest(). @note If there is no function defined emits an invalid QVariant.\n Emits warning() when the object to draw hasn't changes since the last call.
    Q_INVOKABLE void drawPanel();

    //! Calls limitCam_addUsers() for allowed_users and emits limitCamStatusChanged() with the given message.
    Q_INVOKABLE void limitCam_start(const QString &message, const QScriptValue &allowed_users = QScriptValue());

    //! Emits limitCamStatusChanged().
    Q_INVOKABLE void limitCam_stop();

    //! Adds every name in allowed_users to the QSet and emits limitCamAccessChanged(). @note emits warning() if a user was already in the set.
    Q_INVOKABLE void limitCam_addUsers(const QScriptValue &allowed_users);

    //! Remove every name in removed_users from the QSet and emits limitCamAccessChanged(). @note emits warning() if a user wasn't in the set.
    Q_INVOKABLE void limitCam_removeUsers(const QScriptValue &removed_users);

    //! Emits limitCamAccessChanged() for every name and clears the QSet.
    Q_INVOKABLE void limitCam_removeAllUsers();

    //! Returns true if the user is in the QSet, otherwise false.
    Q_INVOKABLE bool limitCam_userHasAccess(const QString& user) const {return m_limitcam_allowed.contains(user);}

    //! Creates a new array in the engine with all names in the QSet. @note Safe to call from c++.
    Q_INVOKABLE QScriptValue limitCam_allUsersWithAccess() const;

    //! Returns true after a show was started with limitCam_start() until limitCam_stop() is called.
    Q_INVOKABLE bool limitCam_isRunning() const {return m_limitcam_active;}

    //! Emits cbLog() with val.toString(). As a convenience converts objects and arrays so JSON.
    Q_INVOKABLE void log(QScriptValue val);

    //! Defines the scriptfunction to be called when drawPanel() is called. @sa drawPanelRequest()
    Q_INVOKABLE void onDrawPanel(const QScriptValue& f);

    //! Defines the scriptfunction to be called when callEnterFunction() is called.
    Q_INVOKABLE void onEnter(const QScriptValue& f);

    //! Defines the scriptfunction to be called when callLeaveFunction() is called.
    Q_INVOKABLE void onLeave(const QScriptValue& f);

    //! Defines the scriptfunction to be called when callMessageFunction() is called.
    Q_INVOKABLE void onMessage(const QScriptValue& f);

    //! Defines the scriptfunction to be called when callTipFunction() is called.
    Q_INVOKABLE void onTip(const QScriptValue& f);

    //! Creates a new QTimer and inserts it together with f into a QHash. The timers interval is set to msecs and connected to onTimer().
    Q_INVOKABLE void setTimeout(const QScriptValue& f, const int& msecs);

    //! Defines the scriptfunction to be called when getTipOptions() is called.
    Q_INVOKABLE void tipOptions(const QScriptValue& f);



    //! Calls the scriptfunction defined in onEnter(). @returns false on script error, otherwise true.
    bool callEnterFunction(QScriptValue event);

    //! Calls the scriptfunction defined in onLeave(). @returns false on script error, otherwise true.
    bool callLeaveFunction(QScriptValue event);

    //! Calls the scriptfunction defined in onMessage(). @returns false on script error, otherwise true.
    bool callMessageFunction(QScriptValue msg);

    //! Calls the scriptfunction defined in onTip(). @returns false on script error, otherwise true.
    bool callTipFunction(QScriptValue tip);

    //! Calls the scriptfunction defined in onDrawPanel(). @returns The object as QVariant or an invalid on error.
    QVariant callDrawPanelFunction(QScriptValue user);

    //! Calls the scriptfunction defined in tipOptions() and returns the object as QVariant.
    Q_INVOKABLE QVariant getTipOptions();

    //! Resets private variables and deletes all timers. Call this before loading a new script.
    void reset();

private:
    QSet<QString> m_limitcam_allowed;
    bool m_limitcam_active = false;
    QHash<QTimer *, QScriptValue> m_timedfunctions;
    //TODO QVariant m_lastdraw;
    QScriptValue m_tipfunction, m_messagefunction, m_enterfunction, m_leavefunction, m_drawpanelfunction, m_tipoptionsfunction;
    QScriptEngine *m_engine;

signals:
    //! Emitted when a limitCam show is started or stopped. @sa limitCam_start(), limitCam_stop()
    void limitCamStatusChanged(bool active, QString message = QString());
    //! Emitted when a Viewer gains or loses access to a limitcam show. @sa limitCam_removeUsers(), limitCam_addUsers(), limitCam_removeAllUsers(), limitCam_start()
    void limitCamAccessChanged(QString username, bool access);
    //! Emitted when log is called.
    void cbLog(QString msg);
    //! Emitted after a call to drawPanel(). @sa callDrawPanelFunction()
    void drawPanelRequest();

private slots:
    //! Removes the item from the QHash, calls the scriptfunction and deletes the QTimer. @warning Sender has to be a QTimer created by setTimeout().
    void onTimer();

public slots:
    //! Pauses all Timers started by setTimeout.
    void pauseTimers();
    //! Resumes all Timers. @sa pauseTimers
    void resumeTimers();
};

#endif // CBOBJECTIMPL_H
