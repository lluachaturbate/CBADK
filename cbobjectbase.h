#ifndef CBOBJECTBASE_H
#define CBOBJECTBASE_H

#include <QObject>
#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptable>


//! The base class for cb object in the scriptengine. Basically an empty cb object. @note The documentation in here is from CB, so changes in the CB API can be tracked. @sa CBObjectImpl
class CBObjectBase : public QObject, public QScriptable
{
    Q_OBJECT
    //! A variable that contains the name of the current room. This can be used to determine if a message is being sent by the broadcaster.
    Q_PROPERTY(QString room_slug READ getRoomSlug NOTIFY room_slugChanged)

    //! Set this variable in order to have a form filled out by the broadcaster before the app is launched.\n \code{.js} cb.settings_choices = [{name:'tokens_per_minute_to_be_king', type:'int',minValue:1, maxValue:99, defaultValue:5, label: "Tokens per Minute"},{name:'remove_king_when', type:'choice',choice1:'someone else outbids',choice2:'score decays to 0', defaultValue:'someone else outbids'}];\endcode \n For each name in cb.settings_choices, there will be a value loaded in cb.settings. For the example above, there will be a cb.settings.remove_king_when variable.\n Field Types\li int\li str\li choice\n You may add as many choices as needed. The next choice would be choice3, followed by choice4, etc. \n Optional fields\li All fields accept a required: false parameter which makes them become optional.\li All fields accept a label: "Some String" field. This will be the display name for the field as shown in the final rendered form.\li All fields accept a defaultValue: parameter.
    Q_PROPERTY(QScriptValue settings_choices READ getSettingsChoices WRITE setSettingsChoices)

    //! @sa settings_choices
    Q_PROPERTY(QScriptValue settings READ getSettings)
public:
    //! cTor. @note Needs the QScriptEngine* to intialize variables.
    explicit CBObjectBase(QScriptEngine *e, QObject *parent = 0) : QObject(parent)
    {
        m_settings = e->newObject();
        m_settings_choices = e->newArray();
    }
    CBObjectBase(QScriptEngine *e, const QString& room_slug, QObject *parent = 0) : CBObjectBase(e, parent) {setRoomSlug(room_slug);}

    //! This function is only available to apps, not bots. Requests that all users reload the panel (the HTML info area below the cam). The contents of the panel are controlled by cb.onDrawPanel(func).
    Q_INVOKABLE void drawPanel() {}

    //! Hides the cam feed from viewers and shows them a custom message. You can optionally pass in an array of usernames of whom you’d like to be able to view the cam.
    Q_INVOKABLE void limitCam_start(const QString &message, const QScriptValue &allowed_users = QScriptValue()) {Q_UNUSED(message) Q_UNUSED(allowed_users)}

    //! Stops the camera from being hidden from viewers, returning the broadcaster to public broadcasting.
    Q_INVOKABLE void limitCam_stop() {}

    //! Add an array of usernames to allow viewing of the cam while it is hidden to others. You can use this before, during, or after you start/stop limiting the cam.
    Q_INVOKABLE void limitCam_addUsers(const QScriptValue &allowed_users) {Q_UNUSED(allowed_users)}

    //! Remove an array of usernames to no longer be able to view the cam.
    Q_INVOKABLE void limitCam_removeUsers(const QScriptValue &removed_users) {Q_UNUSED(removed_users)}

    //! Remove all viewers from being able to view the cam.
    Q_INVOKABLE void limitCam_removeAllUsers() {}

    //! Check if a particular username is in the list of those allowed to view the cam.
    Q_INVOKABLE bool limitCam_userHasAccess(const QString& user) const {Q_UNUSED(user) return false;}

    //! Get an array of the usernames that are allowed to view the cam.
    Q_INVOKABLE QScriptValue limitCam_allUsersWithAccess() const {return QScriptValue();}

    //! Check if the cam is viewable by those not in the allowed list.
    Q_INVOKABLE bool limitCam_isRunning() const {return false;}

