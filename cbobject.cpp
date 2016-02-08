#include "cbobject.h"

CBJSObject::CBJSObject(QObject *parent) : QObject(parent) {}

CBObject::CBObject(QJSEngine *eng, const QJsonObject &settings, QObject *parent) : QObject(parent)
{
    m_engine = eng;
    m_settings = settings;
}

void CBObject::changeRoomSubject(const QString& subject)
{
    emit roomSubjectChanged(subject);
}

void CBObject::drawPanel()
{
    if (m_drawpanelfunction.isCallable())
    {
        QJSValue j = m_drawpanelfunction.call();
        if (!j.isError())
        {
            QJsonObject o = QJsonDocument::fromVariant(j.toVariant()).object();
            if (o == m_lastdraw)
                emit error(QString("cb.drawPanel() called without any data changed. ( %1 )").arg(QString(QJsonDocument(o).toJson())));
            m_lastdraw = o;
            emit drawPanelRequest(o);
        }
        else
            emit error("Error in function given to cb.onDrawPanel(): " + j.toString());
    }
    else
        emit error("Argument for cb.onDrawPanel() has to be a function.");
}

void CBObject::limitCam_start(const QString& message, const QJsonArray& allowed_users)
{
    m_limitcam_message = message;
    limitCam_addUsers(allowed_users);
    m_limitcam_active = true;
    emit limitCamStatusChanged(true);
}

void CBObject::limitCam_addUsers(const QJsonArray& allowed_users)
{
    for (auto i = allowed_users.constBegin(); i != allowed_users.constEnd(); ++i)
    {
        if (m_limitcam_allowed.contains((*i).toString()))
            emit error(QString("cb.limitCam_addUsers(): Trying to add a user (%1) that's allready in the list.").arg((*i).toString()));
        else
        {
            m_limitcam_allowed << (*i).toString();
            emit limitCamUserAdded((*i).toString());
        }
    }
}

void CBObject::limitCam_removeUsers(const QJsonArray& removed_users)
{
    for (auto i = removed_users.constBegin(); i != removed_users.constEnd(); ++i)
    {
        if (!m_limitcam_allowed.remove((*i).toString()))
            emit error(QString("cb.limitCam_removeUsers(): Trying to remove a user (%1) that wasn't in the list.").arg((*i).toString()));
        else
            emit limitCamUserRemoved((*i).toString());
    }
}

void CBObject::limitCam_removeAllUsers()
{
    for (auto i = m_limitcam_allowed.constBegin(); i != m_limitcam_allowed.constEnd(); ++i)
        emit limitCamUserRemoved((*i));
    m_limitcam_allowed.clear();
}

void CBObject::log(QJSValue val)
{
    QString s;
    if (val.isArray())
        s = QString("[Array (%1)]: %2").arg(val.toVariant().value<QVariantList>().size()).arg(QString(QJsonDocument::fromVariant(val.toVariant()).toJson(QJsonDocument::Indented)));
    else if (val.isObject() && !val.isCallable() && !val.isQObject() && !val.isVariant())
        s = QString("[Object (%1)]: %2").arg(val.toVariant().value<QVariantMap>().size()).arg(QString(QJsonDocument::fromVariant(val.toVariant()).toJson(QJsonDocument::Indented)));
    else
        s = val.toString();
    emit cbLog(s);
}

void CBObject::onDrawPanel(const QJSValue& f)
{
    if (f.isCallable())
    {
        m_drawpanelfunction = f;
    }
    else
        emit error("Argument for cb.onDrawPanel() has to be a function.");
}

void CBObject::onEnter(const QJSValue& f)
{
    if (f.isCallable())
        m_enterfunction = f;
    else
        emit error("Argument for cb.onEnter() has to be a function.");
}

void CBObject::onLeave(const QJSValue& f)
{
    if (f.isCallable())
        m_leavefunction = f;
    else
        emit error("Argument for cb.onLeave() has to be a function.");
}

void CBObject::onMessage(const QJSValue& f)
{
    if (f.isCallable())
        m_messagefunction = f;
    else
        emit error("Argument for cb.onMessage() has to be a function.");
}

void CBObject::onTip(const QJSValue& f) {
    if (f.isCallable())
        m_tipfunction = f;
    else
        emit error("Argument for cb.onTip() has to be a function.");
}

void CBObject::sendNotice(const QString& message, const QString& to_user, const QString& background, const QString& foreground, const QString& weight, const QString& to_group)
{
    emit appNotice(message, to_user, background, foreground, weight, to_group);
}

void CBObject::setTimeout(const QJSValue& f, const int& msecs)
{
    if (msecs < 1000)
        emit error("Timeout for setTimeout(func, msecs) should be higher than 1000.");
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &CBObject::onJSTimer);
    m_timedfunctions.insert(++m_hashidx, f);
    t->setProperty("funcidx", m_hashidx);
    t->start(msecs);
}

void CBObject::tipOptions(const QJSValue& f)
{
    if (f.isCallable())
        m_tipoptionsfunction = f;
    else
        emit error("Argument for cb.tipOptions() has to be a function.");
}

QJsonObject CBObject::getTipOptions()
{
    if (!m_tipoptionsfunction.isUndefined() && m_tipoptionsfunction.isCallable())
    {
        QJSValue ret = m_tipoptionsfunction.call();
        if (ret.isError())
            emit error("Error in cb.tipOptions(): " + ret.toString());
        return ret.isUndefined() ? QJsonObject() : QJsonDocument::fromVariant(ret.toVariant()).object();
    }
    return QJsonObject();
}

