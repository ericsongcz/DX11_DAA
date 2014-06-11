#include "stdafx.h"
#include "d3drenderingwidget.h"

D3DRenderingWidget::D3DRenderingWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(20);
}

D3DRenderingWidget::~D3DRenderingWidget()
{

}

void D3DRenderingWidget::resizeEvent(QResizeEvent* event)
{

}

void D3DRenderingWidget::paintEvent(QPaintEvent* event)
{
	if (updatesEnabled())
	{
		drawScene();
	}
}

HWND D3DRenderingWidget::getHWND()
{
	return (HWND)winId();
}

void D3DRenderingWidget::drawScene()
{

}