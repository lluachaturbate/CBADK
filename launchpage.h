#ifndef LAUNCHPAGE_H
#define LAUNCHPAGE_H

#include <QDialog>
#include <QHBoxLayout>
#include <QQuickWidget>
#include <QQmlContext>


class LaunchPage : public QDialog
{
    Q_OBJECT
public:
    LaunchPage(const QVariant& choices, const QVariant& lastchoices, QWidget* parent = 0) : QDialog(parent)
    {
        QQuickWidget *quickwidget = new QQuickWidget(this);
        QHBoxLayout *hb = new QHBoxLayout(this);
        hb->setMargin(0);
        hb->addWidget(quickwidget);
        setLayout(hb);

        quickwidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        quickwidget->rootContext()->setContextProperty("choices", choices);
        quickwidget->rootContext()->setContextProperty("lastchoices", lastchoices);
        quickwidget->rootContext()->setContextProperty("backend", this);
        quickwidget->setSource(QUrl("qrc:/qml/LoadApp.qml"));

        setMinimumWidth(698);
        resize(698,690);
        show();
    }

    Q_INVOKABLE void setInput(QJSValue input) {m_input = input.toVariant();}
    QVariant getSettings() const {return m_input;}

private:
    QVariant m_input;
};

#endif // LAUNCHPAGE_H
