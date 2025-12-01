#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QSet>
#include <QBrush>
#include <QColor>
#include <QApplication>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    connect(ui->pushButtonBrowse1, SIGNAL(clicked()), this, SLOT(onBrowseFolder1()));
    connect(ui->pushButtonBrowse2, SIGNAL(clicked()), this, SLOT(onBrowseFolder2()));
    connect(ui->pushButtonCompare, SIGNAL(clicked()), this, SLOT(onCompare()));
    connect(ui->pushButtonCopyTo2, SIGNAL(clicked()), this, SLOT(onCopyToFolder2()));
    connect(ui->pushButtonCopyTo1, SIGNAL(clicked()), this, SLOT(onCopyToFolder1()));
    connect(ui->pushButtonMoveTo2, SIGNAL(clicked()), this, SLOT(onMoveToFolder2()));
    connect(ui->pushButtonMoveTo1, SIGNAL(clicked()), this, SLOT(onMoveToFolder1()));
    
    QStringList headers1;
    headers1 << "Status" << "File/Folder" << "Modified Date" << "Size";
    ui->treeWidgetFolder1->setHeaderLabels(headers1);
    ui->treeWidgetFolder1->setColumnCount(4);
    ui->treeWidgetFolder1->setColumnWidth(0, 120);
    ui->treeWidgetFolder1->setColumnWidth(1, 400);
    ui->treeWidgetFolder1->setColumnWidth(2, 150);
    ui->treeWidgetFolder1->setColumnWidth(3, 100);
    connect(ui->treeWidgetFolder1, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(ui->treeWidgetFolder1->verticalScrollBar(), SIGNAL(valueChanged(int)), 
            this, SLOT(onFolder1ScrollChanged(int)));
    
    QStringList headers2;
    headers2 << "Status" << "File/Folder" << "Modified Date" << "Size";
    ui->treeWidgetFolder2->setHeaderLabels(headers2);
    ui->treeWidgetFolder2->setColumnCount(4);
    ui->treeWidgetFolder2->setColumnWidth(0, 120);
    ui->treeWidgetFolder2->setColumnWidth(1, 400);
    ui->treeWidgetFolder2->setColumnWidth(2, 150);
    ui->treeWidgetFolder2->setColumnWidth(3, 100);
    connect(ui->treeWidgetFolder2, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(ui->treeWidgetFolder2->verticalScrollBar(), SIGNAL(valueChanged(int)), 
            this, SLOT(onFolder2ScrollChanged(int)));
    
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);
    
    syncingScroll = false;
    
    updateButtonStates();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBrowseFolder1()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder 1", 
                                                       ui->lineEditFolder1->text());
    if (!folder.isEmpty()) {
        ui->lineEditFolder1->setText(folder);
        folder1Path = folder;
    }
}

void MainWindow::onBrowseFolder2()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder 2", 
                                                       ui->lineEditFolder2->text());
    if (!folder.isEmpty()) {
        ui->lineEditFolder2->setText(folder);
        folder2Path = folder;
    }
}

int MainWindow::countFilesRecursive(const QString &folderPath)
{
    int count = 0;
    QDir dir(folderPath);
    if (!dir.exists()) {
        return 0;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, 
                                              QDir::Name | QDir::DirsFirst);
    
    int processed = 0;
    foreach (const QFileInfo &info, entries) {
        count++;
        processed++;
        
        if (processed % 20 == 0) {
            ui->statusBar->showMessage(QString("Counting files... %1 found so far").arg(count));
            QApplication::processEvents();
        }
        
        if (info.isDir()) {
            count += countFilesRecursive(info.absoluteFilePath());
        }
    }
    return count;
}

