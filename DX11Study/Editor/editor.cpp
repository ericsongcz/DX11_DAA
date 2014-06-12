#include "stdafx.h"
#include "editor.h"

Editor::Editor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	resize(QSize(1024, 768));

	int w = frameGeometry().width();

	d3dWidget = new D3DRenderingWidget(width(), height(), this);
	//setCentralWidget(d3dWidget);

	d3dWidget->installEventFilter(this);

	setWindowTitle(tr("Qt D3D Demo"));
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

bool Editor::eventFilter(QObject *target, QEvent *event)
{
	if (target == d3dWidget)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(event);

			if (ke->key() == Qt::Key_Escape)
			{
				// special tab handling here     
				close();
				return true;
			}
		}
	}

	return QWidget::eventFilter(target, event);
}

void Editor::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	int w = width();
	d3dWidget->resize(width(), height());
}
