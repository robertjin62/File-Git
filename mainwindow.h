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
    QDateTime createdDate;
    qint64 size;
    bool isDir;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool eventFilter(QObject *obj, QEvent *event);

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
    void onFolder1SelectionChanged();
    void onFolder2SelectionChanged();
    void onFolder1FocusChanged();
    void onFolder2FocusChanged();
    void onFilter1Changed();
    void onFilter2Changed();
    void onStatusFilter1Changed();
    void onStatusFilter2Changed();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void showContextMenu(const QPoint& pos);
    void onOpenInExplorer();
    void onShowProperties();
    void onCopyPath();
    void onCopyFileName();

private:
    void scanFolder(const QString &folderPath, QMap<QString, FileInfo> &fileMap, const QString &basePath = "", int *fileCount = 0);
    void compareFolders();
    int countFilesRecursive(const QString &folderPath);
    void addTreeItem(const QString &status, const QString &relativePath, 
                     const FileInfo &info1, const FileInfo &info2);
    bool copyFileOrFolder(const QString &source, const QString &destination);
    bool moveFileOrFolder(const QString &source, const QString &destination);
    bool removeDirectoryRecursive(const QString &dirPath);
    int countFilesForOperation(const QString &path);
    bool copyFileOrFolderWithProgress(const QString &source, const QString &destination, int *processed, int *total);
    bool moveFileOrFolderWithProgress(const QString &source, const QString &destination, int *processed, int *total);
    QString getFullPath(const QString &relativePath, int folderNumber);
    void updateButtonStates();
    void applyStyles();

    Ui::MainWindow *ui;
    QMap<QString, FileInfo> folder1Files;
    QMap<QString, FileInfo> folder2Files;
    QString folder1Path;
    QString folder2Path;
    bool syncingScroll;
    bool syncingSelection;
    QTreeWidget* activeTreeWidget;
    void syncSelection(QTreeWidget* source, QTreeWidget* target);
    void applyFilter(QTreeWidget* treeWidget, const QString& textFilter, const QString& statusFilter);
    void setupFilterComboBoxes();
    void setupContextMenus();
    void openInExplorer(const QString& filePath, bool selectItem = true);
    void showPropertiesDialog(const QString& relativePath, const QFileInfo& fileInfo, 
                              const FileInfo& info1, const FileInfo& info2, 
                              bool inFolder1, bool inFolder2, const QString& status);
    QTreeWidgetItem* contextMenuItem;
    QTreeWidget* contextMenuTree;
};

#endif // MAINWINDOW_H
