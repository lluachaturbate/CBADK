#ifndef CBDS_H
#define CBDS_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QAction>

#include "chatmodel.h"
#include "viewermodel.h"
#include "cbobjectimpl.h"
#include "cbjsobject.h"

#include "viewerchatmodel.h"

//! The main object. Holds the models, loads scripts and creates event objects.
class CBDS : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ViewerModel* viewers READ getViewerModel CONSTANT)
    Q_PROPERTY(ViewerChatModel* chat READ getViewerChatModel CONSTANT)
    Q_PROPERTY(CBObjectImpl* cb READ getCBObject CONSTANT)
public:
    explicit CBDS(QObject *parent = 0);

    //! Loads the script specified by filename with an CBObjectBase object. @returns QVariant(QVariantList) of the settings_choices object or an invalid QVariant on error.
    QVariant getSettingsFromScript(const QString& filename);

    //! Resets and sets settings of the CBObject and loads the script specified by filename. @returns false on any error.
    bool startApp(const QString& filename, const QVariant& settings);

    //! Adds a Viewer to the model
    Q_INVOKABLE Viewer *addViewer(const QString& username, const int& tips=0, const bool& in_fanclub=0, const bool& is_mod=0, const bool& has_tokens=0, const char& gender='m');

    //! Decides if the chat is cleared upon loading a new app.
    void setClearChatStart(const bool& clear) {m_clearchatonstart = clear;}

    //! Populates the ViewerModel with Viewers created from the json data.
    void parseViewerData(const QVariantList& data);

    //! @sa CBObjectImpl::callDrawPanelFunction()
    Q_INVOKABLE QVariant getViewerPanel(Viewer* v);

    ViewerModel *getViewerModel() {return &m_viewers;}
    ViewerChatModel *getViewerChatModel() {return &m_viewerchat;}
    ChatModel *getChatModel() {return &m_chat;}
    CBObjectImpl* getCBObject() {return m_cbo;}
    QScriptEngineDebugger* debugger() {return &m_debugger;}
private:
    QScriptEngine m_engine;
    QScriptEngineDebugger m_debugger;
    CBJSObject m_cbjso;
    CBObjectImpl *m_cbo;
    ViewerModel m_viewers;
    ChatModel m_chat;
    ViewerChatModel m_viewerchat;
    bool m_clearchatonstart = false, m_resumeonnextsuspension = false;
    QString m_roomowner = "llua";
    QVariantMap m_lastdraw;

    Viewer* connectViewer(Viewer * v);
    QScriptValue createViewerValue(Viewer *v);

    void setRoomOwner(const QString& slug) {m_roomowner = slug; m_cbo->setRoomSlug(slug);}
    void engageDebugger() {m_debugger.action(QScriptEngineDebugger::InterruptAction)->trigger(); m_resumeonnextsuspension = true;}

signals:
    //! Emitted on any error thats not scriptrelated.
    void error(QString msg);
    //! Emitted when the panel should be updated. object is a map of username and their return object from callDrawPanelFunction().
    void updatePanel(QVariantMap object);

private slots:
    //! Creates a tip object and calls CBObjectImpl::callTipFunction(). @warning needs Viewer* as sender()
    void onTip(const int& amount, const QString& message);
    //! Creates a message object and calls CBObjectImpl::callMessageFunction(). @warning needs Viewer* as sender()
    void onChat(const QString& message);
    //! Creates a enter object and calls CBObjectImpl::callEnterFunction(). @warning needs Viewer* as sender()
    void onEnter();
    //! Creates a leave object and calls CBObjectImpl::callLeaveFunction(). @warning needs Viewer* as sender()
    void onLeave();
    //! Calls CBObjectImpl::callDrawPanelFunction() with a viewer object for every viewer and emits updatePanel() with the results.
    void onDrawPanel();
    //! Adds a notice ChatLine to the model.
    void onAppNotice(const QString &message, const QString &to_user ="", const QString &background = "#FFFFFF", const QString &foreground = "#000000", const QString &weight="normal", const QString &to_group ="");
    //! Adds a logmessage ChatLine to the model.
    void onCbLog(const QString& message);
    //! Adds a roomsubject change ChatLine to the model.
    void onRoomSubjectChanged(const QString &new_subject);
    //! Sets the viewers limitcam access to allowed.
    void onLimitCamAccessChanged(const QString& name, const bool& allowed);

public slots:
    //! If Viewers.json exists loads users from that file. @return true if loaded. @sa saveViewers()
    void loadViewers();

    //! Saves Viewers to Viewers.json.
    void saveViewers();
};

#endif // CBDS_H
