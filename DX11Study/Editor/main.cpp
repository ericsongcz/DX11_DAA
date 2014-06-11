#include "stdafx.h"
#include "editor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Editor w;
	w.setFixedWidth(800);
	w.setFixedHeight(600);
	w.show();
	return a.exec();
}
