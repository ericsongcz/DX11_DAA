#ifndef D3DRENDERINGWIDGET_H
#define D3DRENDERINGWIDGET_H

#include <QWidget>
#include "ui_d3drenderingwidget.h"

class D3DRenderingWidget : public QWidget
{
	Q_OBJECT

public:
	D3DRenderingWidget(QWidget *parent = 0);
	~D3DRenderingWidget();

	virtual QPaintEngine* painteEngine() const { return nullptr; }
	bool initDirect3D();
	HWND getHWND();

	void drawScene();
protected:
	virtual void resizeEvent(QResizeEvent* event);
	virtual void paintEvent(QPaintEvent* event);
	virtual void keyPressEvent(QKeyEvent *event);
private:
	Ui::D3DRenderingWidget ui;
};

#endif // D3DRENDERINGWIDGET_H
