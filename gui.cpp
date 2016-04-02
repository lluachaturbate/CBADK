#include "gui.h"

//Qt seems to have problems with plugin integration if statically compiled.
//I didn't look into the issue but grabbed this somewhere from the web, hope that fixes it.
#ifdef QT_STATIC
#include <QtPlugin>
#include <QQmlExtensionPlugin>

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuickControlsPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
#endif

GUI::GUI(QWidget *parent)
    : QMainWindow(parent)
{
    connect(&m_cbds, &CBDS::error, [&] (QString msg) {QMessageBox::critical(this,"Error", msg, QMessageBox::Close);});

#ifdef QT_STATIC
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->registerTypes("QtQuick");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->registerTypes("QtQuick.Controls.Styles");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->registerTypes("QtQuick.Layouts");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->registerTypes("QtQuick.Window");

    QQmlEngine *e = qobject_cast<QQmlEngine *>(m_quickui.engine());
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2Plugin().instance())->initializeEngine(e, "QtQuick");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Controls");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Private");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickControlsPlugin().instance())->initializeEngine(e, "QtQuick.Styles");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuickLayoutsPlugin().instance())->initializeEngine(e, "QtQuick.Layouts");
    qobject_cast<QQmlExtensionPlugin*>(qt_static_plugin_QtQuick2WindowPlugin().instance())->initializeEngine(e, "QtQuick.Window");
#endif

    m_quickui.engine()->rootContext()->setContextProperty("backend", &m_cbds);
    m_quickui.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_quickui.setMinimumHeight(250);
    setCentralWidget(&m_quickui);
    qmlRegisterType<Viewer>();


    QMenu *menu = new QMenu("App", this);
    menu->addAction("Load App", this, SLOT(onLoadApp()));
    m_reloadmenu = menu->addMenu("Reload App");
    menuBar()->addMenu(menu);

    m_viewmenu = new QMenu("Views", this);
    menuBar()->addMenu(m_viewmenu);

    m_cbds.debugger()->setAutoShowStandardWindow(false);
    QMenu *dmenu = m_cbds.debugger()->createStandardMenu(this);
    dmenu->addSeparator();
    connect(dmenu->addAction("Debugger manual"), &QAction::triggered, [] () {QDesktopServices::openUrl(QUrl("http://doc.qt.io/qt-4.8/qtscriptdebugger-manual.html"));});
    menuBar()->addMenu(dmenu);

    QMenu* settings = new QMenu("Settings", this);
    QAction* sa = settings->addAction("Save GUI settings");
    sa->setCheckable(true);
    sa->setChecked(m_save);
    connect(sa, &QAction::triggered, [&] (bool checked) {m_save = checked;});
    m_clearaction = settings->addAction("Clear chat on app start");
    m_clearaction->setCheckable(true);
    connect(m_clearaction, &QAction::triggered, [&] (bool checked) {m_cbds.setClearChatStart(checked);});
    menuBar()->addMenu(settings);
    m_resolveimagesaction = settings->addAction("Load Images from \"Image\" subfolder");
    m_resolveimagesaction->setCheckable(true);
    connect(m_resolveimagesaction, &QAction::triggered, [&] (bool checked) {m_cbds.getChatModel()->setResolveImages(checked);});

    QMenu* about = new QMenu("About", this);
    connect(about->addAction("About CBADK"), &QAction::triggered, [&]
    {
        QMessageBox m(this);
        m.setStandardButtons(QMessageBox::Close);
        m.setTextFormat(Qt::RichText);
        m.setWindowTitle("About");
        m.setText("<p align='center'>This project exists out of pure neccessarity.</p><p align='center'>I wanted to make my own app and since im terrible with JS i couldn't code without having a useful way of testing.<br>I didn't want to copy the CB site because i wanted some kind of multi-user support to test real use scenarios.</p><p align='center'>If you find any bugs or miss any functionality head over to <a href='https://github.com/lluachaturbate/CBADK'>github</a></p>.");
        m.setIcon(QMessageBox::Information);
        m.exec();
    });
    connect(about->addAction("Donate"), &QAction::triggered, [&]
    {
       QMessageBox m(this);
       m.setStandardButtons(QMessageBox::Close);
       m.setTextFormat(Qt::RichText);
       m.setWindowTitle("Thank you!");
       m.setText("<b>Thank you for clicking your way here.</b>");
       m.setInformativeText("<p align='center'>Since you obviously like CB, why not just head over to <a href='https://chaturbate.com/llua'>my profile</a> and show your appreciation in tokens?</p>");
       m.setIcon(QMessageBox::Information);
       m.exec();
    });
    about->addAction("About Qt", qApp, SLOT(aboutQt()));
    menuBar()->addMenu(about);

    QToolBar* t = m_cbds.debugger()->createStandardToolBar(this);
    t->setObjectName("DbgToolBar");
    t->setWindowTitle("DebugToolBar");
    addToolBar(Qt::RightToolBarArea, t);

    createDockWidgets();
    loadSettings();
    m_quickui.setSource(QUrl("qrc:/qml/main.qml"));
}

