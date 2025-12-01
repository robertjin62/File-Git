/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Dec 1 10:41:03 2025
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_1;
    QLabel *labelFolder1;
    QLineEdit *lineEditFolder1;
    QPushButton *pushButtonBrowse1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelFolder2;
    QLineEdit *lineEditFolder2;
    QPushButton *pushButtonBrowse2;
    QPushButton *pushButtonCompare;
    QSplitter *splitter;
    QWidget *widgetFolder1;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelFolder1Title;
    QTreeWidget *treeWidgetFolder1;
    QWidget *widgetFolder2;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelFolder2Title;
    QTreeWidget *treeWidgetFolder2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonCopyTo2;
    QPushButton *pushButtonCopyTo1;
    QPushButton *pushButtonMoveTo2;
    QPushButton *pushButtonMoveTo1;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1400, 800);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_1 = new QHBoxLayout();
        horizontalLayout_1->setSpacing(6);
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        labelFolder1 = new QLabel(centralWidget);
        labelFolder1->setObjectName(QString::fromUtf8("labelFolder1"));

        horizontalLayout_1->addWidget(labelFolder1);

        lineEditFolder1 = new QLineEdit(centralWidget);
        lineEditFolder1->setObjectName(QString::fromUtf8("lineEditFolder1"));

        horizontalLayout_1->addWidget(lineEditFolder1);

        pushButtonBrowse1 = new QPushButton(centralWidget);
        pushButtonBrowse1->setObjectName(QString::fromUtf8("pushButtonBrowse1"));

        horizontalLayout_1->addWidget(pushButtonBrowse1);


        verticalLayout->addLayout(horizontalLayout_1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelFolder2 = new QLabel(centralWidget);
        labelFolder2->setObjectName(QString::fromUtf8("labelFolder2"));

        horizontalLayout_2->addWidget(labelFolder2);

        lineEditFolder2 = new QLineEdit(centralWidget);
        lineEditFolder2->setObjectName(QString::fromUtf8("lineEditFolder2"));

        horizontalLayout_2->addWidget(lineEditFolder2);

        pushButtonBrowse2 = new QPushButton(centralWidget);
        pushButtonBrowse2->setObjectName(QString::fromUtf8("pushButtonBrowse2"));

        horizontalLayout_2->addWidget(pushButtonBrowse2);


        verticalLayout->addLayout(horizontalLayout_2);

        pushButtonCompare = new QPushButton(centralWidget);
        pushButtonCompare->setObjectName(QString::fromUtf8("pushButtonCompare"));
        pushButtonCompare->setMinimumSize(QSize(0, 30));

        verticalLayout->addWidget(pushButtonCompare);

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        widgetFolder1 = new QWidget(splitter);
        widgetFolder1->setObjectName(QString::fromUtf8("widgetFolder1"));
        verticalLayout_2 = new QVBoxLayout(widgetFolder1);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelFolder1Title = new QLabel(widgetFolder1);
        labelFolder1Title->setObjectName(QString::fromUtf8("labelFolder1Title"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        labelFolder1Title->setFont(font);

        verticalLayout_2->addWidget(labelFolder1Title);

        treeWidgetFolder1 = new QTreeWidget(widgetFolder1);
        treeWidgetFolder1->setObjectName(QString::fromUtf8("treeWidgetFolder1"));

        verticalLayout_2->addWidget(treeWidgetFolder1);

        splitter->addWidget(widgetFolder1);
        widgetFolder2 = new QWidget(splitter);
        widgetFolder2->setObjectName(QString::fromUtf8("widgetFolder2"));
        verticalLayout_3 = new QVBoxLayout(widgetFolder2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        labelFolder2Title = new QLabel(widgetFolder2);
        labelFolder2Title->setObjectName(QString::fromUtf8("labelFolder2Title"));
        labelFolder2Title->setFont(font);

        verticalLayout_3->addWidget(labelFolder2Title);

        treeWidgetFolder2 = new QTreeWidget(widgetFolder2);
        treeWidgetFolder2->setObjectName(QString::fromUtf8("treeWidgetFolder2"));

        verticalLayout_3->addWidget(treeWidgetFolder2);

        splitter->addWidget(widgetFolder2);

        verticalLayout->addWidget(splitter);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButtonCopyTo2 = new QPushButton(centralWidget);
        pushButtonCopyTo2->setObjectName(QString::fromUtf8("pushButtonCopyTo2"));
        pushButtonCopyTo2->setEnabled(false);

        horizontalLayout_3->addWidget(pushButtonCopyTo2);

        pushButtonCopyTo1 = new QPushButton(centralWidget);
        pushButtonCopyTo1->setObjectName(QString::fromUtf8("pushButtonCopyTo1"));
        pushButtonCopyTo1->setEnabled(false);

        horizontalLayout_3->addWidget(pushButtonCopyTo1);

        pushButtonMoveTo2 = new QPushButton(centralWidget);
        pushButtonMoveTo2->setObjectName(QString::fromUtf8("pushButtonMoveTo2"));
        pushButtonMoveTo2->setEnabled(false);

        horizontalLayout_3->addWidget(pushButtonMoveTo2);

        pushButtonMoveTo1 = new QPushButton(centralWidget);
        pushButtonMoveTo1->setObjectName(QString::fromUtf8("pushButtonMoveTo1"));
        pushButtonMoveTo1->setEnabled(false);

        horizontalLayout_3->addWidget(pushButtonMoveTo1);


        verticalLayout->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1400, 21));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Folder Comparison Tool", 0, QApplication::UnicodeUTF8));
        labelFolder1->setText(QApplication::translate("MainWindow", "Folder 1:", 0, QApplication::UnicodeUTF8));
        pushButtonBrowse1->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        labelFolder2->setText(QApplication::translate("MainWindow", "Folder 2:", 0, QApplication::UnicodeUTF8));
        pushButtonBrowse2->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        pushButtonCompare->setText(QApplication::translate("MainWindow", "Compare Folders", 0, QApplication::UnicodeUTF8));
        labelFolder1Title->setText(QApplication::translate("MainWindow", "Folder 1 Contents", 0, QApplication::UnicodeUTF8));
        labelFolder2Title->setText(QApplication::translate("MainWindow", "Folder 2 Contents", 0, QApplication::UnicodeUTF8));
        pushButtonCopyTo2->setText(QApplication::translate("MainWindow", "Copy Selected to Folder 2", 0, QApplication::UnicodeUTF8));
        pushButtonCopyTo1->setText(QApplication::translate("MainWindow", "Copy Selected to Folder 1", 0, QApplication::UnicodeUTF8));
        pushButtonMoveTo2->setText(QApplication::translate("MainWindow", "Move Selected to Folder 2", 0, QApplication::UnicodeUTF8));
        pushButtonMoveTo1->setText(QApplication::translate("MainWindow", "Move Selected to Folder 1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
