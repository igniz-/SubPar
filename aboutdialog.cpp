#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QDesktopServices>
#include <QLabel>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog) {

    ui->setupUi(this);
    connect( findChild<QLabel*>("linkLabel"), &QLabel::linkActivated, this, &AboutDialog::OpenLink );
}

AboutDialog::~AboutDialog() {
    delete ui;
}

void AboutDialog::OpenLink(const QString &link) {
    QDesktopServices::openUrl( QUrl("https://www.arcanespire.com/subpar", QUrl::TolerantMode) );
}

