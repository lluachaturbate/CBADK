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

QVariantList ViewerModel::serializeViewers() const
{
    QVariantList a;
    for (auto i = m_viewers.constBegin(); i != m_viewers.constEnd(); ++i)
    {
        Viewer def("def");
        QVariantMap o;
        o.insert("name", (*i)->getName());
        if (def.getGender() != (*i)->getGender())
            o.insert("gender", (*i)->getGender());
        if (def.getTipped() != (*i)->getTipped())
            o.insert("tipped", (*i)->getTipped());
        if (def.getFont() != (*i)->getFont())
            o.insert("font", (*i)->getFont());
        if (def.getTextcolor() != (*i)->getTextcolor())
            o.insert("textcolor", (*i)->getTextcolor());
        if (def.hasTokens() != (*i)->hasTokens())
            o.insert("hastokens", (*i)->hasTokens());
        if (def.isModerator() != (*i)->isModerator())
            o.insert("mod", (*i)->isModerator());
        if (def.isFanclubmember() != (*i)->isFanclubmember())
            o.insert("fanclub", (*i)->isFanclubmember());
        if (def.isRoomOwner() != (*i)->isRoomOwner())
            o.insert("roomowner", (*i)->isRoomOwner());
        a.append(o);
    }
    return a;
}

void ViewerModel::populate(QList<Viewer*> l)
{
    beginResetModel();
    for (auto i = m_viewers.constBegin(); i != m_viewers.constEnd(); ++i)
        (*i)->deleteLater();
    m_viewers.clear();
    QStringList names = m_reserved_names.toList();
    for (auto i = l.constBegin(); i != l.constEnd(); ++i)
    {
        if (names.contains((*i)->getName()))
            break;
        names << (*i)->getName();
        (*i)->setParent(this);
        m_viewers << (*i);
    }
    endResetModel();
}
