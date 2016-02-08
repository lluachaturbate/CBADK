#include "cbds.h"

CBDS::CBDS(QObject *parent) : QObject(parent)
{
}

bool CBDS::load(const QUrl &filename)
{
    QFile f(filename.toLocalFile());
    if (f.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QString dummy = "var cb = {"
                        "changeRoomSubject: function (new_subject) { },"
                        "drawPanel: function () { },"
                        "limitCam_start: function (message, allowed_users) { },"
                        "limitCam_stop: function () { },"
                        "limitCam_addUsers: function (allowed_users) { },"
                        "limitCam_removeUsers: function (removed_users) { },"
                        "limitCam_removeAllUsers: function () { },"
                        "limitCam_userHasAccess: function (user) { },"
                        "limitCam_allUsersWithAccess: function () { },"
                        "limitCam_isRunning: function () { },"
                        "log: function (message) { },"
                        "onDrawPanel: function (func) { },"
                        "onEnter: function (func) { },"
                        "onLeave: function (func) { },"
                        "onMessage: function (func) { },"
                        "onTip: function (func) { },"
                        "room_slug: '',"
                        "sendNotice: function (message, to_user, background, foreground, weight, to_group) { },"
                        "setTimeout: function (func, msecs) { },"
                        "settings_choices: [],"
                        "settings: {},"
                        "tipOptions: function (func) { },"
                        "}; var cbjs = {arrayContains: function(array, object) { }, arrayRemove: function(array, object) { }};\n"
                + QString(f.readAll());

        QJSEngine engine;
        QJSValue ret = engine.evaluate(dummy, "", 0);
        if (ret.isError())
        {
            emit error(QString("Error at line %1: %2").arg(ret.property("lineNumber").toString()).arg(ret.toString()));
            return false;
        }
        emit uiRequest(QJsonArray::fromVariantList(engine.globalObject().property("cb").property("settings_choices").toVariant().toList()));
        return true;
    }
    return false;
}

bool CBDS::startApp(const QUrl &filename, const QJsonObject &settings)
{
    QFile f(filename.toLocalFile());
    if (f.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        m_chat.clear();
        if (m_appengine) m_appengine->deleteLater();
        m_appengine = new QJSEngine(this);
        m_appengine->globalObject().setProperty("cbjs", m_appengine->newQObject(new CBJSObject(m_appengine)));
        m_cbo = new CBObject(m_appengine, settings, m_appengine);
        m_appengine->globalObject().setProperty("cb", m_appengine->newQObject(m_cbo));
        connect(m_cbo, &CBObject::appNotice, &m_chat, &ChatModel::onAppNotice);
        connect(m_cbo, &CBObject::newMessage, &m_chat, &ChatModel::onUserChat);

        QList<User *> us = m_users.getList();
        for (auto i = us.constBegin(); i != us.constEnd(); ++i)
        {
            (*i)->disconnect();
            connectUser((*i));
        }

        emit cbObjChanged();
        QJSValue result = m_appengine->evaluate(QString(f.readAll()));
        f.close();
        if (result.isError())
        {
            emit error(QString("Error at line %1: %2").arg(result.property("lineNumber").toString()).arg(result.toString()));
            return false;
        }
        m_chat.addMessage(new ChatMessage(filename.fileName() + " app has started."));
        return true;
    }
    emit error("Can't open file: " + filename.toLocalFile());
    return false;
}

User *CBDS::addUser(const QString &name, const int &tipped, const bool &in_fanclub, const bool &is_mod, const bool &has_tokens, const QString &gender)
{
    User *u = new User(name, tipped, in_fanclub, is_mod, has_tokens, gender.at(0).toLatin1());
    if (m_users.addUser(u))
    {
        if (m_cbo)
            return connectUser(u);
        return u;
    }
    emit error(QString("Unable to add User: \"%1\". \"%1\" is allready in the list or reserved.").arg(u->name));
    u->deleteLater();
    return Q_NULLPTR;
}

User* CBDS::connectUser(User *u)
{
    connect(u, &User::tip, m_cbo, &CBObject::receiveTip);
    connect(u, &User::chat, m_cbo, &CBObject::receiveChat);
    connect(u, &User::leave, m_cbo, &CBObject::onUserLeave);
    connect(u, &User::enter, m_cbo, &CBObject::onUserEnter);
    connect(u, &User::tip, &m_chat, &ChatModel::onUserTip);
    return u;
}

