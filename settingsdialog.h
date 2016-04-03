#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonDocument>

//! Dialog to get app settings from user.
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SettingsDialog(const QVariant& choices, const QVariant& lastchoices, QWidget* parent = 0) : QDialog(parent)
    {
        QVariantMap lastsettings = lastchoices.toMap();
        m_startbutton = new QPushButton(this);
        m_startbutton->setText("Start App");
        connect(m_startbutton, &QPushButton::clicked, this, &SettingsDialog::accept);

        enum TYPE {INT, STRING, COMBO};
        QFormLayout *l = new QFormLayout(this);
        l->setHorizontalSpacing(25);

        QVariantList a = choices.toList();
        for (int i = 0; i < a.size(); ++i)
        {
            QVariantMap object = a.at(i).toMap();
            QString name = object.value("name").toString();
            bool required = object.value("required", true).toBool();
            QString label = object.value("label", name).toString();
            if (required)
                label += "*";
            QString type = object.value("type").toString();
            TYPE wtype;

            if (type == "int") wtype = INT;
            else if (type == "str") wtype = STRING;
            else if (type == "choice") wtype = COMBO;

            QWidget *w;
            switch (wtype)
            {
                case INT:
                {
                    QLineEdit *e = new QLineEdit(this);
                    e->setValidator(new QIntValidator(object.value("minValue", -999999).toInt(), object.value("maxValue", 9999999).toInt(), e));
                    if (lastsettings.contains(name))
                        e->insert(lastsettings.value(name).toString());
                    else if (object.value("defaultValue").isValid())
                        e->setText(object.value("defaultValue").toString());

                    connect(e, &QLineEdit::textEdited, [=] () {
                        if (QRegExp("0+").exactMatch(e->text()) && !e->hasAcceptableInput())
                            e->setText("");
                    });

                    if (required)
                    {
                        e->setProperty("minLength", 1);
                        reqfields << e;
                        connect(e, &QLineEdit::textEdited, this, &SettingsDialog::checkRequired);
                    }

                    w = e;
                    break;
                }
                case STRING:
                {
                    QLineEdit *e = new QLineEdit(this);
                    if (lastsettings.contains(name))
                        e->setText(lastsettings.value(name).toString());
                    else
                        e->setText(object.value("defaultValue").toString());
                    e->setMaxLength(object.value("maxLength", 32767).toInt());
                    if (required)
                    {
                        e->setProperty("minLength", object.value("minLength", 1).toInt());
                        reqfields << e;
                        connect(e, &QLineEdit::textEdited, this, &SettingsDialog::checkRequired);
                    }
                    w = e;
                    break;
                }
                case COMBO:
                {
                    QComboBox *b = new QComboBox(this);
                    QStringList cl = object.keys().filter("choice");
                    for (auto i = cl.constBegin(); i != cl.constEnd(); ++i)
                        b->addItem(object.value((*i)).toString());
                    if (lastsettings.contains(name) && b->findText(lastsettings.value(name).toString()) != -1)
                        b->setCurrentText(lastsettings.value(name).toString());
                    else if (object.contains("defaultValue"))
                        b->setCurrentText(object.value("defaultValue").toString());
                    w = b;
                    break;
                }
            }
            w->setProperty("settingname", name);
            w->setToolTip(QString(QJsonDocument::fromVariant(object).toJson(QJsonDocument::Indented)));
            l->addRow(label, w);
        }

        QPushButton *reject = new QPushButton(this);
        reject->setText("Cancel");
        connect(reject, &QPushButton::clicked, this, &SettingsDialog::reject);
        l->addRow(reject, m_startbutton);
        checkRequired();
        setLayout(l);
    }


    //! Returns a QVariant(QVariantMap) containing the user input.
    QVariant getSettings() const
    {
        QVariantMap ret;
        QList<QWidget *> wl = findChildren<QWidget *>();
        for (auto i = wl.constBegin(); i != wl.constEnd(); ++i)
        {
            if ((*i)->property("settingname").isValid())
            {
                if (QString((*i)->metaObject()->className()) == "QComboBox")
                    ret.insert((*i)->property("settingname").toString(), qobject_cast<QComboBox *>((*i))->currentText());
                else if (QString((*i)->metaObject()->className()) == "QLineEdit")
                    ret.insert((*i)->property("settingname").toString(), qobject_cast<QLineEdit *>((*i))->text());
            }
        }
        return QVariant(ret);
    }

private:
    QPushButton* m_startbutton;
    QList<QLineEdit *> reqfields;

private slots:
    //! Checks if all required fields are filled / match the requirements.
    void checkRequired()
    {
        bool allset = true;
        m_startbutton->setToolTip("");
        QStringList missing;
        for (auto i = reqfields.constBegin(); i != reqfields.constEnd(); ++i)
        {
            if ((*i)->text().length() < (*i)->property("minLength").toInt() || !(*i)->hasAcceptableInput())
            {
                missing << (*i)->property("settingname").toString();
                allset = false;
            }
        }
        if (allset)
            m_startbutton->setEnabled(true);
        else
        {
            m_startbutton->setToolTip("<font color=\"red\"><b>Missing required fields:</b>\n</font>\n" + missing.join('\n'));
            m_startbutton->setEnabled(false);
        }
    }

};

#endif // SETTINGSDIALOG_H
