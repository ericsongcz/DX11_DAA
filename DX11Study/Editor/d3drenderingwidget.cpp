#include "stdafx.h"
#include "d3drenderingwidget.h"
#include "D3DUtils.h"
#include "FBXImporter.h"
#include "SharedParameters.h"

D3DRenderingWidget::D3DRenderingWidget(QWidget *parent)
	: QWidget(parent)
{

}

D3DRenderingWidget::D3DRenderingWidget(int width, int height, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	mScreenWidth = width;
	mScreenHeight = height;
	float topLeftY = ((QMainWindow*)parent)->menuBar()->height();

	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);
}

D3DRenderingWidget::~D3DRenderingWidget()
{
}


HWND D3DRenderingWidget::getHWND()
{
	return (HWND)winId();
}