    //! Adds a debug message to the chat. These log messages are broadcast to the chat room, but you must enable debug mode to see them.\n To enable or disable debug mode, type /debug into chat.
    Q_INVOKABLE void log(QScriptValue val) {Q_UNUSED(val)}

    //! This function is only available to apps, not bots. Return data needed to display the info panel for a user. The func argument should be a function that receives 1 argument itself, user.\n These fields are available:\li user: user who entered the room\li in_fanclub: is the user in the broadcaster’s fan club\li has_tokens: does the user have at least 1 token\li is_mod: is the user a moderator\li tipped_recently: is the user a “dark blue”?\li tipped_alot_recently: is the user a “purple”?\li tipped_tons_recently: is the user a “dark purple”?\li gender: “m” (male), “f” (female), “s” (trans), or “c” (couple)\n The return value is a key-value set with a template key. Depending on the template chosen, additional keys should be passed in. For more information, see Available Templates\n \li 3_rows_of_labels\li 3_rows_11_21_31\li 3_rows_12_21_31\li 3_rows_12_22_31
    Q_INVOKABLE void onDrawPanel(const QScriptValue& f) {Q_UNUSED(f)}

    //! Receive a notification when a registered member enters the room. The func argument should be a function that receives 1 argument itself, user.\n These fields are available:\li user: user who entered the room\li in_fanclub: is the user in the broadcaster’s fan club\li has_tokens: does the user have at least 1 token\li is_mod: is the user a moderator\li tipped_recently: is the user a “dark blue”?\li tipped_alot_recently: is the user a “purple”?\li tipped_tons_recently: is the user a “dark purple”?\li gender: “m” (male), “f” (female), “s” (trans), or “c” (couple)
    Q_INVOKABLE void onEnter(const QScriptValue& f) {Q_UNUSED(f)}

    //! Receive a notification when a registered member leaves the room. The func argument should be a function that receives 1 argument itself, user.\n These fields are available:\li user: user who left the room\li in_fanclub: is the user in the broadcaster’s fan club\li has_tokens: does the user have at least 1 token\li is_mod: is the user a moderator\li tipped_recently: is the user a “dark blue”?\li tipped_alot_recently: is the user a “purple”?\li tipped_tons_recently: is the user a “dark purple”?\li gender: “m” (male), “f” (female), “s” (trans), or “c” (couple)
    Q_INVOKABLE void onLeave(const QScriptValue& f) {Q_UNUSED(f)}

    //! Receive a notification when a message is sent. The func argument should be a function that receives 1 argument itself, message.\n Your app can manipulate the message.\n You must return the original message object.\n The message variable passed to the function has these fields:\li c: message color\li m: the message text\li user: username of message sender\li f: message font\li in_fanclub: is the user in the broadcasters fan club\li has_tokens: does the user have at least 1 token\li is_mod: is the user a moderator\li tipped_recently: is the user a “dark blue”?\li tipped_alot_recently: is the user a “purple”?\li tipped_tons_recently: is the user a “dark purple”?\li gender: “m” (male), “f” (female), “s” (trans), or “c” (couple)
    Q_INVOKABLE void onMessage(const QScriptValue& f) {Q_UNUSED(f)}

    //! Receive a notification when a tip is sent. The func argument should be a function that receives 1 argument itself, tip.\n These fields are available:\li amount: amount of tip\li message: message in tip\li to_user: user who received tip\li from_user: user who sent tip\li from_user_in_fanclub: is the user in the broadcasters fan club\li from_user_has_tokens: does the user have at least 1 token\li from_user_is_mod: is the user a moderator\li from_user_tipped_recently: is the user a “dark blue”?\li from_user_tipped_alot_recently: is the user a “purple”?\li from_user_tipped_tons_recently: is the user a “dark purple”?\li from_user_gender: “m” (male), “f” (female), “s” (trans), or “c” (couple)
    Q_INVOKABLE void onTip(const QScriptValue& f) {Q_UNUSED(f)}

