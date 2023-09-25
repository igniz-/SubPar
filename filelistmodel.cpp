// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "filelistmodel.h"

#include <QGuiApplication>
#include <QDir>
#include <QPalette>

static const int batchSize = 100;

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int FileListModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : fileCount;
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    const int row = index.row();

    if (row >= fileList.size() || row < 0)
        return {};

    switch (role) {
        case Qt::DisplayRole:
            return fileList.at(row).fileName();
        case Qt::BackgroundRole: {
            const QPalette &palette = QGuiApplication::palette();
            return palette.Window;
        }
        case Qt::DecorationRole:
            return iconProvider.icon(fileList.at(row));
        case Qt::StatusTipRole: {
            QFileItem item = fileList.at(row);
            return item.status();
        }
    }
    return {};
}

QFileItem FileListModel::fileInfoAt(const QModelIndex &index) const {
    return fileList.at(index.row());
}

QFileItem FileListModel::fileInfoAt(const int index) const {
    return fileList.at(index);
}

bool FileListModel::canFetchMore(const QModelIndex &parent) const {
    if (parent.isValid())
        return false;
    return (fileCount < fileList.size());
}

void FileListModel::fetchMore(const QModelIndex &parent) {
    if (parent.isValid())
        return;
    const int start = fileCount;
    const int remainder = int(fileList.size()) - start;
    const int itemsToFetch = qMin(batchSize, remainder);

    if (itemsToFetch <= 0)
        return;

    beginInsertRows(QModelIndex(), start, start + itemsToFetch - 1);

    fileCount += itemsToFetch;

    endInsertRows();

    emit numberPopulated(path, start, itemsToFetch, int(fileList.size()));
}

void FileListModel::setDirPath(const QString &path) {
    QDir dir(path);

    beginResetModel();
    this->path = path;
//    fileList = dir.entryInfoList(QDir::NoDot | QDir::AllEntries, QDir::Name);
    fileCount = 0;
    endResetModel();
}

void FileListModel::addFile(const QString &path) {
    beginResetModel();

    QFileItem file(path);
    if (file.isDir()) {
        //add all files inside
        QDir dir(path);
        QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::Name);

        fileList.clear();
        foreach (QFileInfo fileInfo, list) {
            file = QFileItem(fileInfo.absoluteFilePath());
            fileList.append(file);
        }

        fileCount = 0;
    } else {
        fileList.append(file);
        fileCount++;
    }

    endResetModel();
}

void FileListModel::updateFileStatus(const QString &path, int newStatus) {
    int index = -1; //invalid index
    for (int i = 0; i < fileList.count() && index == -1; i++) {
        QFileItem item = fileList.at(i);
        if (item.filePath() == path) {
            index = i;
        }
    }
    if (index != -1)
        updateFileStatus(index, newStatus);
}

void FileListModel::updateFileStatus(const int index, int newStatus) {
    beginResetModel();

    QFileItem item = fileList.at(index);
    item.setStatus(newStatus);
    fileList.replace(index, item);

    endResetModel();
}
