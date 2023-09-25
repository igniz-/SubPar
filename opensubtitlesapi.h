#ifndef OPENSUBTITLESAPI_H
#define OPENSUBTITLESAPI_H

#include <QObject>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>

class OpenSubtitlesAPI : public QObject {
    Q_OBJECT

private:
    const QByteArray API_KEY = "PUT_YOUR_API_KEY_HERE!";
    const QString BASE_URL = "https://api.opensubtitles.com/api/v1/";

    QNetworkReply* dbReply;
    QNetworkAccessManager* manager;
    QString apiCall;
    bool cacheResponse;

    //store these for when login returns
    QString _subtitle_id;

    void CallAPI_Get(QString url, QUrlQuery params, QString reqId = "");
    void CallAPI_Post(QString url, QJsonObject params, bool needsAuth = false, QString reqId = "");
    QString GetCachedResponse();
    QString CalcHash(QString filepath);

private slots:
    void replyFinished(QNetworkReply* response);

signals:
    void Response(QJsonObject response, QString apiCall);

public:
    explicit OpenSubtitlesAPI(QObject *parent = nullptr);
    ~OpenSubtitlesAPI();

    QString authToken;

    void GetLanguages();
    void SearchSubtitle(QString filepath);
    void Login(QString username, QString password);
    void DownloadSubtitle(qint64 subtitle_id, QString _filepath);
    void DownloadFile(QString url, QString filepath);
};

#endif // OPENSUBTITLESAPI_H