void CBObject::onJSTimer()
{
    QTimer *t = qobject_cast<QTimer *>(sender());
    t->stop();
    QJSValue f = m_timedfunctions.take(t->property("funcidx").toInt());
    if (f.isCallable())
    {
        QJSValue ret = f.call();
        if (ret.isError())
            emit error("Error in function given to cb.setTimeout(): " + ret.toString());
    }
    else
        emit error("First argument for cb.setTimeOut() has to be a function.");
    t->deleteLater();
}

void CBObject::receiveTip(const int& amount, const QString& message)
{
    if (!m_tipfunction.isUndefined() && m_tipfunction.isCallable())
    {
        User *u = qobject_cast<User *>(sender());
        QJSValue t = m_engine->newObject();
        t.setProperty("amount", amount);
        t.setProperty("message", message);
        t.setProperty("from_user", u->name);
        t.setProperty("from_user_in_fanclub", u->fanclub);
        t.setProperty("from_user_has_tokens", u->has_tokens);
        t.setProperty("from_user_is_mod", u->mod);
        t.setProperty("from_user_tipped_recently", u->tipped >= User::TIPPED_RECENTLY ? true : false);
        t.setProperty("from_user_tipped_alot_recently", u->tipped >= User::TIPPED_ALOT ? true : false);
        t.setProperty("from_user_tipped_tons_recently", u->tipped >= User::TIPPED_TONS ? true : false);
        t.setProperty("from_user_gender", QString::fromLatin1(&u->gender, 1));
        QJSValue ret = m_tipfunction.call(QJSValueList() << t);
        if (ret.isError())
            emit error("Error in cb.onTip: " + ret.toString());
    }
    else
        emit error("cb.onTip() not defined");
}

void CBObject::receiveChat(const QString& msg)
{
    if (!m_messagefunction.isUndefined() && m_messagefunction.isCallable())
    {
        User *u = qobject_cast<User *>(sender());
        QJSValue m = m_engine->newObject();
        m.setProperty("c", u->textcolor);
        m.setProperty("m", msg);
        m.setProperty("user", u->name);
        m.setProperty("f", u->font);
        m.setProperty("in_fanclub", u->fanclub);
        m.setProperty("has_tokens", u->has_tokens);
        m.setProperty("is_mod", u->mod);
        m.setProperty("tipped_recently", u->tipped >= User::TIPPED_RECENTLY ? true : false);
        m.setProperty("tipped_alot_recently", u->tipped >= User::TIPPED_ALOT ? true : false);
        m.setProperty("tipped_tons_rec5ently", u->tipped >= User::TIPPED_TONS ? true : false);
        m.setProperty("gender", QString::fromLatin1(&u->gender, 1));

        QJSValue ret = m_messagefunction.call(QJSValueList() << m);
        if (!ret.isUndefined() && !ret.isError())
        {
            if (!ret.property("X-Spam").toBool())
                emit newMessage(ret.property("m").toString(), u, ret.property("background").isUndefined() ? "#FFFFFF" : ret.property("background").toString());
        }
        else
            emit error("Error in cb.onMessage(): " + ret.toString());
    }
    else
        emit error("cb.onMessage() not defined");
}

void CBObject::onUserEnter()
{
    if (!m_enterfunction.isUndefined() && m_enterfunction.isCallable())
    {
        User *u = qobject_cast<User *>(sender());
        QJSValue e = m_engine->newObject();
        e.setProperty("user", u->name);
        e.setProperty("in_fanclub", u->fanclub);
        e.setProperty("has_tokens", u->has_tokens);
        e.setProperty("is_mod", u->mod);
        e.setProperty("tipped_recently", u->tipped >= User::TIPPED_RECENTLY ? true : false);
        e.setProperty("tipped_alot_recently", u->tipped >= User::TIPPED_ALOT ? true : false);
        e.setProperty("tipped_tons_recently", u->tipped >= User::TIPPED_TONS ? true : false);
        e.setProperty("gender", QString::fromLatin1(&u->gender, 1));

        QJSValue ret = m_enterfunction.call(QJSValueList() << e);
        if (ret.isError())
            emit error("Error in cb.onEnter(): " + ret.toString());
    }
    else
        emit error("cb.onEnter() not defined");
}

void CBObject::onUserLeave()
{
    if (!m_leavefunction.isUndefined() && m_leavefunction.isCallable())
    {
        User *u = qobject_cast<User *>(sender());
        QJSValue l = m_engine->newObject();
        l.setProperty("user", u->name);
        l.setProperty("in_fanclub", u->fanclub);
        l.setProperty("has_tokens", u->has_tokens);
        l.setProperty("is_mod", u->mod);
        l.setProperty("tipped_recently", u->tipped >= User::TIPPED_RECENTLY ? true : false);
        l.setProperty("tipped_alot_recently", u->tipped >= User::TIPPED_ALOT ? true : false);
        l.setProperty("tipped_tons_recently", u->tipped >= User::TIPPED_TONS ? true : false);
        l.setProperty("gender", QString::fromLatin1(&u->gender, 1));
        QJSValue ret = m_leavefunction.call(QJSValueList() << l);
        if (ret.isError())
            emit error("Error in cb.onLeave(): " + ret.toString());
    }
    else
        emit error("cb.onLeave() not defined");
}