void MainWindow::scanFolder(const QString &folderPath, QMap<QString, FileInfo> &fileMap, const QString &basePath, int *fileCount)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        return;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, 
                                              QDir::Name | QDir::DirsFirst);
    
    int processed = 0;
    foreach (const QFileInfo &info, entries) {
        QString relativePath = basePath.isEmpty() ? info.fileName() : basePath + "/" + info.fileName();
        
        FileInfo fileInfo;
        fileInfo.path = relativePath;
        fileInfo.modifiedDate = info.lastModified();
        fileInfo.size = info.size();
        fileInfo.isDir = info.isDir();
        
        fileMap[relativePath] = fileInfo;
        
        if (info.isDir()) {
            scanFolder(info.absoluteFilePath(), fileMap, relativePath, fileCount);
        }
        
        if (fileCount) {
            (*fileCount)++;
            processed++;
            
            if (processed % 10 == 0) {
                int maxValue = ui->progressBar->maximum();
                if (maxValue > 0) {
                    int progressValue = qMin(*fileCount, maxValue);
                    ui->progressBar->setValue(progressValue);
                    int percent = (maxValue > 0) ? (progressValue * 100 / maxValue) : 0;
                    ui->statusBar->showMessage(QString("Scanning... %1/%2 files (%3%)").arg(progressValue).arg(maxValue).arg(percent));
                } else {
                    ui->statusBar->showMessage(QString("Scanning... %1 files processed").arg(*fileCount));
                }
                QApplication::processEvents();
            }
        }
    }
}

void MainWindow::onCompare()
{
    folder1Path = ui->lineEditFolder1->text();
    folder2Path = ui->lineEditFolder2->text();
    
    if (folder1Path.isEmpty() || folder2Path.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select both folders to compare.");
        return;
    }
    
    QDir dir1(folder1Path);
    QDir dir2(folder2Path);
    
    if (!dir1.exists()) {
        QMessageBox::warning(this, "Warning", "Folder 1 does not exist.");
        return;
    }
    
    if (!dir2.exists()) {
        QMessageBox::warning(this, "Warning", "Folder 2 does not exist.");
        return;
    }
    
    ui->pushButtonCompare->setEnabled(false);
    ui->treeWidgetFolder1->setEnabled(false);
    ui->treeWidgetFolder2->setEnabled(false);
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 0);
    ui->progressBar->setValue(0);
    ui->statusBar->showMessage("Counting files in Folder 1...");
    QApplication::processEvents();
    
    int folder1Count = countFilesRecursive(folder1Path);
    ui->statusBar->showMessage("Counting files in Folder 2...");
    QApplication::processEvents();
    
    int folder2Count = countFilesRecursive(folder2Path);
    int totalFiles = folder1Count + folder2Count;
    
    if (totalFiles > 0) {
        ui->progressBar->setRange(0, totalFiles);
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setRange(0, 0);
    }
    
    ui->statusBar->showMessage(QString("Scanning Folder 1... (0/%1)").arg(totalFiles));
    QApplication::processEvents();
    
    folder1Files.clear();
    folder2Files.clear();
    
    int fileCount = 0;
    scanFolder(folder1Path, folder1Files, "", &fileCount);
    
    ui->statusBar->showMessage(QString("Scanning Folder 2... (%1/%2)").arg(fileCount).arg(totalFiles));
    QApplication::processEvents();
    
    scanFolder(folder2Path, folder2Files, "", &fileCount);
    
    ui->statusBar->showMessage("Comparing files...");
    ui->progressBar->setRange(0, 0);
    QApplication::processEvents();
    
    ui->treeWidgetFolder1->clear();
    ui->treeWidgetFolder2->clear();
    
    compareFolders();
    
    ui->progressBar->setRange(0, 0);
    ui->progressBar->setVisible(false);
    ui->pushButtonCompare->setEnabled(true);
    ui->treeWidgetFolder1->setEnabled(true);
    ui->treeWidgetFolder2->setEnabled(true);
    
    int folder1ItemCount = ui->treeWidgetFolder1->topLevelItemCount();
    int folder2ItemCount = ui->treeWidgetFolder2->topLevelItemCount();
    ui->statusBar->showMessage(QString("Comparison complete. Folder 1: %1 items | Folder 2: %2 items")
                               .arg(folder1ItemCount).arg(folder2ItemCount));
}

