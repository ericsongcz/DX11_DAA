#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_editor.h"
#include "d3drenderingwidget.h"
#include "Direct3DRenderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "GameTimer.h"

class Editor : public QMainWindow
{
	Q_OBJECT

public:
	Editor(QWidget *parent = 0);
	~Editor();

	HWND getHWND();
	void drawScene();

protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void resizeEvent(QResizeEvent* event);
	virtual void paintEvent(QPaintEvent* event);
	virtual QPaintEngine* paintEngine() const { return NULL; }

private:
	Ui::EditorClass ui;
	D3DRenderingWidget* d3dWidget;
	Direct3DRenderer* mRenderer;
	Camera* mCamera;
	Geometry* mGeometry;
	float mScreenWidth;
	float mScreenHeight;
	GameTimer mTimer;
};

#endif // EDITOR_H
