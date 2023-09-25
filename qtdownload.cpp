#include "qtdownload.h"
#include <QCoreApplication>
#include <QUrl>
#include <QNetworkRequest>
#include <QFile>
#include <QDebug>

QtDownload::QtDownload(QObject *parent) : QObject{parent} {
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
}

QtDownload::~QtDownload() {
    delete manager;
}

void QtDownload::downloadFile(const QString& url, const QString& target) {
    this->url = url;
    this->target = target;

    qDebug() << target;

    QNetworkRequest request(this->url);
    request.setRawHeader("accept", "*/*");

    reply = manager->get(request);
    qDebug() << reply;
}

void QtDownload::downloadFinished(QNetworkReply *data) {
    qDebug() << "downloadFinished";

    QFile localFile(target);
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    const QByteArray sdata = data->readAll();
    localFile.write(sdata);
    qDebug() << sdata;
    localFile.close();

    emit done();
}

void QtDownload::downloadError() {
    qDebug() << reply->errorString();
}

void QtDownload::download() {
//    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
//    qDebug() << reply->errorString();
}

void QtDownload::downloadProgress(qint64 recieved, qint64 total) {
    qDebug() << recieved << total;
}