void MainWindow::compareFolders()
{
    QSet<QString> allPaths;
    
    foreach (const QString &path, folder1Files.keys()) {
        allPaths.insert(path);
    }
    foreach (const QString &path, folder2Files.keys()) {
        allPaths.insert(path);
    }
    
    int totalPaths = allPaths.size();
    int processed = 0;
    
    foreach (const QString &relativePath, allPaths) {
        processed++;
        
        if (processed % 100 == 0) {
            int percent = (totalPaths > 0) ? (processed * 100 / totalPaths) : 0;
            ui->statusBar->showMessage(QString("Comparing files... %1/%2 (%3%)").arg(processed).arg(totalPaths).arg(percent));
            QApplication::processEvents();
        }
        
        bool inFolder1 = folder1Files.contains(relativePath);
        bool inFolder2 = folder2Files.contains(relativePath);
        
        FileInfo info1, info2;
        if (inFolder1) {
            info1 = folder1Files[relativePath];
        }
        if (inFolder2) {
            info2 = folder2Files[relativePath];
        }
        
        QString status;
        if (!inFolder1 && inFolder2) {
            status = "New in Folder 2";
        } else if (inFolder1 && !inFolder2) {
            status = "New in Folder 1";
        } else if (inFolder1 && inFolder2) {
            if (info1.isDir != info2.isDir) {
                status = "Type Mismatch";
            } else if (info1.isDir) {
                status = "Same";
            } else {
                if (info1.modifiedDate != info2.modifiedDate) {
                    if (info1.modifiedDate > info2.modifiedDate) {
                        status = "Folder 1 Newer";
                    } else {
                        status = "Folder 2 Newer";
                    }
                } else if (info1.size != info2.size) {
                    status = "Size Different";
                } else {
                    status = "Same";
                }
            }
        }
        
        addTreeItem(status, relativePath, info1, info2);
    }
    
    ui->statusBar->showMessage("Expanding tree views...");
    QApplication::processEvents();
    
    ui->treeWidgetFolder1->expandAll();
    ui->treeWidgetFolder2->expandAll();
}

void MainWindow::addTreeItem(const QString &status, const QString &relativePath, 
                             const FileInfo &info1, const FileInfo &info2)
{
    QColor color;
    QString displayStatus1;
    QString displayStatus2;
    
    if (status == "New in Folder 1" || status == "Folder 1 Newer") {
        color = QColor(0, 150, 0);
    } else if (status == "New in Folder 2" || status == "Folder 2 Newer") {
        color = QColor(150, 0, 0);
    } else if (status == "Same") {
        color = QColor(100, 100, 100);
    } else {
        color = QColor(200, 150, 0);
    }
    
    // Map to git-like short status codes per side
    if (status == "New in Folder 1") {
        displayStatus1 = "A";   // added in Folder 1
        displayStatus2 = "D";   // deleted from Folder 2
    } else if (status == "New in Folder 2") {
        displayStatus1 = "D";   // deleted from Folder 1
        displayStatus2 = "A";   // added in Folder 2
    } else if (status == "Folder 1 Newer") {
        displayStatus1 = "M";   // modified (newer) in Folder 1
        displayStatus2 = "Old"; // older version
    } else if (status == "Folder 2 Newer") {
        displayStatus1 = "Old"; // older version
        displayStatus2 = "M";   // modified (newer) in Folder 2
    } else if (status == "Size Different" || status == "Type Mismatch") {
        displayStatus1 = "M";
        displayStatus2 = "M";
    } else if (status == "Same") {
        displayStatus1 = "";
        displayStatus2 = "";
    } else {
        displayStatus1 = status;
        displayStatus2 = status;
    }
    
    // Left list (Folder 1) – show even when only exists in Folder 2 (as deleted)
    if (info1.modifiedDate.isValid() || status == "New in Folder 1" || status == "New in Folder 2") {
        QTreeWidgetItem *item1 = new QTreeWidgetItem(ui->treeWidgetFolder1);
        item1->setText(0, displayStatus1);
        item1->setText(1, relativePath);
        
        if (info1.modifiedDate.isValid()) {
            item1->setText(2, info1.modifiedDate.toString("yyyy-MM-dd hh:mm:ss"));
        } else {
            item1->setText(2, "N/A");
        }
        
        if (info1.size > 0) {
            QString sizeStr;
            if (info1.size < 1024) {
                sizeStr = QString::number(info1.size) + " B";
            } else if (info1.size < 1024 * 1024) {
                sizeStr = QString::number(info1.size / 1024.0, 'f', 2) + " KB";
            } else {
                sizeStr = QString::number(info1.size / (1024.0 * 1024.0), 'f', 2) + " MB";
            }
            item1->setText(3, sizeStr);
        } else {
            item1->setText(3, "-");
        }
        
        item1->setForeground(0, QBrush(color));
        item1->setData(0, Qt::UserRole, relativePath);
        item1->setData(1, Qt::UserRole, 1);
    }
    
    // Right list (Folder 2) – show even when only exists in Folder 1 (as deleted)
    if (info2.modifiedDate.isValid() || status == "New in Folder 1" || status == "New in Folder 2") {
        QTreeWidgetItem *item2 = new QTreeWidgetItem(ui->treeWidgetFolder2);
        item2->setText(0, displayStatus2);
        item2->setText(1, relativePath);
        
        if (info2.modifiedDate.isValid()) {
            item2->setText(2, info2.modifiedDate.toString("yyyy-MM-dd hh:mm:ss"));
        } else {
            item2->setText(2, "N/A");
        }
        
        if (info2.size > 0) {
            QString sizeStr;
            if (info2.size < 1024) {
                sizeStr = QString::number(info2.size) + " B";
            } else if (info2.size < 1024 * 1024) {
                sizeStr = QString::number(info2.size / 1024.0, 'f', 2) + " KB";
            } else {
                sizeStr = QString::number(info2.size / (1024.0 * 1024.0), 'f', 2) + " MB";
            }
            item2->setText(3, sizeStr);
        } else {
            item2->setText(3, "-");
        }
        
        item2->setForeground(0, QBrush(color));
        item2->setData(0, Qt::UserRole, relativePath);
        item2->setData(1, Qt::UserRole, 2);
    }
}

