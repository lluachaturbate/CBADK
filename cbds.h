#ifndef CBDS_H
#define CBDS_H

#include <QObject>
#include <QJSEngine>
#include <QFile>
#include <QUrl>

#include "cbobject.h"
#include "chatmodel.h"
#include "users.h"

//! Basically the main object. Responsible for creation and connection of the rest and loading apps.
class CBDS : public QObject
{
    Q_OBJECT
    //! Pointer to the chatmodel for qml
    Q_PROPERTY(ChatModel* chat READ getChat CONSTANT)
    //! Pointer to the usermodel for qml
    Q_PROPERTY(Users* users READ getUsers CONSTANT)
    //!Pointer to the current cb object. Will change on every appload. @sa load
    Q_PROPERTY(CBObject* cbobj READ getCBObj NOTIFY cbObjChanged)
public:
    explicit CBDS(QObject *parent = 0);

    //! Opens filename and creates a new QJSEngine as well as CBObject. @sa startApp @note My wild guess is that when you upload a script CB just executes it with a dummy cb object to save the settings_choices array in a database which will later be used to fill the settings object upon starting an app. So that is what i gonna imitate here.
    Q_INVOKABLE bool load(const QUrl& filename);

    //! Creates a new QJSEngine and CBObject with given settings and executes the app
    Q_INVOKABLE bool startApp(const QUrl& filename, const QJsonObject& settings);


    ChatModel* getChat() {return &m_chat;} //!< Returns the chatmodel
    Users* getUsers() {return &m_users;} //!< Returns the usermodel
    CBObject* getCBObj() {return m_cbo;} //!< Returns the CBObject

    //! Creates a new User and adds it to the usermodel as well as connecting its signals
    Q_INVOKABLE User* addUser(const QString& name, const int& tipped=0, const bool& in_fanclub=0, const bool& is_mod=0, const bool& has_tokens=0, const QString &gender="m");

private:
    ChatModel m_chat;
    Users m_users;
    QJSEngine *m_appengine = Q_NULLPTR;

    CBObject *m_cbo = Q_NULLPTR;

    User* connectUser(User* u);

signals:
    void uiRequest(QJsonArray settings_choices); //!< Requests a UI to start an app
    void cbObjChanged(); //!< Emitted when a new app is loaded to inform the UI
    void error(QString msg); //!< Emitted on any error

public slots:
};

#endif // CBDS_H
