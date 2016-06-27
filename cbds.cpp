#include "cbds.h"

CBDS::CBDS(QObject *parent) : QObject(parent)
{
    //Fix for the difference in number object
    m_engine.evaluate("Number.parseInt = parseInt;Number.parseFloat = parseFloat;Number.isNaN = Number.isNaN || function(value) {return typeof value === \"number\" && isNaN(value);}");

    m_cbo = new CBObjectImpl(&m_engine, this);
    m_viewerchat.setSourceModel(&m_chat);
    m_debugger.attachTo(&m_engine);
    QScriptContext *ctx = m_engine.pushContext();
    ctx->activationObject().setProperty("cb", m_engine.newQObject(m_cbo));
    ctx->activationObject().setProperty("cbjs", m_engine.newQObject(&m_cbjso));

    connect(m_cbo, &CBObjectImpl::cbLog, this, &CBDS::onCbLog);
    connect(m_cbo, &CBObjectImpl::changeRoomSubject, this, &CBDS::onRoomSubjectChanged);
    connect(m_cbo, &CBObjectImpl::sendNotice, this, &CBDS::onAppNotice);
    connect(m_cbo, &CBObjectImpl::limitCamAccessChanged, this, &CBDS::onLimitCamAccessChanged);
    connect(m_cbo, &CBObjectImpl::drawPanelRequest, this, &CBDS::onDrawPanel);
    connect(&m_debugger, &QScriptEngineDebugger::evaluationSuspended, m_cbo, &CBObjectImpl::pauseTimers);
    connect(&m_debugger, &QScriptEngineDebugger::evaluationResumed, m_cbo, &CBObjectImpl::resumeTimers);
}

QVariant CBDS::getSettingsFromScript(const QString &filename)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {
        QString appcode(f.readAll());
        f.close();
        QScriptSyntaxCheckResult res = m_engine.checkSyntax(appcode);
        if (res.state() != QScriptSyntaxCheckResult::Valid)
        {
            emit error(QString("SyntaxCheck Failed: Line: %1 Column: %2: %3").arg(res.errorLineNumber()).arg(res.errorColumnNumber()).arg(res.errorMessage()));
            return QVariant();
        }

        QScriptContext *ctx = m_engine.pushContext();
        CBJSObject js;
        CBObjectBase cbo(&m_engine, m_roomowner);
        ctx->activationObject().setProperty("cb", m_engine.newQObject(&cbo));
        ctx->activationObject().setProperty("cbjs", m_engine.newQObject(&js));
        QScriptValue ret = m_engine.evaluate(appcode, QFileInfo(filename).fileName());
        if (ret.isError())
        {
            emit error(ret.toString());
            m_engine.popContext();
            return QVariant();
        }

        m_engine.popContext();
        return cbo.getSettingsChoices().toVariant();
    }
    emit error("Can't open file: " + filename);
    return QVariant();
}

bool CBDS::startApp(const QString &filename, const QVariant &settings)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {
        QString appcode(f.readAll());
        f.close();
        m_cbo->reset();
        m_lastdraw = QVariantMap();
        m_engine.collectGarbage();
        if (settings.isValid())
            m_cbo->setSettings(m_engine.toScriptValue(settings));
        if (!m_engine.evaluate(appcode, QFileInfo(filename).fileName()).isError())
        {
            if (m_clearchatonstart)
                m_chat.clear();
            m_chat.addLine(new ChatLine(QFileInfo(filename).baseName() + " app has started."));
            m_cbo->drawPanel();
            return true;
        }
        else
            return false;
    }
    emit error("Can't open file: " + filename);
    return false;
}

Viewer *CBDS::addViewer(const QString &username, const int &tips, const bool &in_fanclub, const bool &is_mod, const bool &has_tokens, const char &gender)
{
    Viewer* nv = new Viewer(username, tips, in_fanclub, is_mod, has_tokens, gender, this);
    Viewer* v = m_viewers.addViewer(nv);
    if (v)
        return connectViewer(v);
    emit error("Can't add \"" + username + "\". Name already exists or is reserved.");
    nv->deleteLater();
    return v;
}