void MainWindow::onItemSelectionChanged()
{
    updateButtonStates();
}

void MainWindow::updateButtonStates()
{
    bool hasSelection1 = !ui->treeWidgetFolder1->selectedItems().isEmpty();
    bool hasSelection2 = !ui->treeWidgetFolder2->selectedItems().isEmpty();
    
    ui->pushButtonCopyTo2->setEnabled(hasSelection1);
    ui->pushButtonCopyTo1->setEnabled(hasSelection2);
    ui->pushButtonMoveTo2->setEnabled(hasSelection1);
    ui->pushButtonMoveTo1->setEnabled(hasSelection2);
}

void MainWindow::onFolder1ScrollChanged(int value)
{
    if (!syncingScroll) {
        syncingScroll = true;
        ui->treeWidgetFolder2->verticalScrollBar()->setValue(value);
        syncingScroll = false;
    }
}

void MainWindow::onFolder2ScrollChanged(int value)
{
    if (!syncingScroll) {
        syncingScroll = true;
        ui->treeWidgetFolder1->verticalScrollBar()->setValue(value);
        syncingScroll = false;
    }
}

QString MainWindow::getFullPath(const QString &relativePath, int folderNumber)
{
    QString basePath = (folderNumber == 1) ? folder1Path : folder2Path;
    return QDir(basePath).filePath(relativePath);
}

bool MainWindow::copyFileOrFolder(const QString &source, const QString &destination)
{
    QFileInfo sourceInfo(source);
    QFileInfo destInfo(destination);
    
    if (!sourceInfo.exists()) {
        return false;
    }
    
    if (sourceInfo.isDir()) {
        QDir destDir;
        if (!destDir.exists(destination)) {
            if (!destDir.mkpath(destination)) {
                return false;
            }
        }
        
        QDir sourceDir(source);
        QFileInfoList entries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        
        foreach (const QFileInfo &entry, entries) {
            QString destPath = QDir(destination).filePath(entry.fileName());
            if (!copyFileOrFolder(entry.absoluteFilePath(), destPath)) {
                return false;
            }
        }
        return true;
    } else {
        if (destInfo.exists()) {
            QFile::remove(destination);
        }
        
        QDir destDir = destInfo.absoluteDir();
        if (!destDir.exists()) {
            destDir.mkpath(".");
        }
        
        return QFile::copy(source, destination);
    }
}

