/********************************************************************************
** Form generated from reading UI file 'd3drenderingwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_D3DRENDERINGWIDGET_H
#define UI_D3DRENDERINGWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_D3DRenderingWidget
{
public:

    void setupUi(QWidget *D3DRenderingWidget)
    {
        if (D3DRenderingWidget->objectName().isEmpty())
            D3DRenderingWidget->setObjectName(QStringLiteral("D3DRenderingWidget"));
        D3DRenderingWidget->resize(400, 300);

        retranslateUi(D3DRenderingWidget);

        QMetaObject::connectSlotsByName(D3DRenderingWidget);
    } // setupUi

    void retranslateUi(QWidget *D3DRenderingWidget)
    {
        D3DRenderingWidget->setWindowTitle(QApplication::translate("D3DRenderingWidget", "D3DRenderingWidget", 0));
    } // retranslateUi

};

namespace Ui {
    class D3DRenderingWidget: public Ui_D3DRenderingWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_D3DRENDERINGWIDGET_H