    //! This example will print “hello world” in the chat every 10 seconds.\n \code{.js} function callme() { cb.chatNotice("hello world"); cb.setTimeout(callme, 10000) } cb.setTimeout(callme, 10000) \endcode
    Q_INVOKABLE void setTimeout(const QScriptValue& f, const int& msecs) {Q_UNUSED(f) Q_UNUSED(msecs)}

    //! This function is only available to apps, not bots.\n When users send a tip, present them with a list of messages to send with their tip. These messages can be received and processed later by cb.onTip(func).
    Q_INVOKABLE void tipOptions(const QScriptValue& f) {Q_UNUSED(f)}


    QString getRoomSlug() const {return m_room_slug;}
    void setRoomSlug(const QString& name) {if (name != m_room_slug) {m_room_slug = name; emit room_slugChanged(name);}}

    QScriptValue getSettings() const {return m_settings;}
    void setSettings(const QScriptValue& settings) {m_settings = settings;}

    QScriptValue getSettingsChoices() const {return m_settings_choices;}

    //! Checks that settings_choices are conform with the CB API.
    void setSettingsChoices(const QScriptValue &settings_choices)
    {
        if (context())
        {
            if (settings_choices.isArray())
            {
                QStringList names;
                QVariantList a = settings_choices.toVariant().toList();
                for (auto i = a.constBegin(); i != a.constEnd(); ++i)
                {
                    QVariantMap o = (*i).toMap();
                    QString name = o.value("name", "").toString();
                    if (name.isEmpty())
                    {
                        context()->throwError(QScriptContext::TypeError, "Objects in cb.settings_choices need a name property");
                        return;
                    }
                    if (names.contains(name, Qt::CaseInsensitive))
                    {
                        context()->throwError(QScriptContext::ReferenceError, "property \"name\" has to be unique. \"" + name + "\" is a duplicate.");
                        return;
                    }
                    names << name;
                    QString type = o.value("type", "").toString();
                    QStringList allowed = (QStringList() << "int" << "str" << "choice");
                    switch (allowed.indexOf(type))
                    {
                        case 0: //int
                        {
                            bool ok = false;
                            int min, max;
                            bool hasmin = o.value("minValue").isValid();
                            if (hasmin)
                            {
                                min = o.value("minValue").toInt(&ok);
                                if (!ok)
                                {
                                    context()->throwError(QScriptContext::TypeError, "minValue has to be a number.");
                                    return;
                                }
                            }

                            bool hasmax = o.value("maxValue").isValid();
                            if (hasmax)
                            {
                                max = o.value("maxValue").toInt(&ok);
                                if (!ok)
                                {
                                    context()->throwError(QScriptContext::TypeError, "maxValue has to be a number.");
                                    return;
                                }
                            }

                            if (o.value("defaultValue").isValid())
                            {
                                int defaultv = o.value("defaultValue").toInt(&ok);
                                if (!ok)
                                {
                                    context()->throwError(QScriptContext::TypeError, "defaultValue has to be a number.");
                                    return;
                                }
                                if (hasmin && min > defaultv)
                                {
                                    //context()->throwError(QScriptContext::RangeError, "defaultValue has to be higher than minValue");
                                    //return;
                                    emit warning(QString("%1: defaultValue (%2) is lower than minValue (%3). CB allows that, but when they decide to fix their bugs you are screwed.").arg(name).arg(defaultv).arg(min));
                                }
                                if (hasmax && max < defaultv)
                                {
                                    //context()->throwError(QScriptContext::RangeError, "defaultValue has to be lower than maxValue.");
                                    //return;
                                    emit warning(QString("%1: defaultValue (%2) is higher than maxValue (%3). CB allows that, but when they decide to fix their bugs you are screwed.").arg(name).arg(defaultv).arg(max));
                                }
                            }
                            break;
                        }
                        case 1: //str
                        {
                            if (!o.value("minLength", 0).canConvert(QMetaType::Int) || !o.value("maxLength", 0).canConvert(QMetaType::Int) || !o.value("defaultValue", "").canConvert(QMetaType::QString))
                            {
                                context()->throwError(QScriptContext::TypeError, "type \"str\": minLength and maxLength have to be numbers, defaultValue has to be a string.");
                                return;
                            }
                            if (o.value("defaultValue").isValid())
                            {
                                QString defval = o.value("defaultValue").toString();
                                int deflength = defval.length();
                                int minL = o.value("minLength", 0).toInt();
                                int maxL = o.value("maxLength", 32767).toInt();
                                if (deflength < minL)
                                {
                                    //context()->throwError(QScriptContext::RangeError, "defaultValue \"" + defval + "\" has to be longer than minLength.");
                                    //return;
                                    emit warning(QString("%1: defaultValue \"%2\" is shorter than minLength (%3). CB allows that, but when they decide to fix their bugs you are screwed.").arg(name).arg(defval).arg(minL));
                                }
                                if (deflength > maxL)
                                {

                                    //context()->throwError(QScriptContext::RangeError, "defaultValue \"" + defval + "\" has to be shorter than maxLength.");
                                    //return;
                                    emit warning(QString("%1: defaultValue \"%2\" is longer than maxLength (%3). CB allows that, but when they decide to fix their bugs you are screwed.").arg(name).arg(defval).arg(maxL));
                                }
                            }
                            break;
                        }
                        case 2: //choice
                        {
                            if (o.value("defaultValue").isValid())
                            {
                                bool ok = false;
                                QStringList choices = o.keys().filter(QRegExp("choice\\d+"));
                                for (auto i = choices.constBegin(); i != choices.constEnd(); ++i)
                                {
                                    if (o.value((*i)) == o.value("defaultValue"))
                                    {
                                        ok = true;
                                        break;
                                    }
                                }
                                if (!ok)
                                {
                                    //context()->throwError(QScriptContext::ReferenceError, "defaultValue \"" + o.value("defaultValue").toString() + "\" does not match any of the choices");
                                    //return;
                                    emit warning(name +  ": defaultValue \"" + o.value("defaultValue").toString() + "\" does not match any of the choices. CB allows that, but when they decide to fix their bugs you are screwed.");
                                }
                            }
                            break;
                        }
                        default: //not allowed type
                        {
                            context()->throwError(QScriptContext::TypeError, "type property has to be \"int\", \"str\" or \"choice\"");
                            return;
                        }
                    }
                }
            }
            else
            {
                context()->throwError(QScriptContext::TypeError, "settings_choices: Expected array.");
                return;
            }
        }
        m_settings_choices = settings_choices;
    }



protected:
    QString m_room_slug = "llua";
    QScriptValue m_settings_choices, m_settings;

signals:
    //! Emitted when the room_slug changes
    void room_slugChanged(QString room_slug);