bool MainWindow::removeDirectoryRecursive(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        return true;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    foreach (const QFileInfo &entry, entries) {
        if (entry.isDir()) {
            if (!removeDirectoryRecursive(entry.absoluteFilePath())) {
                return false;
            }
        } else {
            if (!QFile::remove(entry.absoluteFilePath())) {
                return false;
            }
        }
    }
    return dir.rmdir(dirPath);
}

bool MainWindow::moveFileOrFolder(const QString &source, const QString &destination)
{
    if (copyFileOrFolder(source, destination)) {
        QFileInfo sourceInfo(source);
        if (sourceInfo.isDir()) {
            return removeDirectoryRecursive(source);
        } else {
            return QFile::remove(source);
        }
    }
    return false;
}

int MainWindow::countFilesForOperation(const QString &path)
{
    QFileInfo info(path);
    if (!info.exists()) {
        return 0;
    }
    
    if (info.isDir()) {
        int count = 1;
        QDir dir(path);
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        foreach (const QFileInfo &entry, entries) {
            count += countFilesForOperation(entry.absoluteFilePath());
        }
        return count;
    } else {
        return 1;
    }
}

bool MainWindow::copyFileOrFolderWithProgress(const QString &source, const QString &destination, int *processed, int *total)
{
    QFileInfo sourceInfo(source);
    QFileInfo destInfo(destination);
    
    if (!sourceInfo.exists()) {
        return false;
    }
    
    if (sourceInfo.isDir()) {
        QDir destDir;
        if (!destDir.exists(destination)) {
            if (!destDir.mkpath(destination)) {
                return false;
            }
        }
        
        if (processed && total) {
            (*processed)++;
            if (*processed % 10 == 0) {
                int percent = (*total > 0) ? (*processed * 100 / *total) : 0;
                ui->progressBar->setValue(*processed);
                ui->statusBar->showMessage(QString("Copying... %1/%2 (%3%)").arg(*processed).arg(*total).arg(percent));
                QApplication::processEvents();
            }
        }
        
        QDir sourceDir(source);
        QFileInfoList entries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
        
        foreach (const QFileInfo &entry, entries) {
            QString destPath = QDir(destination).filePath(entry.fileName());
            if (!copyFileOrFolderWithProgress(entry.absoluteFilePath(), destPath, processed, total)) {
                return false;
            }
        }
        return true;
    } else {
        if (destInfo.exists()) {
            QFile::remove(destination);
        }
        
        QDir destDir = destInfo.absoluteDir();
        if (!destDir.exists()) {
            destDir.mkpath(".");
        }
        
        bool result = QFile::copy(source, destination);
        
        if (processed && total && result) {
            (*processed)++;
            if (*processed % 10 == 0 || *processed == *total) {
                int percent = (*total > 0) ? (*processed * 100 / *total) : 0;
                ui->progressBar->setValue(*processed);
                ui->statusBar->showMessage(QString("Copying... %1/%2 (%3%)").arg(*processed).arg(*total).arg(percent));
                QApplication::processEvents();
            }
        }
        
        return result;
    }
}

bool MainWindow::moveFileOrFolderWithProgress(const QString &source, const QString &destination, int *processed, int *total)
{
    if (copyFileOrFolderWithProgress(source, destination, processed, total)) {
        QFileInfo sourceInfo(source);
        if (sourceInfo.isDir()) {
            return removeDirectoryRecursive(source);
        } else {
            return QFile::remove(source);
        }
    }
    return false;
}

