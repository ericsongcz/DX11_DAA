/********************************************************************************
** Form generated from reading UI file 'editor.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITOR_H
#define UI_EDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditorClass
{
public:
    QAction *actionLoad_Model;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *shit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *EditorClass)
    {
        if (EditorClass->objectName().isEmpty())
            EditorClass->setObjectName(QStringLiteral("EditorClass"));
        EditorClass->resize(600, 400);
        actionLoad_Model = new QAction(EditorClass);
        actionLoad_Model->setObjectName(QStringLiteral("actionLoad_Model"));
        centralWidget = new QWidget(EditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        EditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(EditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        shit = new QMenu(menuBar);
        shit->setObjectName(QStringLiteral("shit"));
        EditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        EditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(EditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        EditorClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(shit->menuAction());
        menuFile->addAction(actionLoad_Model);

        retranslateUi(EditorClass);

        QMetaObject::connectSlotsByName(EditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *EditorClass)
    {
        EditorClass->setWindowTitle(QApplication::translate("EditorClass", "Editor", 0));
        actionLoad_Model->setText(QApplication::translate("EditorClass", "Load Model", 0));
        menuFile->setTitle(QApplication::translate("EditorClass", "File", 0));
        shit->setTitle(QApplication::translate("EditorClass", "Shit", 0));
    } // retranslateUi

};

namespace Ui {
    class EditorClass: public Ui_EditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITOR_H
