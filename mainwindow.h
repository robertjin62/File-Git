#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QDateTime>

namespace Ui {
    class MainWindow;
}

struct FileInfo {
    QString path;
    QDateTime modifiedDate;
    qint64 size;
    bool isDir;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onBrowseFolder1();
    void onBrowseFolder2();
    void onCompare();
    void onItemSelectionChanged();
    void onCopyToFolder2();
    void onCopyToFolder1();
    void onMoveToFolder2();
    void onMoveToFolder1();
    void onFolder1ScrollChanged(int value);
    void onFolder2ScrollChanged(int value);

private:
    void scanFolder(const QString &folderPath, QMap<QString, FileInfo> &fileMap, const QString &basePath = "", int *fileCount = 0);
    void compareFolders();
    int countFilesRecursive(const QString &folderPath);
    void addTreeItem(const QString &status, const QString &relativePath, 
                     const FileInfo &info1, const FileInfo &info2);
    bool copyFileOrFolder(const QString &source, const QString &destination);
    bool moveFileOrFolder(const QString &source, const QString &destination);
    bool removeDirectoryRecursive(const QString &dirPath);
    QString getFullPath(const QString &relativePath, int folderNumber);
    void updateButtonStates();

    Ui::MainWindow *ui;
    QMap<QString, FileInfo> folder1Files;
    QMap<QString, FileInfo> folder2Files;
    QString folder1Path;
    QString folder2Path;
    bool syncingScroll;
};

#endif // MAINWINDOW_H
