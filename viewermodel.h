#ifndef VIEWERMODEL_H
#define VIEWERMODEL_H

#include <QAbstractListModel>
#include <QSet>

#include "viewer.h"

//!Model for Viewers
class ViewerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ViewerModel(QObject *parent = 0);

    enum ModelRoles {
        Name = Qt::UserRole + 100,
        Moderator,
        Fanclub,
        Has_Tokens
    };


    //! Reimplemented from QAbstractItemModel.
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const {Q_UNUSED(parent) return m_viewers.size();}

    //! Sets parent and adds Viewer to the model. @returns v or Q_NULLPTR if a user with that name already exists. @warning Some names are reserved, in that case Q_NULLPTR is returned.
    Viewer* addViewer(Viewer* v);

    //! @returns Viewer* at idx or Q_NULLPTR if idx is out of bounds
    Q_INVOKABLE Viewer* getViewer(const int& idx) const;

    //! @returns Viewer* with the given name or Q_NULLPTR if no such Viewer exists.
    Q_INVOKABLE Viewer* getViewerByName(const QString& name) const;

    //! @returns a JSON compatible VariantList of all viewers with their non-default values.
    QVariantList serializeViewers() const;

    //! Resets the model with the viewers provided. @warning Some names are reserved and duplicates aren't allowed. Viewers that fail those restrictions won't be added and parented.
    void populate(QList<Viewer*> l);

    //! Returns a QList of all viewers.
    QList<Viewer *> viewers() const {return m_viewers;}


protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<Viewer *> m_viewers;

    const QSet<QString> m_reserved_names = QSet<QString>() << "" << "red" << "green" << "orange" << "lightblue" << "darkblue" << "lightpurple" << "darkpurple" << "grey" << "cblog" << "roomsubject";

private slots:
    //! Emits dataChanged() if a viewer's values change.
    void viewerDataChanged();
};

#endif // VIEWERMODEL_H
