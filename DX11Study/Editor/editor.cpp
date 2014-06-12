#include "stdafx.h"
#include "editor.h"
#include <QLayout>
#include <QPushButton>

Editor::Editor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	resize(QSize(1024, 768));

	int menuBarHeight = menuBar()->height();
	int screenWidth = width();
	int screenHeight = height() - menuBarHeight;

	d3dWidget = new D3DRenderingWidget(screenWidth, screenHeight, this);
	d3dWidget->setGeometry(QRect(0, menuBarHeight, screenWidth, screenHeight));
	d3dWidget->installEventFilter(this);

	setWindowTitle(tr("Qt D3D Demo"));

	QPushButton* button = new QPushButton(this);
	button->setText(tr("fuck me"));
	button->setGeometry(90, 10, 60, 20);

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(d3dWidget);
	mainLayout->addWidget(button);

	setLayout(mainLayout);
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
	d3dWidget->resize(width(), height() - menuBar()->height());
}
