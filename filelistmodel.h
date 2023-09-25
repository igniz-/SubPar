// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QFileInfoList>
#include <QFileIconProvider>
#include "qfileitem.h"

class FileListModel : public QAbstractListModel {
    Q_OBJECT

public:
    FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QFileItem fileInfoAt(const QModelIndex &) const;
    QFileItem fileInfoAt(const int index) const;

signals:
    void numberPopulated(const QString &path, int start, int number, int total);

public slots:
    void setDirPath(const QString &path);
    void addFile(const QString &path);
    void updateFileStatus(const QString &path, int newStatus);
    void updateFileStatus(const int index, int newStatus);

protected:
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    QList<QFileItem> fileList;
    QString path;
    QFileIconProvider iconProvider;
    int fileCount = 0;
};

#endif // FILELISTMODEL_H

