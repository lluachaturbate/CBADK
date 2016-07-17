#include "viewerchatmodel.h"


void ViewerChatModel::setViewer(Viewer *v)
{
    m_viewer = v;
    invalidateFilter();
}

void ViewerChatModel::clearChat()
{
    ChatModel* c = qobject_cast<ChatModel *>(sourceModel());
    if (c)
        c->clear();
}

void ViewerChatModel::copyToClipboard(const QString &string) const
{
    QClipboard *cb = QApplication::clipboard();
    if (cb)
        cb->setText(QString(string));
}

bool ViewerChatModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return m_viewer && m_viewer->canRead(sourceModel()->index(source_row,0, source_parent).data(ChatModel::To).toString());
}
