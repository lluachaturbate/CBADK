#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "chatmessage.h"
#include "user.h"

//! Model and container for ChatMessages
class ChatModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatModel(QObject *parent = 0);

    //! Roles for roleNames() and data()
    enum ModelRoles {
        Message = Qt::UserRole + 100,
        ForeGround,
        BackGround,
        UserP,
        To,
        Weight
    };
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const {Q_UNUSED(parent) return m_messages.size();}
    QHash<int, QByteArray> roleNames() const;

    //! Adds m and sets its parent to the model
    ChatMessage* addMessage(ChatMessage *m);

    //! Deletes all ChatMessages in the model.
    void clear();

    
private:
    QList<ChatMessage *> m_messages;

public slots:
    //! Creates a new ChatMessage in the model. @warning requires User * as sender!
    void onUserTip(const int& amount, const QString &msg);
    //! Creates a new ChatMessage in the model
    void onUserChat(const QString &msg, User *u, const QString &background);
    //! Creates a new ChatMessage in the model
    void onAppNotice(const QString &message, const QString &to_user, const QString &bg, const QString &fg, const QString &weight, const QString &to_group);
};

#endif // CHATMODEL_H