void MainWindow::onCopyToFolder2()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidgetFolder1->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    ui->pushButtonCopyTo2->setEnabled(false);
    ui->pushButtonCopyTo1->setEnabled(false);
    ui->pushButtonMoveTo2->setEnabled(false);
    ui->pushButtonMoveTo1->setEnabled(false);
    ui->treeWidgetFolder1->setEnabled(false);
    ui->treeWidgetFolder2->setEnabled(false);
    
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 0);
    ui->statusBar->showMessage("Counting files to copy...");
    QApplication::processEvents();
    
    int totalFiles = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        totalFiles += countFilesForOperation(sourcePath);
    }
    
    if (totalFiles > 0) {
        ui->progressBar->setRange(0, totalFiles);
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setRange(0, 0);
    }
    
    int processed = 0;
    int successCount = 0;
    QStringList failedItems;
    
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        QString destPath = getFullPath(relativePath, 2);
        
        ui->statusBar->showMessage(QString("Copying: %1").arg(relativePath));
        QApplication::processEvents();
        
        if (copyFileOrFolderWithProgress(sourcePath, destPath, &processed, &totalFiles)) {
            successCount++;
        } else {
            failedItems.append(relativePath);
        }
    }
    
    ui->progressBar->setVisible(false);
    ui->pushButtonCopyTo2->setEnabled(true);
    ui->pushButtonCopyTo1->setEnabled(true);
    ui->pushButtonMoveTo2->setEnabled(true);
    ui->pushButtonMoveTo1->setEnabled(true);
    ui->treeWidgetFolder1->setEnabled(true);
    ui->treeWidgetFolder2->setEnabled(true);
    
    if (successCount > 0) {
        if (failedItems.isEmpty()) {
            QMessageBox::information(this, "Success", 
                QString("Successfully copied %1 item(s) to Folder 2.").arg(successCount));
        } else {
            QMessageBox::warning(this, "Partial Success", 
                QString("Copied %1 item(s) successfully.\nFailed: %2").arg(successCount).arg(failedItems.join(", ")));
        }
        onCompare();
    } else if (!failedItems.isEmpty()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to copy all items:\n%1").arg(failedItems.join("\n")));
    }
}

void MainWindow::onCopyToFolder1()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidgetFolder2->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    ui->pushButtonCopyTo2->setEnabled(false);
    ui->pushButtonCopyTo1->setEnabled(false);
    ui->pushButtonMoveTo2->setEnabled(false);
    ui->pushButtonMoveTo1->setEnabled(false);
    ui->treeWidgetFolder1->setEnabled(false);
    ui->treeWidgetFolder2->setEnabled(false);
    
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 0);
    ui->statusBar->showMessage("Counting files to copy...");
    QApplication::processEvents();
    
    int totalFiles = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        totalFiles += countFilesForOperation(sourcePath);
    }
    
    if (totalFiles > 0) {
        ui->progressBar->setRange(0, totalFiles);
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setRange(0, 0);
    }
    
    int processed = 0;
    int successCount = 0;
    QStringList failedItems;
    
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        QString destPath = getFullPath(relativePath, 1);
        
        ui->statusBar->showMessage(QString("Copying: %1").arg(relativePath));
        QApplication::processEvents();
        
        if (copyFileOrFolderWithProgress(sourcePath, destPath, &processed, &totalFiles)) {
            successCount++;
        } else {
            failedItems.append(relativePath);
        }
    }
    
    ui->progressBar->setVisible(false);
    ui->pushButtonCopyTo2->setEnabled(true);
    ui->pushButtonCopyTo1->setEnabled(true);
    ui->pushButtonMoveTo2->setEnabled(true);
    ui->pushButtonMoveTo1->setEnabled(true);
    ui->treeWidgetFolder1->setEnabled(true);
    ui->treeWidgetFolder2->setEnabled(true);
    
    if (successCount > 0) {
        if (failedItems.isEmpty()) {
            QMessageBox::information(this, "Success", 
                QString("Successfully copied %1 item(s) to Folder 1.").arg(successCount));
        } else {
            QMessageBox::warning(this, "Partial Success", 
                QString("Copied %1 item(s) successfully.\nFailed: %2").arg(successCount).arg(failedItems.join(", ")));
        }
        onCompare();
    } else if (!failedItems.isEmpty()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to copy all items:\n%1").arg(failedItems.join("\n")));
    }
}

