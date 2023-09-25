#ifndef QLISTVIEWDRAWDELEGATE_H
#define QLISTVIEWDRAWDELEGATE_H

#include <QItemDelegate>

class QListViewDrawDelegate : public QItemDelegate {
    Q_OBJECT
public:
    explicit QListViewDrawDelegate(QObject *parent = nullptr);

//    QWidget* createEditor(QWidget *parent,
//                        const QStyleOptionViewItem &option,
//                        const QModelIndex &index);
    void paint(QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index) const override;
signals:

};

#endif // QLISTVIEWDRAWDELEGATE_H
