#include "viewermodel.h"

ViewerModel::ViewerModel(QObject *parent) : QAbstractListModel(parent)
{
}

Viewer* ViewerModel::addViewer(Viewer* v)
{
    if (m_reserved_names.contains(v->getName()))
        return Q_NULLPTR;
    for (auto i = m_viewers.constBegin(); i != m_viewers.constEnd(); ++i)
        if ((*i)->getName() == v->getName())
            return Q_NULLPTR;

    beginInsertRows(QModelIndex(), m_viewers.size(), m_viewers.size());
    v->setParent(this);
    m_viewers << v;
    endInsertRows();
    return v;
}

Viewer* ViewerModel::getViewer(const int& idx) const
{
    if (idx < 0 || idx > m_viewers.size()-1)
        return Q_NULLPTR;
    return m_viewers.at(idx);
}

Viewer *ViewerModel::getViewerByName(const QString &name) const
{
    for (auto i = m_viewers.constBegin(); i != m_viewers.constEnd(); ++i)
        if ((*i)->getName() == name)
            return (*i);
    return Q_NULLPTR;
}
