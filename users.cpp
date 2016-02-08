#include "users.h"

Users::Users(QObject *parent) : QAbstractListModel(parent)
{
    addUser(new User("llua",0,0,0,1,'c'));
    m_users.at(0)->broadcaster = true;
}

User *Users::addUser(User *u)
{
    bool canadd = true;
    if (m_reserved_names.contains(u->name)) canadd = false;
    for (auto i = m_users.constBegin(); i != m_users.constEnd(); ++i)
        if ((*i)->name == u->name)
            canadd = false;

    if (canadd)
    {
        beginInsertRows(QModelIndex(), m_users.size(), m_users.size());
        u->setParent(this);
        m_users << u;
        endInsertRows();
        return u;
    }
    return Q_NULLPTR;
}

void Users::clear()
{
    beginResetModel();
    for (auto i = m_users.constBegin(); i != m_users.constEnd(); ++i)
        (*i)->deleteLater();
    m_users.clear();
    endResetModel();
}

User *Users::getUser(const int& idx) const
{
    if (idx < 0 || idx > m_users.size()-1)
        return Q_NULLPTR;
    return m_users.at(idx);
}

