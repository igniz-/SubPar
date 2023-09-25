#include "qfileitem.h"

QFileItem::QFileItem(const QString &name) : QFileInfo(name) {
    this->_status = 0;
}

int QFileItem::status() {
    return this->_status;
}

void QFileItem::setStatus(int newStatus) {
    this->_status = newStatus;
}
