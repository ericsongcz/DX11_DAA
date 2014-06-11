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
