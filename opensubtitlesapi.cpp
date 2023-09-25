#include "opensubtitlesapi.h"
#include <QSsl>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDateTime>
#include <QBuffer>
#include <QSettings>
#include <QDir>
#include "optionsdialog.h"

OpenSubtitlesAPI::OpenSubtitlesAPI(QObject *parent) : QObject{parent} {
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

OpenSubtitlesAPI::~OpenSubtitlesAPI() {
    delete manager;
}

void OpenSubtitlesAPI::CallAPI_Post(QString url, QJsonObject params, bool needsAuth, QString reqId) {
    qDebug() << apiCall;
    QString cachedContent = GetCachedResponse();
    if (cachedContent != "") {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(cachedContent.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
        emit Response( jsonObj, apiCall );

        return;
    }

    QNetworkRequest request(BASE_URL + url);

    request.setRawHeader("api-key", API_KEY);
    if (needsAuth) {
        request.setRawHeader("authorization", "Bearer " + authToken.toUtf8());
    }
    request.setRawHeader("accept", "*/*");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "SubPar 1.0");
    request.setAttribute(QNetworkRequest::User, reqId);

    qDebug() << "3 - " << url << reqId;

    QJsonDocument doc(params);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    dbReply = manager->post(request, strJson.toUtf8());
}

void OpenSubtitlesAPI::CallAPI_Get(QString url, QUrlQuery params, QString reqId) {
    QString cachedContent = GetCachedResponse();
    if (cachedContent != "") {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(cachedContent.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
        emit Response( jsonObj, apiCall );

        return;
    }

    QString fullUrl = url;
    if (apiCall != "DownloadFile") {
        fullUrl = BASE_URL + url + "?" + params.toString(QUrl::FullyEncoded);
    }
    QNetworkRequest request(fullUrl);

    request.setRawHeader("accept", "*/*");
    request.setRawHeader("Api-Key", API_KEY);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "SubPar 1.0");
    request.setAttribute(QNetworkRequest::User, reqId);

    qDebug() << "2 - " << url << reqId;

    manager->get(request);
}

QString OpenSubtitlesAPI::GetCachedResponse() {
    QString filename = apiCall + ".cache";
    QFileInfo check_file(filename);
    // check response should be cached, file exists, it is really a file and no older than 7 days
    if (cacheResponse && check_file.exists() && check_file.isFile() && (check_file.birthTime().daysTo(QDateTime::currentDateTime()) < 7)) {
        //if all true, then use the cached response
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QString cachedContent = file.readAll();

            return cachedContent; //no need to continue
        }
    }
    return "";
}

void OpenSubtitlesAPI::replyFinished(QNetworkReply* response) {
    qDebug() << "replyFinished: " << apiCall;
    //check for errors
    if (response->error() != QNetworkReply::NoError) {
        QString strReply = (QString)response->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject serverReply = jsonResponse.object();

        QJsonObject jsonObj;
        jsonObj["error"] = true;
        jsonObj["errorMessage"] = response->errorString();
        jsonObj["serverReply"] = serverReply;

        qDebug() << response;

        emit Response( jsonObj, apiCall );
        return;
    }

    QString _filepath = response->request().attribute(QNetworkRequest::User).toString();
    qDebug() << "1 -" << _filepath;
    //if we tried to download a file, save it
    if (apiCall == "DownloadFile") {
        QFileInfo fileInfo(_filepath);
        QString filename = fileInfo.absoluteDir().absolutePath() + "/" + fileInfo.completeBaseName() + ".srt";

        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            file.write(response->readAll());

            file.close();
        }
        QJsonObject jsonObj;
        jsonObj["filepath"] = _filepath;
        jsonObj["subtitle"] = filename;
        emit Response( jsonObj, apiCall );
        return;
    }

    //all other responses
    QString strReply = (QString)response->readAll();

    if (cacheResponse) {
        QString filename = apiCall + ".cache";
        QFile file(filename);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            stream << strReply << Qt::endl;

            file.close();
        }
    }

    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject jsonObj = jsonResponse.object();

    //Login is a special case
    //We only need it to get the token to download a subtitle
    if (apiCall == "Login") {
        authToken = jsonObj["token"].toString();
        emit Response( jsonObj, apiCall );
        return;
    }

    qDebug() << "filepath: " << _filepath;
    jsonObj["filepath"] = _filepath; // request.attribute(QNetworkRequest::User).toJsonValue();

    emit Response( jsonObj, apiCall );
    response->deleteLater();
}

void OpenSubtitlesAPI::Login(QString username, QString password) {
    cacheResponse = false;
    apiCall = "Login";

    QJsonObject params;
    params["username"] = username;
    params["password"] = password;
    CallAPI_Post( "login", params );
}

QString OpenSubtitlesAPI::CalcHash(QString filepath) {
    QFile file(filepath);
    uint64_t hash = 0;

    if (file.open(QIODevice::ReadOnly)) {
        //begin with the file size, as it is part of the hash
        uint64_t hash = file.size();
        //first 8192 bytes
        file.seek(0);
        for(int i = 0; i < 65536/sizeof(hash); i++) {
            uint64_t temp = 0;
            file.read((char*)&temp, sizeof(temp));
            hash += temp;
        }

        //last 8192 bytes
        file.seek(file.size() - 65536/sizeof(hash));
        for(int i = 0; i < 65536/sizeof(hash); i++) {
            uint64_t temp = 0;
            file.read((char*)&temp, sizeof(temp));
            hash += temp;
        }

        file.close();
        //first 16 is length of hash, second is base (hex)
        return QStringLiteral("%1").arg(hash, 16, 16, QLatin1Char('0'));
    }
    return "";
}

void OpenSubtitlesAPI::GetLanguages() {
    cacheResponse = true;
    apiCall = "GetLanguages";
    QUrlQuery params;
    CallAPI_Get("infos/languages", params);
}

void OpenSubtitlesAPI::SearchSubtitle(QString filepath) {
    cacheResponse = false;
    apiCall = "SearchSubtitle";

    QString hash = CalcHash(filepath);

    QFileInfo fileInfo(filepath);
    QString fileName = fileInfo.fileName();

    QSettings settings("ArcaneSpire", "SubPar");
    settings.beginGroup("OpenSubtitles");
    QStringList langs = settings.value("languages").toStringList();
    settings.endGroup();

    QUrlQuery params;
    params.addQueryItem("moviehash", hash);
    params.addQueryItem("query", fileName);
    params.addQueryItem("languages", langs.join(","));
    CallAPI_Get( "subtitles", params, filepath );
}

void OpenSubtitlesAPI::DownloadSubtitle( qint64 subtitle_id, QString _filepath ) {
    cacheResponse = false;
    apiCall = "DownloadSubtitle";

    //if we have no authToken we need to login
    if (authToken == "") {
        QSettings settings("ArcaneSpire", "SubPar");
        settings.beginGroup("OpenSubtitles");
        QString username = settings.value("username").toString();
        QString password = OptionsDialog::xor_crypt(settings.value("password").toString());
        Login( username, password );
        settings.endGroup();
    } else {
        QJsonObject params;
        params["file_id"] = subtitle_id;
        CallAPI_Post("download", params, true, _filepath);
    }
}

void OpenSubtitlesAPI::DownloadFile(QString url, QString filepath) {
    cacheResponse = false;
    apiCall = "DownloadFile";

    QUrlQuery params;
    CallAPI_Get(url, params, filepath);
}
