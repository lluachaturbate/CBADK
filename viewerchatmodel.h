#ifndef VIEWERCHATMODEL_H
#define VIEWERCHATMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QApplication>
#include <QClipboard>

#include "chatmodel.h"

//! Proxy model to only show ChatLines the user can read.
class ViewerChatModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ViewerChatModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

    //! Sets the current Viewer.
    Q_INVOKABLE void setViewer(Viewer* v);

    //! Calls Chatmodel::clear of the sourceModel().
    Q_INVOKABLE void clearChat();

    //! Copies given string to the clipboard.
    Q_INVOKABLE void copyToClipboard(const QString& string) const;

protected:
    //! Reimplemented: Returns Viewer::canRead() to filter ChatLines that should be hidden.
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    Viewer* m_viewer;
};

#endif // VIEWERCHATMODEL_H
