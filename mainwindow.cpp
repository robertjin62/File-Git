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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    connect(ui->pushButtonBrowse1, SIGNAL(clicked()), this, SLOT(onBrowseFolder1()));
    connect(ui->pushButtonBrowse2, SIGNAL(clicked()), this, SLOT(onBrowseFolder2()));
    connect(ui->pushButtonCompare, SIGNAL(clicked()), this, SLOT(onCompare()));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(ui->pushButtonCopyTo2, SIGNAL(clicked()), this, SLOT(onCopyToFolder2()));
    connect(ui->pushButtonCopyTo1, SIGNAL(clicked()), this, SLOT(onCopyToFolder1()));
    connect(ui->pushButtonMoveTo2, SIGNAL(clicked()), this, SLOT(onMoveToFolder2()));
    connect(ui->pushButtonMoveTo1, SIGNAL(clicked()), this, SLOT(onMoveToFolder1()));
    
    QStringList headers;
    headers << "Status" << "File/Folder" << "Folder 1 Date" << "Folder 2 Date" << "Size";
    ui->treeWidget->setHeaderLabels(headers);
    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->setColumnWidth(0, 100);
    ui->treeWidget->setColumnWidth(1, 300);
    ui->treeWidget->setColumnWidth(2, 150);
    ui->treeWidget->setColumnWidth(3, 150);
    ui->treeWidget->setColumnWidth(4, 100);
    
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

void MainWindow::scanFolder(const QString &folderPath, QMap<QString, FileInfo> &fileMap, const QString &basePath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        return;
    }
    
    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, 
                                              QDir::Name | QDir::DirsFirst);
    
    foreach (const QFileInfo &info, entries) {
        QString relativePath = basePath.isEmpty() ? info.fileName() : basePath + "/" + info.fileName();
        
        FileInfo fileInfo;
        fileInfo.path = relativePath;
        fileInfo.modifiedDate = info.lastModified();
        fileInfo.size = info.size();
        fileInfo.isDir = info.isDir();
        
        fileMap[relativePath] = fileInfo;
        
        if (info.isDir()) {
            scanFolder(info.absoluteFilePath(), fileMap, relativePath);
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
    
    ui->statusBar->showMessage("Scanning folders...");
    QApplication::processEvents();
    
    folder1Files.clear();
    folder2Files.clear();
    
    scanFolder(folder1Path, folder1Files);
    scanFolder(folder2Path, folder2Files);
    
    ui->treeWidget->clear();
    compareFolders();
    
    ui->statusBar->showMessage(QString("Comparison complete. Found %1 items.").arg(ui->treeWidget->topLevelItemCount()));
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
    
    foreach (const QString &relativePath, allPaths) {
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
    
    ui->treeWidget->expandAll();
}

void MainWindow::addTreeItem(const QString &status, const QString &relativePath, 
                             const FileInfo &info1, const FileInfo &info2)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, status);
    item->setText(1, relativePath);
    
    if (info1.modifiedDate.isValid()) {
        item->setText(2, info1.modifiedDate.toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        item->setText(2, "N/A");
    }
    
    if (info2.modifiedDate.isValid()) {
        item->setText(3, info2.modifiedDate.toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        item->setText(3, "N/A");
    }
    
    if (info1.size > 0 || info2.size > 0) {
        qint64 size = info1.size > info2.size ? info1.size : info2.size;
        if (size < 1024) {
            item->setText(4, QString::number(size) + " B");
        } else if (size < 1024 * 1024) {
            item->setText(4, QString::number(size / 1024.0, 'f', 2) + " KB");
        } else {
            item->setText(4, QString::number(size / (1024.0 * 1024.0), 'f', 2) + " MB");
        }
    } else {
        item->setText(4, "-");
    }
    
    if (status == "New in Folder 1" || status == "Folder 1 Newer") {
        item->setForeground(0, QBrush(QColor(0, 150, 0)));
    } else if (status == "New in Folder 2" || status == "Folder 2 Newer") {
        item->setForeground(0, QBrush(QColor(150, 0, 0)));
    } else if (status == "Same") {
        item->setForeground(0, QBrush(QColor(100, 100, 100)));
    } else {
        item->setForeground(0, QBrush(QColor(200, 150, 0)));
    }
    
    item->setData(0, Qt::UserRole, relativePath);
}

void MainWindow::onItemSelectionChanged()
{
    updateButtonStates();
}

void MainWindow::updateButtonStates()
{
    bool hasSelection = !ui->treeWidget->selectedItems().isEmpty();
    ui->pushButtonCopyTo2->setEnabled(hasSelection);
    ui->pushButtonCopyTo1->setEnabled(hasSelection);
    ui->pushButtonMoveTo2->setEnabled(hasSelection);
    ui->pushButtonMoveTo1->setEnabled(hasSelection);
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

void MainWindow::onCopyToFolder2()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidget->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int count = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        QString destPath = getFullPath(relativePath, 2);
        
        if (copyFileOrFolder(sourcePath, destPath)) {
            count++;
        } else {
            QMessageBox::warning(this, "Error", 
                QString("Failed to copy: %1").arg(relativePath));
        }
    }
    
    if (count > 0) {
        QMessageBox::information(this, "Success", 
            QString("Successfully copied %1 item(s) to Folder 2.").arg(count));
        onCompare();
    }
}

void MainWindow::onCopyToFolder1()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidget->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int count = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        QString destPath = getFullPath(relativePath, 1);
        
        if (copyFileOrFolder(sourcePath, destPath)) {
            count++;
        } else {
            QMessageBox::warning(this, "Error", 
                QString("Failed to copy: %1").arg(relativePath));
        }
    }
    
    if (count > 0) {
        QMessageBox::information(this, "Success", 
            QString("Successfully copied %1 item(s) to Folder 1.").arg(count));
        onCompare();
    }
}

void MainWindow::onMoveToFolder2()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidget->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Move", 
        "Are you sure you want to move the selected items? This will delete them from the source folder.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    int count = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 1);
        QString destPath = getFullPath(relativePath, 2);
        
        if (moveFileOrFolder(sourcePath, destPath)) {
            count++;
        } else {
            QMessageBox::warning(this, "Error", 
                QString("Failed to move: %1").arg(relativePath));
        }
    }
    
    if (count > 0) {
        QMessageBox::information(this, "Success", 
            QString("Successfully moved %1 item(s) to Folder 2.").arg(count));
        onCompare();
    }
}

void MainWindow::onMoveToFolder1()
{
    QList<QTreeWidgetItem*> selected = ui->treeWidget->selectedItems();
    if (selected.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Move", 
        "Are you sure you want to move the selected items? This will delete them from the source folder.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    int count = 0;
    foreach (QTreeWidgetItem *item, selected) {
        QString relativePath = item->data(0, Qt::UserRole).toString();
        QString sourcePath = getFullPath(relativePath, 2);
        QString destPath = getFullPath(relativePath, 1);
        
        if (moveFileOrFolder(sourcePath, destPath)) {
            count++;
        } else {
            QMessageBox::warning(this, "Error", 
                QString("Failed to move: %1").arg(relativePath));
        }
    }
    
    if (count > 0) {
        QMessageBox::information(this, "Success", 
            QString("Successfully moved %1 item(s) to Folder 1.").arg(count));
        onCompare();
    }
}
