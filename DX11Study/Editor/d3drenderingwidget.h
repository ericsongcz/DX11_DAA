#ifndef D3DRENDERINGWIDGET_H
#define D3DRENDERINGWIDGET_H

#include <QWidget>
#include "ui_d3drenderingwidget.h"
#include "Direct3DRenderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "GameTimer.h"

class D3DRenderingWidget : public QWidget
{
	Q_OBJECT

public:
	D3DRenderingWidget(QWidget *parent = 0);
	D3DRenderingWidget(int width, int height, QWidget* parent = 0);
	~D3DRenderingWidget();
	virtual QPaintEngine* paintEngine() const { return NULL; }
	HWND getHWND();

private:
	Ui::D3DRenderingWidget ui;

	float mScreenWidth;
	float mScreenHeight;
};

#endif // D3DRENDERINGWIDGET_H
