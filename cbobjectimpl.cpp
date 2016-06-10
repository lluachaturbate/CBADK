#include "cbobjectimpl.h"

CBObjectImpl::CBObjectImpl(QScriptEngine *e, QObject *parent) : CBObjectBase(e, parent)
{
    m_engine = e;
}

void CBObjectImpl::drawPanel()
{
    if (m_drawpanelfunction.isFunction())
        emit drawPanelRequest();
}

void CBObjectImpl::limitCam_start(const QString &message, const QScriptValue &allowed_users)
{
    m_limitcam_active = true;
    if (allowed_users.isValid() && allowed_users.isArray())
        limitCam_addUsers(allowed_users);
    emit limitCamStatusChanged(true, message);
}

void CBObjectImpl::limitCam_stop()
{
    m_limitcam_active = false;
    emit limitCamStatusChanged(false);
}

void CBObjectImpl::limitCam_addUsers(const QScriptValue &allowed_users)
{
    if (allowed_users.isArray())
    {
        QStringList list(allowed_users.toString());
        for (auto i = list.constBegin(); i != list.constEnd(); ++i)
        {
            if (!m_limitcam_allowed.contains((*i)))
            {
                m_limitcam_allowed << (*i);
                emit limitCamAccessChanged((*i), true);
            }
            else
                emit warning("cb.limitCam_addUsers(): Trying to add user \"" + (*i) + "\" twice.");
        }
    }
    else
        context()->throwError(QScriptContext::TypeError, "cb.limitCam_addUsers(): Argument needs to be an array.");
}

void CBObjectImpl::limitCam_removeUsers(const QScriptValue &removed_users)
{
    if (removed_users.isArray())
    {
        QStringList l(removed_users.toString());
        for (auto i = l.constBegin(); i != l.constEnd(); ++i)
        {
            if (m_limitcam_allowed.remove((*i)))
                emit limitCamAccessChanged((*i), false);
            else
                emit warning("cb.limitCam_removeUsers(): Trying to remove viewer \"" + (*i) + "\" which wasn't allowed");
        }
    }
    else
        context()->throwError(QScriptContext::TypeError, "cb.limitCam_removeUsers(): Argument needs to be an array.");
}

void CBObjectImpl::limitCam_removeAllUsers()
{
    for (auto i = m_limitcam_allowed.constBegin(); i != m_limitcam_allowed.constEnd(); ++i)
        emit limitCamAccessChanged((*i), false);
    m_limitcam_allowed.clear();
}

QScriptValue CBObjectImpl::limitCam_allUsersWithAccess() const
{
    QVariantList a;
    for (auto i = m_limitcam_allowed.constBegin(); i != m_limitcam_allowed.constEnd(); ++i)
        a << (*i);
    return m_engine->toScriptValue(a);
}

void CBObjectImpl::log(QScriptValue val)
{
    QString s;
    if (val.isArray())
        s = QString("[Array (%1)]: %2").arg(val.toVariant().toList().size()).arg(QString(QJsonDocument::fromVariant(val.toVariant()).toJson(QJsonDocument::Compact)));
    else if (val.isObject() && !val.isFunction() && !val.isQObject())
        s = QString("[Object (%1)]: %2").arg(val.toVariant().toMap().size()).arg(QString(QJsonDocument::fromVariant(val.toVariant()).toJson(QJsonDocument::Compact)));
    else
        s = val.toString();
    emit cbLog(s);
}

void CBObjectImpl::onDrawPanel(const QScriptValue &f)
{
    if (f.isFunction())
        m_drawpanelfunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.onDrawPanel(): Argument needs to be a function.");
}

void CBObjectImpl::onEnter(const QScriptValue &f)
{
    if (f.isFunction())
        m_enterfunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.onEnter(): Argument needs to be a function.");
}

void CBObjectImpl::onLeave(const QScriptValue &f)
{
    if (f.isFunction())
        m_leavefunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.onLeave(): Argument needs to be a function.");
}

void CBObjectImpl::onMessage(const QScriptValue &f)
{
    if (f.isFunction())
        m_messagefunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.onMessage(): Argument needs to be a function.");
}

void CBObjectImpl::onTip(const QScriptValue &f)
{
    if (f.isFunction())
        m_tipfunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.onTip(): Argument needs to be a function.");
}

