#ifndef QTDOWNLOAD_H
#define QTDOWNLOAD_H

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


class QtDownload : public QObject {
    Q_OBJECT
public:
    explicit QtDownload(QObject *parent = nullptr);
    ~QtDownload();

    void downloadFile(const QString& url, const QString& target);

private:
    QNetworkAccessManager* manager;
    QString url;
    QString target;
    QNetworkReply* reply;

signals:
    void done();

public slots:
    void download();
    void downloadFinished(QNetworkReply* data);
    void downloadProgress(qint64 recieved, qint64 total);

    void downloadError();
};

#endif // QTDOWNLOAD_H
