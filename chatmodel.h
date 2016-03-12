#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include "chatline.h"

//! Model for ChatLines
class ChatModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ChatModel(QObject *parent = 0);

    //! Roles for data() and rolesNames()
    enum ModelRoles {
        Message = Qt::UserRole + 100,
        ForeGround,
        BackGround,
        To,
        Weight,
        Tip,
        Line
    };
    //! Reimplementation from QAbstractItemModel::data().
    QVariant data(const QModelIndex &index, int role) const;
    //! Reimplementation from QAbstractItemModel::rowCount().
    int rowCount(const QModelIndex &parent) const {Q_UNUSED(parent) return m_chatlines.size();}
    //! Reimplementation from QAbstractItemModel::roleNames().
    QHash<int, QByteArray> roleNames() const;

    //! Adds line to the model.
    ChatLine* addLine(ChatLine *line);

    //! Deletes all ChatLines in the model.
    void clear();

private:
    QList<ChatLine *> m_chatlines;

public slots:
    //! Emits QAbstractItemModel::dataChanged() to update views. @warning Expects Viewer* as sender().
    void viewerDataChanged();
};

#endif // CHATMODEL_H
