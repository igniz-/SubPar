#ifndef QFILEITEM_H
#define QFILEITEM_H

#include <QFileInfo>

class QFileItem : public QFileInfo {
private:
    int _status;

public:
    explicit QFileItem(const QString &name);
    int status();
    void setStatus(int newStatus);
};

#endif // QFILEITEM_H