void CBDS::parseViewerData(const QVariantList &data)
{
    Viewer def("def");
    QList<Viewer*> list;
    bool ownerset = false;
    for (auto i = data.constBegin(); i != data.constEnd(); ++i)
    {
        QVariantMap o = (*i).toMap();
        Viewer *v = new Viewer(o.value("name").toString(), o.value("tipped", def.getTipped()).toInt(), o.value("fanclub", def.isFanclubmember()).toBool(), o.value("mod", def.isModerator()).toBool(), o.value("hastokens", def.hasTokens()).toBool(), o.value("gender", def.getGender()).toString().at(0).toLatin1());
        if (o.value("roomowner", false).toBool())
        {
            if (!ownerset)
            {
                v->setRoomOwner(true);
                setRoomOwner(v->getName());
                ownerset = true;
            }
            else
                emit error(QString("Can't have more than 1 room owner. Owner: \"%1\" second: \"%2\"").arg(m_roomowner, v->getName()));
        }
        list << v;
    }

    if (!ownerset && !list.isEmpty())
    {
        list.first()->setRoomOwner(true);
        setRoomOwner(list.first()->getName());
        emit error("No Viewer is set as the room owner. Automatically set \"" + list.first()->getName() + "\" as the owner.");
    }

    m_viewers.populate(list);
    for (auto i = list.constBegin(); i != list.constEnd(); ++i)
    {
        if ((*i)->parent())
            connectViewer((*i));
        else
        {
            emit error("Can't add \"" + (*i)->getName() + "\". Name already exists or is reserved.");
            (*i)->deleteLater();
        }
    }
}

QVariant CBDS::getViewerPanel(Viewer *v)
{
    return v ? m_cbo->callDrawPanelFunction(createViewerValue(v)) : QVariant();
}

Viewer *CBDS::connectViewer(Viewer *v)
{
    connect(v, &Viewer::tip, this, &CBDS::onTip);
    connect(v, &Viewer::chat, this, &CBDS::onChat);
    connect(v, &Viewer::enter, this, &CBDS::onEnter);
    connect(v, &Viewer::leave, this, &CBDS::onLeave);
    connect(v, &Viewer::dataChanged, &m_chat, &ChatModel::viewerDataChanged);
    return v;
}

QScriptValue CBDS::createViewerValue(Viewer *v)
{
    QScriptValue e = m_engine.newObject();
    QScriptValue::PropertyFlags flag = QScriptValue::ReadOnly | QScriptValue::Undeletable;
    e.setProperty("user", v->getName(), flag);
    e.setProperty("in_fanclub", v->isFanclubmember(), flag);
    e.setProperty("has_tokens", v->hasTokens(), flag);
    e.setProperty("is_mod", v->isModerator(), flag);
    e.setProperty("tipped_recently", v->getTipped() >= Viewer::TIPPED_RECENTLY ? true : false, flag);
    e.setProperty("tipped_alot_recently", v->getTipped() >= Viewer::TIPPED_ALOT? true : false, flag);
    e.setProperty("tipped_tons_recently", v->getTipped() >= Viewer::TIPPED_TONS ? true : false, flag);
    e.setProperty("gender", v->getGender(), flag);
    return e;
}

void CBDS::onTip(const int &amount, const QString &message)
{
    Viewer *v = qobject_cast<Viewer *>(sender());
    if (v)
    {
        m_chat.addLine(new ChatLine(message, v, "#000000", "#FFFF33", amount));
        QScriptValue e = m_engine.newObject();
        QScriptValue::PropertyFlags flag = QScriptValue::ReadOnly | QScriptValue::Undeletable;
        e.setProperty("amount", amount, flag);
        e.setProperty("message", message, flag);
        e.setProperty("from_user", v->getName(), flag);
        e.setProperty("from_user_in_fanclub", v->isFanclubmember(), flag);
        e.setProperty("from_user_has_tokens", v->hasTokens(), flag);
        e.setProperty("from_user_is_mod", v->isModerator(), flag);
        e.setProperty("from_user_tipped_recently", v->getTipped() >= Viewer::TIPPED_RECENTLY ? true : false, flag);
        e.setProperty("from_user_tipped_alot_recently", v->getTipped() >= Viewer::TIPPED_ALOT ? true : false, flag);
        e.setProperty("from_user_tipped_tons_recently", v->getTipped() >= Viewer::TIPPED_TONS ? true : false, flag);
        e.setProperty("from_user_gender", v->getGender(), flag);
        m_cbo->callTipFunction(e);
    }
}

