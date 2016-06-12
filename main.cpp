#include "gui.h"
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser p;
    QCommandLineOption load(QStringList() << "l" << "load", "Script to load at startup.", "scriptfile");
    p.addOption(load);
    p.addHelpOption();
    p.process(a);

    GUI w;
    w.show();

    if (p.isSet(load))
    {
        QString filename = p.value(load);
        if (QFile::exists(filename))
        {
            QAction ac(&a);
            ac.setProperty("filename", filename);
            QObject::connect(&ac, &QAction::triggered, &w, &GUI::onLoadApp);
            ac.trigger();
        }
    }

    return a.exec();
}
