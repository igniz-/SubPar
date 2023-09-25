#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDropEvent>
#include <QListView>
#include <QLabel>
#include "opensubtitlesapi.h"
#include "filelistmodel.h"
#include "qlistviewdrawdelegate.h"
#include "listview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool eventFilter(QObject *obj, QEvent *event);
    bool dropMimeData(int index, const QMimeData *data, Qt::DropAction action);

protected:
    void dropEvent(QDropEvent *event);

private slots:
    void ShowOptionsDialog();
    void ShowAboutDialog();
    void Response(QJsonObject response, QString apiCall);

public slots:
    void activated(const QModelIndex &);

private:
    Ui::MainWindow *ui;
    OpenSubtitlesAPI* api;

    ListView *listView;
    FileListModel *model;
    QListViewDrawDelegate* drawDelegate;
    QLabel* remainingDownloads;
};
#endif // MAINWINDOW_H
