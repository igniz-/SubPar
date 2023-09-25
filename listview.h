#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class ListView : public QListView {
private:
    void paintEvent(QPaintEvent *e);
public:
    ListView();
};

#endif // LISTVIEW_H
