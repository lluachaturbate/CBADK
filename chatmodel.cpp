#include "chatmodel.h"

ChatModel::ChatModel(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        default:
        case Qt::DisplayRole:
        case Message: return m_messages.at(index.row())->msg;
        case ForeGround: return m_messages.at(index.row())->fg;
        case BackGround: return m_messages.at(index.row())->bg;
        case UserP: return QVariant::fromValue(m_messages.at(index.row())->user);
        case To: return m_messages.at(index.row())->to;
        case Weight: return m_messages.at(index.row())->weight;
    }
    return QVariant();
}

QHash<int, QByteArray> ChatModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Message] = "message";
    roles[ForeGround] = "foreground";
    roles[BackGround] = "background";
    roles[UserP] = "user";
    roles[To] = "to";
    roles[Weight] = "weight";
    return roles;
}

ChatMessage *ChatModel::addMessage(ChatMessage *m)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m->setParent(this);
    m_messages << m;
    endInsertRows();
    return m;
}

void ChatModel::clear()
{
    beginResetModel();
    for (auto i = m_messages.constBegin(); i != m_messages.constEnd(); ++i)
        (*i)->deleteLater();
    m_messages.clear();
    endResetModel();
}

void ChatModel::onUserTip(const int &amount, const QString& msg)
{
    User *u = qobject_cast<User *>(sender());
    QString s = QString("%1 tipped %2 tokens").arg(u->name).arg(amount);
    if (!msg.trimmed().isEmpty())
        s.append(" -- " + msg);
    addMessage(new ChatMessage(s, u, "#ffff33"));
}

void ChatModel::onUserChat(const QString& msg, User* u, const QString& background)
{
    addMessage(new ChatMessage(msg, u, background));
}

void ChatModel::onAppNotice(const QString& message, const QString& to_user, const QString& bg, const QString& fg, const QString& weight, const QString& to_group)
{
    QStringList l = message.split("\n");
    for (auto i = l.constBegin(); i != l.constEnd(); ++i)
        addMessage(new ChatMessage((*i), !fg.isEmpty() ? fg : "#000000", !bg.isEmpty() ? bg : "#FFFFFF", to_group.isEmpty() ? to_user : to_group, !weight.isEmpty() ? weight : "normal"));
}

