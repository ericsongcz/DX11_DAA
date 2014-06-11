#include "stdafx.h"
#include "editor.h"
#include "d3drenderingwidget.h"

Editor::Editor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	D3DRenderingWidget* d3dWidget = new D3DRenderingWidget(this);
	setCentralWidget(d3dWidget);
}

Editor::~Editor()
{

}

void Editor::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		close();
		break;
	default:
		break;
	}
}
