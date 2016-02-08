#ifndef USERS_H
#define USERS_H

#include <QObject>
#include <QAbstractListModel>
#include <QSet>

#include "user.h"

//! Container and model for User* @sa class User
class Users : public QAbstractListModel
{
    Q_OBJECT
public:
    Users(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const {Q_UNUSED(parent) return m_users.size();}
    QVariant data(const QModelIndex &index, int role) const {Q_UNUSED(role) return m_users.at(index.row())->name;}

    //! Sets parent and adds User to the model
    User* addUser(User* u);

    //! @returns list of users
    QList<User *> getList() const {return m_users;}

    //! Delets all Users and resets the model
    void clear();

    //! @note Always save to call. @returns User * at idx or Q_NULLPTR if idx is out of bounds
    Q_INVOKABLE User* getUser(const int& idx) const;

private:
    QList<User *> m_users;

    QSet<QString> m_reserved_names = QSet<QString>() << "" << "red" << "green" << "orange" << "lightblue" << "darkblue" << "lightpurple" << "darkpurple" << "grey";
};

#endif // USERS_H