void MainWindow::onMoveToFolder2()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidgetFolder1->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Move", 
        "Are you sure you want to move the selected items? This will delete them from the source folder.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    ui->pushButtonCopyTo2->setEnabled(false);
    ui->pushButtonCopyTo1->setEnabled(false);
    ui->pushButtonMoveTo2->setEnabled(false);
    ui->pushButtonMoveTo1->setEnabled(false);
    ui->treeWidgetFolder1->setEnabled(false);
    ui->treeWidgetFolder2->setEnabled(false);
    
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 0);
    ui->statusBar->showMessage("Counting files to move...");
    QApplication::processEvents();
    
    int totalFiles = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        totalFiles += countFilesForOperation(sourcePath);
    }
    
    if (totalFiles > 0) {
        ui->progressBar->setRange(0, totalFiles);
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setRange(0, 0);
    }
    
    int processed = 0;
    int successCount = 0;
    QStringList failedItems;
    
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        QString destPath = getFullPath(relativePath, 2);
        
        ui->statusBar->showMessage(QString("Moving: %1").arg(relativePath));
        QApplication::processEvents();
        
        if (moveFileOrFolderWithProgress(sourcePath, destPath, &processed, &totalFiles)) {
            successCount++;
        } else {
            failedItems.append(relativePath);
        }
    }
    
    ui->progressBar->setVisible(false);
    ui->pushButtonCopyTo2->setEnabled(true);
    ui->pushButtonCopyTo1->setEnabled(true);
    ui->pushButtonMoveTo2->setEnabled(true);
    ui->pushButtonMoveTo1->setEnabled(true);
    ui->treeWidgetFolder1->setEnabled(true);
    ui->treeWidgetFolder2->setEnabled(true);
    
    if (successCount > 0) {
        if (failedItems.isEmpty()) {
            QMessageBox::information(this, "Success", 
                QString("Successfully moved %1 item(s) to Folder 2.").arg(successCount));
        } else {
            QMessageBox::warning(this, "Partial Success", 
                QString("Moved %1 item(s) successfully.\nFailed: %2").arg(successCount).arg(failedItems.join(", ")));
        }
        onCompare();
    } else if (!failedItems.isEmpty()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to move all items:\n%1").arg(failedItems.join("\n")));
    }
}

void MainWindow::onMoveToFolder1()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidgetFolder2->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Move", 
        "Are you sure you want to move the selected items? This will delete them from the source folder.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    ui->pushButtonCopyTo2->setEnabled(false);
    ui->pushButtonCopyTo1->setEnabled(false);
    ui->pushButtonMoveTo2->setEnabled(false);
    ui->pushButtonMoveTo1->setEnabled(false);
    ui->treeWidgetFolder1->setEnabled(false);
    ui->treeWidgetFolder2->setEnabled(false);
    
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, 0);
    ui->statusBar->showMessage("Counting files to move...");
    QApplication::processEvents();
    
    int totalFiles = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        totalFiles += countFilesForOperation(sourcePath);
    }
    
    if (totalFiles > 0) {
        ui->progressBar->setRange(0, totalFiles);
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setRange(0, 0);
    }
    
    int processed = 0;
    int successCount = 0;
    QStringList failedItems;
    
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        QString destPath = getFullPath(relativePath, 1);
        
        ui->statusBar->showMessage(QString("Moving: %1").arg(relativePath));
        QApplication::processEvents();
        
        if (moveFileOrFolderWithProgress(sourcePath, destPath, &processed, &totalFiles)) {
            successCount++;
        } else {
            failedItems.append(relativePath);
        }
    }
    
    ui->progressBar->setVisible(false);
    ui->pushButtonCopyTo2->setEnabled(true);
    ui->pushButtonCopyTo1->setEnabled(true);
    ui->pushButtonMoveTo2->setEnabled(true);
    ui->pushButtonMoveTo1->setEnabled(true);
    ui->treeWidgetFolder1->setEnabled(true);
    ui->treeWidgetFolder2->setEnabled(true);
    
    if (successCount > 0) {
        if (failedItems.isEmpty()) {
            QMessageBox::information(this, "Success", 
            QString("Successfully moved %1 item(s) to Folder 1.").arg(successCount));
        } else {
            QMessageBox::warning(this, "Partial Success", 
                QString("Moved %1 item(s) successfully.\nFailed: %2").arg(successCount).arg(failedItems.join(", ")));
        }
        onCompare();
    } else if (!failedItems.isEmpty()) {
        QMessageBox::warning(this, "Error", 
            QString("Failed to move all items:\n%1").arg(failedItems.join("\n")));
    }
}
