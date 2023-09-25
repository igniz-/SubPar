#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "optionsdialog.h"
#include <QMimeData>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include "opensubtitlesapi.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->setAcceptDrops(true);
    this->installEventFilter(this);

    //Create and add the custom ListView
    listView = new ListView();
    findChild<QVBoxLayout*>("verticalLayout")->addWidget(listView);
    remainingDownloads = findChild<QLabel*>("remainingDownloads");
    remainingDownloads->setText(""); //empty on beginning

    api = new OpenSubtitlesAPI();
    connect(api, SIGNAL(Response(QJsonObject,QString)), this, SLOT(Response(QJsonObject,QString)));

    if (api->authToken == "") {
        QSettings settings("ArcaneSpire", "SubPar");
        settings.beginGroup("OpenSubtitles");

        QString username = settings.value("username", "").toString();
        QString password = OptionsDialog::xor_crypt( settings.value("password", "").toString() );

        settings.endGroup();

        api->Login( username, password );
    }

    model = new FileListModel(this);
    drawDelegate = new QListViewDrawDelegate(this);

    listView->setModel(model);
    listView->setItemDelegate( drawDelegate );

    connect(listView, &QAbstractItemView::activated, this, &MainWindow::activated);
    connect( findChild<QToolButton*>("optionsButton"), &QAbstractButton::clicked, this, &MainWindow::ShowOptionsDialog );
    connect( findChild<QToolButton*>("aboutButton"), &QAbstractButton::clicked, this, &MainWindow::ShowAboutDialog );
}

void MainWindow::activated(const QModelIndex &index) {
    const QFileInfo fi = model->fileInfoAt(index);
    if (fi.isDir()) {
        model->setDirPath(fi.absoluteFilePath());
    }
}

MainWindow::~MainWindow() {
    delete model;
    delete drawDelegate;
    delete api;
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::DragEnter) {
        event->accept();
        return true;
    }
    return false;
}

void MainWindow::dropEvent(QDropEvent * event) {
    event->accept();

    QUrl url;
    foreach (url, event->mimeData()->urls()) {
        api->SearchSubtitle( url.toLocalFile() );
        //add an item to the list
        model->addFile( url.toLocalFile() );
    }
}

void MainWindow::Response(QJsonObject response, QString apiCall) {
    if (apiCall == "SearchSubtitle") {
        int totalCount = response.value("total_count").toInt();
        //we have at least one subtitle
        if (totalCount > 0) {
            //pick the first one
            QJsonArray data = response["data"].toArray();
            QJsonObject first = data.first().toObject();
            QJsonObject attributes = first["attributes"].toObject();
            QJsonObject file = attributes["files"].toArray().first().toObject();

            qDebug() << file["file_id"];
            api->DownloadSubtitle( file["file_id"].toInteger(), response["filepath"].toString() );
            model->updateFileStatus( response["filepath"].toString(), 1 );
        } else {
            //show on the list nothing was found
            model->updateFileStatus( response["filepath"].toString(), 2 );
        }
    } else
    if (apiCall == "DownloadSubtitle") {
        api->DownloadFile(response["link"].toString(), response["filepath"].toString());
        remainingDownloads->setText("Remaining Downloads: " + QString::number(response["remaining"].toInt()));
    } else
    if (apiCall == "DownloadFile") {
        //this means we got a subtitle and downloaded it
        model->updateFileStatus( response["filepath"].toString(), 3 );
    } else
    if (apiCall == "Login") {
        QJsonObject user = response["user"].toObject();
        remainingDownloads->setText("Remaining Downloads: " + QString::number(user["allowed_downloads"].toInt()));
    }
}

void MainWindow::ShowOptionsDialog() {
    OptionsDialog* dialog = new OptionsDialog(this);
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::ShowAboutDialog() {
    AboutDialog* dialog = new AboutDialog(this);
    dialog->exec();
    dialog->deleteLater();
}
