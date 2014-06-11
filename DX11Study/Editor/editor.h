#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_editor.h"
#include "d3drenderingwidget.h"

class Editor : public QMainWindow
{
	Q_OBJECT

public:
	Editor(QWidget *parent = 0);
	~Editor();

	virtual void keyPressEvent(QKeyEvent *event);
	bool eventFilter(QObject *target, QEvent *event);

private:
	Ui::EditorClass ui;
	D3DRenderingWidget* d3dWidget;
};

#endif // EDITOR_H
