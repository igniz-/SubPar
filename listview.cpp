#include "listview.h"
#include <QPainter>

void ListView::paintEvent(QPaintEvent *e) {
    QListView::paintEvent(e);
    if (model() && model()->rowCount(rootIndex()) > 0) return;
    // The view is empty.
    QPainter p(this->viewport());
    p.drawText(rect(), Qt::AlignCenter, "Drop some files...");
    QPen pen = p.pen();
    pen.setColor( QColor::fromRgb(190, 190, 190) );
    pen.setWidth(5);
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);
    p.drawRoundedRect( rect().adjusted(5, 5, -5, -5), 10, 10 );
}

ListView::ListView() {
    //set visual properties
    QFont font = this->font();
    font.setPointSize(16);
    this->setFont(font);
    this->setIconSize( QSize(64, 64) );
    this->setFrameStyle( Shape::NoFrame );
}
