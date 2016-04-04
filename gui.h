#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QMenu>
#include <QFileDialog>
#include <QApplication>
#include <QMenuBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "cbds.h"
#include "settingsdialog.h"

//! The applications main window.
class GUI : public QMainWindow
{
    Q_OBJECT

public:
    GUI(QWidget *parent = 0);
    ~GUI();

protected:
    //! Reimplemented to call saveSettings().
    void closeEvent(QCloseEvent *e) {saveSettings(); QMainWindow::closeEvent(e);}

private:
    CBDS m_cbds;
    QQuickWidget m_quickui;
    QString m_lastscriptfolder, m_settingsfile = QApplication::applicationDirPath() + "/CBADK.ini";
    QMenu* m_reloadmenu, *m_viewmenu;
    QAction* m_clearaction;
    QAction* m_resolveimagesaction;
    QDockWidget* m_warningdock;
    QStringList m_lastapps;
    QHash<QString, QPair<QVariant, QVariant> > m_previousstartoptions;
    bool m_save = true;




    //! If settingsfile exists loads geometry, state and last script folder. @return true if settings loaded. @sa saveSettings().
    bool loadSettings();

    //! Saves geometry, state and last script folder to settingsfile.
    void saveSettings();

    //! Adds a QAction with a "filename" property to the reloadmenu and connects it to onLoadMenu().
    void addToReloadMenu(const QString& filename);

    //! Creates the dockwidgets mostly from QScriptEngineDebugger.
    void createDockWidgets();

private slots:
    //! If sender() has a filename property loads it, otherwise opens a filedialog.
    void onLoadApp();
    //! Saves Viewers to Viewers.json.
    void saveViewers();
};

#endif // GUI_H