    //! Emitted on all warnings. @note Those are mostly bad coding style warnings.
    void warning(QString msg);

    //! This function is only available to apps, not bots. Changes the room subject.
    void changeRoomSubject(QString new_subject);

    //! Send a message to the room. If to_user is given, the message will only be seen by that user. You can also use the optional params background, foreground, and weight to style your message. Only HTML color codes (such as #FF0000) may be given for the color stying, and the font weight will only accept the options normal, bold, or bolder. If you want to provide styling options, but not a to_user, just pass an empty string to to_user.\n You can use a \\n inside the message to send a multi-line notice.\n You may use :emoticons in the notice.\n You can send a message to a certain category of users with the optional to_group param. Just provide the color of which class of users you would like the message to be sent to. Valid choices are red (moderators), green (fan club members), darkblue (users who have tipped 50 recently), lightpurple (users who have tipped 250 recently), darkpurple (users who have tipped 1000 recently), and lightblue (users who own or have purchased tokens). Keep in mind that many users will have multiple categories applied to them, for example a fan club member who is also a moderator will get messages sent with red and green, even though his name only shows in red in chat. Using to_group will always override to_user.
    void sendNotice(const QString &message, const QString &to_user ="", const QString &background = "#FFFFFF", const QString &foreground = "#000000", const QString &weight="", const QString &to_group ="");
};

#endif // CBOBJECTBASE_H
