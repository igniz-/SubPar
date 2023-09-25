#include "qlistviewdrawdelegate.h"
#include <QPainter>
#include <QScrollBar>

QListViewDrawDelegate::QListViewDrawDelegate(QObject *parent): QItemDelegate{parent} {
}

void QListViewDrawDelegate::paint(QPainter *painter,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const {

    const int status = index.model()->data(index, Qt::StatusTipRole).toInt();

    QItemDelegate::paint(painter, option, index);
    //draw the status
    // 0 = added and looking for it        - looking glass
    // 1 = subtitle found, downloading it  - down arrow
    // 2 = subtitle not found              - forbidden sign
    // 3 = subtitle downloaded             - tick mark

    int ypos = ((option.rect.height() / 2) - 16) + option.rect.top();
    int xpos = option.widget->rect().width() - 32;

    //adjust icon position if a vertical scrollbar is showing
    QList<QScrollBar*> scrollbars = option.widget->findChildren<QScrollBar*>();
    if (scrollbars.count() > 0) {
        //if there's only one scroll bar
        if (scrollbars[0]->orientation() == Qt::Vertical && scrollbars[0]->isVisible()) {
            xpos -= scrollbars[0]->width();
        } else
        if (scrollbars.count() > 1 && scrollbars[1]->orientation() == Qt::Vertical && scrollbars[1]->isVisible()) {
            xpos -= scrollbars[1]->width();
        }
    }

    switch (status) {
    case 0:
        painter->fillRect(option.rect, QColor(255,255,255,150));
        painter->drawImage( QPoint(xpos, ypos), QImage(":/images/images/searching.png") );
        break;
    case 1:
        painter->fillRect(option.rect, QColor(255,255,255,150));
        painter->drawImage( QPoint(xpos, ypos), QImage(":/images/images/downloading.png") );
        break;
    case 2:
        painter->drawImage( QPoint(xpos, ypos), QImage(":/images/images/not_found.png") );
        break;
    case 3:
        painter->drawImage( QPoint(xpos, ypos), QImage(":/images/images/success.png") );
        break;

    default:
        break;
    }
}

