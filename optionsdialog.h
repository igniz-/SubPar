#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include "opensubtitlesapi.h"

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog {
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog();
    static QString xor_crypt(QString inputStr);

private slots:
    void on_buttonBox_accepted();
    void Response(QJsonObject response, QString apiCall);
    void accept() override;

private:
    static const QString XOR_KEY;
    const int LANG = 1;

    OpenSubtitlesAPI* api;

    Ui::OptionsDialog *ui;
    QLineEdit* osUsername;
    QLineEdit* osPassword;
    QListWidget* languageList;

    void LoadSettings();
    void SaveSettings();
    void PopulateLanguageList();
    void openSubtitlesButton_click();
};

#endif // OPTIONSDIALOG_H
