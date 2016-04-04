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
    menu->addAction("Load App", this, SLOT(onLoadApp()))->setShortcut(QKeySequence("Ctrl+L"));
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
    m_resolveimagesaction = settings->addAction("Load Images from \"Image\" subfolder");
    m_resolveimagesaction->setCheckable(true);
    m_resolveimagesaction->setShortcut(QKeySequence("Ctrl+I"));
    connect(m_resolveimagesaction, &QAction::triggered, [&] (bool checked) {m_cbds.getChatModel()->setResolveImages(checked);});
    QAction* clearchoices = settings->addAction("Clear previous choices of app settings");
    clearchoices->setShortcut(QKeySequence("Ctrl+W"));
    connect(clearchoices, &QAction::triggered, [&] {m_previousstartoptions.clear();});
    connect(settings->addAction("Change webcam image"), &QAction::triggered, [&]
    {
        QString orgname = m_quickui.rootContext()->contextProperty("CamImagePath").toString();
        QString fname;
        QDialog d(this);
        QFormLayout lay;
        QPushButton open("Browse", &d);
        connect(&open, &QPushButton::clicked, [&]
        {
            fname = QFileDialog::getOpenFileName(this, "Choose an image", "", "Image files (*.jpg *.jpeg *.png *.gif *.bmp)");
            if (!fname.isEmpty())
                m_quickui.rootContext()->setContextProperty("CamImagePath", QUrl::fromLocalFile(fname));
        });
        lay.addRow(new QLabel("Open a local file:", &d), &open);
        QLineEdit urledit(&d);
        connect(&urledit, &QLineEdit::textChanged, [&] (const QString& text) {fname = text;});
        lay.addRow(new QLabel("Enter a URL:"), &urledit);
        QPushButton reset("Reset to default", &d);
        connect(&reset, &QPushButton::clicked, [&] {fname.clear(); d.accept();});
        lay.addRow(new QLabel("Or just:", &d), &reset);
        QPushButton apply("Apply", &d);
        connect(&apply, &QPushButton::clicked, [&] {d.accept();});
        QPushButton cancel("Cancel", &d);
        connect(&cancel, &QPushButton::clicked, [&] {d.reject();});
        lay.addRow(&apply, &cancel);
        d.setLayout(&lay);
        if (d.exec() == QDialog::Accepted)
        {
            QSettings sets(m_settingsfile, QSettings::IniFormat, this);
            if (!fname.isEmpty())
            {
                m_quickui.rootContext()->setContextProperty("CamImagePath", QFile::exists(fname) ? QUrl::fromLocalFile(fname) : fname);
                sets.setValue("GUI/Camimage", fname);
            }
            else
            {
                m_quickui.rootContext()->setContextProperty("CamImagePath", "cam.jpeg");
                sets.remove("GUI/Camimage");
            }
        }
        else
            m_quickui.rootContext()->setContextProperty("CamImagePath", orgname);
    });
    QMenu* defviewers = settings->addMenu("Default Viewers");
    connect(defviewers->addAction("Make current viewers the default"), &QAction::triggered, this, &GUI::saveViewers);
    connect(defviewers->addAction("Load default viewers"), &QAction::triggered, &m_cbds, &CBDS::loadViewers);
    menuBar()->addMenu(settings);

    QMenu *help = new QMenu("Help", this);
    connect(help->addAction("FAQ"), &QAction::triggered, [] {QDesktopServices::openUrl(QUrl("https://github.com/lluachaturbate/CBADK/wiki/FAQ"));});
    connect(help->addAction("Debugger manual"), &QAction::triggered, [] () {QDesktopServices::openUrl(QUrl("http://doc.qt.io/qt-4.8/qtscriptdebugger-manual.html"));});
    connect(help->addAction("Default viewers"), &QAction::triggered, [&]
    {
        Viewer def("defaultviewer");
        QVariantList a;
        QVariantMap o;
        o.insert("name", def.getName());
        o.insert("gender", def.getGender());
        o.insert("tipped", def.getTipped());
        o.insert("font", def.getFont());
        o.insert("textcolor", def.getTextcolor());
        o.insert("hastokens", def.hasTokens());
        o.insert("mod", def.isModerator());
        o.insert("fanclub", def.isFanclubmember());
        o.insert("roomowner", def.isRoomOwner());
        a.append(o);
        QVariantMap o2;
        o2.insert("name", "llua");
        o2.insert("gender", "c");
        o2.insert("roomowner", true);
        a.append(o2);
        QString t = QString(QJsonDocument::fromVariant(a).toJson(QJsonDocument::Indented)).replace("\n", "<br>").replace(": ", ": &nbsp;&nbsp;&nbsp;&nbsp;").replace(" ", "&nbsp;");
        QMessageBox m(this);
        m.setStandardButtons(QMessageBox::Close);
        m.setTextFormat(Qt::RichText);
        m.setWindowTitle("Default viewers");
        m.setText("<p>You can set default users by either clicking on \"<b>Settings -> Default viewers -> Make current viewers the default\"</b><br>or creating a file named \"Viewers.json\" next to the binary.</p><p>The format of the file is a JSON array <b>(not an object)</b> containing viewer objects.</p><p>Only the \"name\" property is required, every other field is optional and will load the defaults shown in the example \"defaultviewer\" below.<br>Also 1 object has to have property \"roomowner\" set to true.</p><p><b>" + t + "</b></p>");
        m.setIcon(QMessageBox::Information);
        m.exec();
    });
    menuBar()->addMenu(help);

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

        m_cbds.loadViewers();

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


void GUI::saveViewers()
{
    QMessageBox::StandardButton proceed = QMessageBox::Yes;
    QFile f(QApplication::applicationDirPath() + "/Viewers.json");
    if (f.exists(QApplication::applicationDirPath() + "/Viewers.json"))
        proceed = QMessageBox::question(this, "Warning", "This will override \"" + f.fileName() + "\". \n Proceed?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (proceed == QMessageBox::Yes)
        m_cbds.saveViewers();
}

void GUI::addToReloadMenu(const QString &filename)
{
    m_lastapps.removeAll(filename);
    m_lastapps.prepend(filename);
    m_reloadmenu->clear();
    for (auto i = m_lastapps.constBegin(); i != m_lastapps.constEnd(); ++i)
    {
        QAction* a = m_reloadmenu->addAction(QFileInfo((*i)).fileName(), this, SLOT(onLoadApp()));
        a->setProperty("filename", (*i));
        if (i == m_lastapps.constBegin())
            a->setShortcut(QKeySequence("Ctrl+R"));
    }
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
                SettingsDialog s(settings, settings == m_previousstartoptions.value(filename).first ? m_previousstartoptions.value(filename).second : QVariant(), this);
                if (s.exec() == QDialog::Accepted)
                {
                    QVariant sets = s.getSettings();
                    m_previousstartoptions.insert(filename, QPair<QVariant, QVariant>(settings, sets));
                    m_cbds.startApp(filename, sets);
                }
            }
            else
                m_cbds.startApp(filename, settings);
        }
        addToReloadMenu(filename);
        m_lastscriptfolder = QFileInfo(filename).absoluteDir().absolutePath();
    }
}
