#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include <QSettings>
#include <QListWidgetItem>
#include <QJsonArray>
#include <QStringList>
#include <QDesktopServices>
#include <QAbstractButton>
#include <QMessageBox>
#include "opensubtitlesapi.h"

const QString OptionsDialog::XOR_KEY = "p?R*XV*9H5*jm2N9";

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog) {
    ui->setupUi(this);

    QToolButton *osButton = findChild<QToolButton*>("openSubtitlesButton");
    connect( osButton, &QAbstractButton::clicked, this, &OptionsDialog::openSubtitlesButton_click );

    api = new OpenSubtitlesAPI(this);
    connect(api, SIGNAL(Response(QJsonObject,QString)), this, SLOT(Response(QJsonObject,QString)));

    osUsername = findChild<QLineEdit*>("osUsername");
    osPassword = findChild<QLineEdit*>("osPassword");
    languageList = findChild<QListWidget*>("languageList");

    PopulateLanguageList();
    LoadSettings();
}

OptionsDialog::~OptionsDialog() {
    delete api;
    delete ui;
}

void OptionsDialog::LoadSettings() {
    QSettings settings("ArcaneSpire", "SubPar");
    settings.beginGroup("OpenSubtitles");

    if(settings.contains("username")) {
        osUsername->setText( settings.value("username", "").toString() );
    }
    if(settings.contains("password")) {
        osPassword->setText( xor_crypt( settings.value("password", "").toString() ));
    }

    settings.endGroup();
}

void OptionsDialog::SaveSettings() {
    QSettings settings("ArcaneSpire", "SubPar");
    settings.beginGroup("OpenSubtitles");

    settings.setValue( "username", osUsername->text() );
    settings.setValue( "password", xor_crypt( osPassword->text() ));

    //check credentials
    api->Login( osUsername->text(), osPassword->text() );

    //languages
    QStringList langs;
    for (int i = 0; i < languageList->count(); i++) {
        QListWidgetItem* item = languageList->item(i);
        if (item->checkState() == Qt::Checked) {
            langs << item->data(LANG).toString();
        }
    }
    settings.setValue( "languages", langs );

    settings.endGroup();
}


void OptionsDialog::on_buttonBox_accepted() {
    SaveSettings();
}

QString OptionsDialog::xor_crypt(QString inputStr) {
    QString result = inputStr;
    for (int i = 0; i < inputStr.length(); i++) {
        result[i] = result[i].fromLatin1(result[i].toLatin1() ^ XOR_KEY[ i % XOR_KEY.size() ].toLatin1());
    }

   return result;
}

void OptionsDialog::PopulateLanguageList() {
    api->GetLanguages();
}

void OptionsDialog::Response(QJsonObject response, QString apiCall) {
    if (apiCall == "GetLanguages") {
        QSettings settings("ArcaneSpire", "SubPar");
        settings.beginGroup("OpenSubtitles");

        QStringList langs = settings.value("languages").toStringList();

        QJsonArray list = response.value("data").toArray();
        for (int i = 0; i < list.count(); i++) {
            QListWidgetItem *listItem = new QListWidgetItem(languageList);
            QString languageCode = list[i].toObject().value("language_code").toString();
            listItem->setCheckState( langs.contains(languageCode) ? Qt::Checked : Qt::Unchecked );
            listItem->setText(list[i].toObject().value("language_name").toString());
            listItem->setData(LANG, languageCode);

            languageList->addItem(listItem);
        }
        languageList->sortItems();
        settings.endGroup();
    } else
    if (apiCall == "Login") {
        qDebug() << "options login" << response;
        if (response["error"].toBool()) {
            //show error message
            QMessageBox::critical(
                        this,
                        tr("SubPar - Login"),
                        response["serverReply"].toObject()["message"].toString()
            );
        }
    }
}

void OptionsDialog::accept() {
    qDebug() << "OptionsDialog::accept";
    //NOTE: we need the save button to do nothing until the Login
    //      call returns and we can then close the dialog if no error
    //      or tell the user if the credentials are invalid
}

void OptionsDialog::openSubtitlesButton_click() {
    //show open subtitles page
    QDesktopServices::openUrl( QUrl("https://www.opensubtitles.com", QUrl::TolerantMode) );
}