GUI::~GUI()
{

}

bool GUI::loadSettings()
{
    m_quickui.rootContext()->setContextProperty("CamImagePath", "cam.jpeg");
    if (QFile::exists(m_settingsfile))
    {
        QSettings sets(m_settingsfile, QSettings::IniFormat, this);
        m_lastscriptfolder = sets.value("GUI/Scriptfolder", "").toString();
        restoreGeometry(sets.value("GUI/Geometry").toByteArray());
        restoreState(sets.value("GUI/State").toByteArray());
        bool clearchatonstart = sets.value("CBDS/ClearChatOnStart", false).toBool();
        m_clearaction->setChecked(clearchatonstart);
        m_cbds.setClearChatStart(clearchatonstart);
        bool resimg = sets.value("CBDS/ResolveImages", false).toBool();
        m_resolveimagesaction->setChecked(resimg);
        m_cbds.getChatModel()->setResolveImages(resimg);

        QString campath = sets.value("GUI/Camimage").toString();
        if (!campath.isEmpty())
            m_quickui.rootContext()->setContextProperty("CamImagePath", QFile::exists(campath) ? QUrl::fromLocalFile(campath) : campath);
        return true;
    }
    resize(1300,600);
    return false;
}

void GUI::saveSettings()
{
    if (m_save)
    {
        QSettings sets(m_settingsfile, QSettings::IniFormat, this);
        sets.setValue("GUI/Scriptfolder", m_lastscriptfolder);
        sets.setValue("GUI/Geometry", saveGeometry());
        sets.setValue("GUI/State", saveState());
        sets.setValue("CBDS/ClearChatOnStart", m_clearaction->isChecked());
        sets.setValue("CBDS/ResolveImages", m_resolveimagesaction->isChecked());
    }
}

void GUI::addToReloadMenu(const QString &filename)
{
    m_lastapps << filename;
    m_reloadmenu->clear();
    for (auto i = m_lastapps.constBegin(); i != m_lastapps.constEnd(); ++i)
        m_reloadmenu->addAction(QFileInfo((*i)).fileName(), this, SLOT(onLoadApp()))->setProperty("filename", (*i));
}