void CBDS::onChat(const QString &message)
{
    Viewer *v = qobject_cast<Viewer *>(sender());
    if (v)
    {
        if (message == "/debug")
        {
            v->setDebugReadable(!v->getDebugReadable());
            m_chat.addLine(new ChatLine(v->getDebugReadable() ? "Debug mode enabled. Type /debug again to disable." : "Debug mode disabled.", Q_NULLPTR, "#000000", "#FFFFFF", 0, v->getName()));
            return;
        }
        QScriptValue e = createViewerValue(v);
        e.setProperty("c", v->getTextcolor());
        e.setProperty("m", message);
        e.setProperty("f", v->getFont());
        if (m_cbo->callMessageFunction(e) && !e.property("X-Spam").toBool())
        {
            //There is an example for changing the background in the API docs. I'd prefer proper documentation.
            QString background = "#FFFFFF";
            if (e.property("background").isValid())
            {
                background = e.property("background").toString().toUpper();
                if (!QRegExp("#([0-9A-F|]{3}|[0-9A-F]{6})").exactMatch(background))
                {
                    emit m_cbo->warning("CSS message background hacks aren't supported since there is no browser support guarantee. If you feel i should add support let me know. IMHO CB should make stuff like this a viable option here and for notices and ensure browser support.");
                    background = "#FFFFFF";
                }
            }
            m_chat.addLine(new ChatLine(e.property("m").toString(), v, e.property("c").toString(), background));
        }
    }
}

void CBDS::onEnter()
{
    Viewer *v = qobject_cast<Viewer *>(sender());
    if (v)
        m_cbo->callEnterFunction(createViewerValue(v));
}

void CBDS::onLeave()
{
    Viewer *v = qobject_cast<Viewer *>(sender());
    if (v)
        m_cbo->callLeaveFunction(createViewerValue(v));
}

void CBDS::onDrawPanel()
{
    QVariantMap m;
    QList<Viewer *> viewers = m_viewers.viewers();
    for (auto i = viewers.constBegin(); i != viewers.constEnd(); i++)
        m.insert((*i)->getName(), getViewerPanel((*i)));

    if (m_lastdraw == m)
        m_cbo->warning("cb.drawPanel(): Called without any data changed. You just sent a completely useless command to 150000 viewers. And no, please don't ask me why CB isn't filtering it (last time i checked).");
    else
    {
        m_lastdraw = m;
        emit updatePanel(m);
    }
}

void CBDS::onAppNotice(const QString &message, const QString &to_user, const QString &background, const QString &foreground, const QString &weight, const QString &to_group)
{
    QStringList l = message.split("\n");
    for (auto i = l.constBegin(); i != l.constEnd(); ++i)
        m_chat.addLine(new ChatLine((*i), Q_NULLPTR, foreground.isEmpty() ? "#000000" : foreground, background.isEmpty() ? "#FFFFFF" : background, 0, to_group.isEmpty() ? to_user : to_group, weight.isEmpty() ? "normal" : weight ));
}

void CBDS::onCbLog(const QString &message)
{
    m_chat.addLine(new ChatLine(message, Q_NULLPTR, "#FF0000", "#FFFFFF", 0, "cblog"));
}

void CBDS::onRoomSubjectChanged(const QString& new_subject)
{
    m_chat.addLine(new ChatLine(new_subject, Q_NULLPTR, "#f47321", "#FFFFFF", 0, "roomsubject", "bold"));
}

void CBDS::onLimitCamAccessChanged(const QString &name, const bool &allowed)
{
    Viewer* v = m_viewers.getViewerByName(name);
    if (v)
        v->setLimitCamAccess(allowed);
}

void CBDS::loadViewers()
{
    QFile f(QApplication::applicationDirPath() + "/Viewers.json");
    if (f.exists())
    {
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QJsonParseError pe;
            QVariantList json = QJsonDocument::fromJson(f.readAll(), &pe).toVariant().toList();
            f.close();
            if (pe.error == QJsonParseError::NoError)
                parseViewerData(json);
            else
                emit error("Failed to parse Viewers.json: " + pe.errorString());
        }
        else
            emit error("Can't open file: " + f.fileName());
    }

    if (!m_viewers.rowCount(QModelIndex()))
    {
        Viewer* l = addViewer("llua", 0, 0, 0, 1, 'c');
        l->setRoomOwner(true);
        setRoomOwner("llua");
    }
}

void CBDS::saveViewers()
{
    QFile f(QApplication::applicationDirPath() + "/Viewers.json");
    if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    {
        f.write(QJsonDocument::fromVariant(m_viewers.serializeViewers()).toJson(QJsonDocument::Indented));
        f.close();
    }
    else
        emit error("Can't open file: " + f.fileName());
}
