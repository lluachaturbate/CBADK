#include "chatmodel.h"

ChatModel::ChatModel(QObject* parent) : QAbstractListModel(parent)
{
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        case Message: return m_chatlines.at(index.row())->getMessage();
        case ForeGround: return m_chatlines.at(index.row())->getForegroundColor();
        case BackGround: return m_chatlines.at(index.row())->getBackgroundColor();
        case To: return m_chatlines.at(index.row())->getVisibleTo();
        case Weight: return m_chatlines.at(index.row())->getWeight();
        case Tip: return m_chatlines.at(index.row())->getTipAmount();
        case Line: return m_chatlines.at(index.row())->getLine(m_resolveimages);
    }
    return QVariant();
}

QHash<int, QByteArray> ChatModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Message] = "message";
    roles[ForeGround] = "foreground";
    roles[BackGround] = "background";
    roles[To] = "to";
    roles[Weight] = "weight";
    roles[Tip] = "tip";
    roles[Line] = "line";
    return roles;
}

ChatLine *ChatModel::addLine(ChatLine *line)
{
    beginInsertRows(QModelIndex(), m_chatlines.size(), m_chatlines.size());
    m_chatlines << line;
    endInsertRows();
    return line;
}

void ChatModel::clear()
{
    beginResetModel();
    for (auto i = m_chatlines.constBegin(); i != m_chatlines.constEnd(); ++i)
        delete (*i);
    m_chatlines.clear();
    endResetModel();
}

void ChatModel::viewerDataChanged()
{
    Viewer* v = qobject_cast<Viewer *>(sender());
    for (int i = 0; i < m_chatlines.size(); ++i)
    {
        Viewer* mv = m_chatlines.at(i)->getViewer();
        if (!mv || mv == v)
        {
            QModelIndex idx = createIndex(i,0);
            emit dataChanged(idx,idx);
        }
    }
}