void GUI::createDockWidgets()
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);

    QDockWidget *dstack = new QDockWidget("Stack", this);
    dstack->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::StackWidget));
    dstack->setObjectName("DbgStack");
    addDockWidget(Qt::RightDockWidgetArea, dstack);
    m_viewmenu->addAction(dstack->toggleViewAction());

    QDockWidget *dscripts = new QDockWidget("Scripts", this);
    dscripts->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::ScriptsWidget));
    dscripts->setObjectName("DbgScripts");
    addDockWidget(Qt::RightDockWidgetArea, dscripts);
    m_viewmenu->addAction(dscripts->toggleViewAction());

    QDockWidget *dlocals = new QDockWidget("Locals", this);
    dlocals->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::LocalsWidget));
    dlocals->setObjectName("DbgLocals");
    addDockWidget(Qt::RightDockWidgetArea, dlocals);
    m_viewmenu->addAction(dlocals->toggleViewAction());

    QDockWidget *dbreakpoints = new QDockWidget("Breakpoints", this);
    dbreakpoints->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::BreakpointsWidget));
    dbreakpoints->setObjectName("DbgBreakpoints");
    addDockWidget(Qt::RightDockWidgetArea, dbreakpoints);
    m_viewmenu->addAction(dbreakpoints->toggleViewAction());

    QDockWidget *dcode = new QDockWidget("Code", this);
    dcode->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::CodeWidget));
    dcode->setObjectName("DbgCode");
    addDockWidget(Qt::BottomDockWidgetArea, dcode);
    m_viewmenu->addAction(dcode->toggleViewAction());

    QDockWidget *ddebugoutput = new QDockWidget("Debug Output", this);
    ddebugoutput->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::DebugOutputWidget));
    ddebugoutput->setObjectName("DbgDebugOutput");
    addDockWidget(Qt::BottomDockWidgetArea, ddebugoutput);
    m_viewmenu->addAction(ddebugoutput->toggleViewAction());

    QDockWidget *derrorlog = new QDockWidget("Error Log", this);
    derrorlog->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::ErrorLogWidget));
    derrorlog->setObjectName("DbgErrorLog");
    addDockWidget(Qt::BottomDockWidgetArea, derrorlog);
    m_viewmenu->addAction(derrorlog->toggleViewAction());

    QDockWidget *dconsole = new QDockWidget("Console", this);
    dconsole->setWidget(m_cbds.debugger()->widget(QScriptEngineDebugger::ConsoleWidget));
    dconsole->setObjectName("DbgConsole");
    addDockWidget(Qt::BottomDockWidgetArea, dconsole);
    m_viewmenu->addAction(dconsole->toggleViewAction());

    m_warningdock = new QDockWidget("Warnings", this);
    m_warningdock->setProperty("orgtitle", m_warningdock->windowTitle());
    m_warningdock->setProperty("unread", 0);
    m_warningdock->setWidget(new QTextEdit(m_warningdock));
    m_warningdock->setObjectName("DockWarnings");
    addDockWidget(Qt::BottomDockWidgetArea, m_warningdock);
    m_viewmenu->addAction(m_warningdock->toggleViewAction());
    connect(m_cbds.getCBObject(), &CBObjectImpl::warning, [&] (QString msg) {
        qobject_cast<QTextEdit *>(m_warningdock->widget())->append(msg);
        if (!m_warningdock->property("selected").toBool())
        {
            int count =  m_warningdock->property("unread").toInt() +1;
            m_warningdock->setProperty("unread", count);
            m_warningdock->setWindowTitle(QString("%1 (%2)").arg(m_warningdock->property("orgtitle").toString()).arg(count));
        }
    });
    connect(m_warningdock, &QDockWidget::visibilityChanged, [&] (bool visible) {
        m_warningdock->setProperty("selected", visible);
        m_warningdock->setProperty("unread", 0);
        m_warningdock->setWindowTitle(m_warningdock->property("orgtitle").toString());
    });

    tabifyDockWidget(ddebugoutput, derrorlog);
    tabifyDockWidget(derrorlog, m_warningdock);
    tabifyDockWidget(m_warningdock, dcode);
    tabifyDockWidget(dcode, dconsole);
    tabifyDockWidget(dlocals, dstack);
}

void GUI::onLoadApp()
{
    QString filename;
    QObject *o = qobject_cast<QObject *>(sender());
    if (o && o->property("filename").isValid())
        filename = o->property("filename").toString();
    else
        filename = QFileDialog::getOpenFileName(this, "Load Script", m_lastscriptfolder, "Javascript (*.js);;All (*)");

    if (!filename.isEmpty())
    {
        qobject_cast<QTextEdit *>(m_warningdock->widget())->clear();
        m_warningdock->setProperty("unread", 0);
        m_warningdock->setWindowTitle(m_warningdock->property("orgtitle").toString());
        QVariant settings = m_cbds.getSettingsFromScript(filename);
        if (settings.isValid())
        {
            if (!settings.toList().isEmpty())
            {
                SettingsDialog s(settings, this);
                if (s.exec() == QDialog::Accepted)
                    m_cbds.startApp(filename, s.getSettings());
            }
            else
                m_cbds.startApp(filename, settings);
        }
        addToReloadMenu(filename);
        m_lastscriptfolder = QFileInfo(filename).absoluteDir().absolutePath();
    }
}