void CBObjectImpl::setTimeout(const QScriptValue &f, const int &msecs)
{
    if (msecs < 1000)
        emit warning("cb.setTimeout(): msecs < 1000 results in undefined behavior.");
    if (f.isFunction())
    {
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        m_timedfunctions.insert(t, f);
        connect(t, &QTimer::timeout, this, &CBObjectImpl::onTimer);
        t->start(qMax(1000, msecs));
    }
    else
        context()->throwError(QScriptContext::TypeError, "cb.setTimeout(): First argument needs to be a function.");
}

void CBObjectImpl::tipOptions(const QScriptValue &f)
{
    if (f.isFunction())
        m_tipoptionsfunction = f;
    else
        context()->throwError(QScriptContext::TypeError, "cb.tipOptions(): Argument needs to be a function.");
}

bool CBObjectImpl::callEnterFunction(QScriptValue event)
{
    if (m_enterfunction.isFunction())
        return !m_enterfunction.call(QScriptValue(), QScriptValueList() << event).isError();
    emit warning("cb.onEnter(): No function to call.");
    return true;
}

bool CBObjectImpl::callLeaveFunction(QScriptValue event)
{
    if (m_leavefunction.isFunction())
        return !m_leavefunction.call(QScriptValue(), QScriptValueList() << event).isError();
    emit warning("cb.onLeave(): No function to call.");
    return true;
}

bool CBObjectImpl::callMessageFunction(QScriptValue msg)
{
    if (m_messagefunction.isFunction())
    {
        QScriptValue ret = m_messagefunction.call(QScriptValue(), QScriptValueList() << msg);
        if (ret.isError())
            return false;
        if (ret.isUndefined())
        {
            //TODO better throw an exception here
            emit warning("cb.onMessage(): You have to return the message object.");
        }
    }
    else
        emit warning("cb.onMessage(): No function to call.");
    return true;
}

bool CBObjectImpl::callTipFunction(QScriptValue tip)
{
    if (m_tipfunction.isFunction())
        return !m_tipfunction.call(QScriptValue(), QScriptValueList() << tip).isError();
    emit warning("cb.onTip(): No function to call.");
    return true;
}

QVariant CBObjectImpl::callDrawPanelFunction(QScriptValue user)
{
    if (m_drawpanelfunction.isFunction())
    {
        QScriptValue ret = m_drawpanelfunction.call(QScriptValue(), QScriptValueList() << user);
        if (!ret.isError())
            return ret.toVariant();
    }
    return QVariant();
}

QVariant CBObjectImpl::getTipOptions()
{
    if (m_tipoptionsfunction.isFunction())
    {
        QScriptValue ret = m_tipoptionsfunction.call();
        if (!ret.isError())
            return ret.toVariant();
    }
    return QVariant();
}

void CBObjectImpl::reset()
{
    QList<QTimer *> timers = m_timedfunctions.keys();
    for (auto i = timers.begin(); i != timers.end(); ++i)
    {
        (*i)->disconnect();
        (*i)->deleteLater();
    }
    limitCam_stop();
    limitCam_removeAllUsers();
    m_timedfunctions.clear();
    m_tipfunction = m_messagefunction = m_enterfunction = m_leavefunction = m_drawpanelfunction = m_tipoptionsfunction = QScriptValue(QScriptValue::UndefinedValue);
    m_settings = m_engine->newObject();
    m_settings_choices = m_engine->newArray();
}

void CBObjectImpl::onTimer()
{
    QTimer *t = qobject_cast<QTimer *>(sender());
    QScriptValue f = m_timedfunctions.take(t);
    t->deleteLater();
    f.call();
}

void CBObjectImpl::pauseTimers()
{
    QList<QTimer *> l = m_timedfunctions.keys();
    for (auto i = l.constBegin(); i != l.constEnd(); ++i)
    {
        int rem = (*i)->remainingTime();
        (*i)->stop();
        (*i)->setInterval(qMax(rem, 0));
    }
}

void CBObjectImpl::resumeTimers()
{
    QList<QTimer *> l = m_timedfunctions.keys();
    for (auto i = l.constBegin(); i != l.constEnd(); ++i)
        (*i)->start();
}


