#include "stdafx.h"
#include "editor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Editor editor;
	editor.show();

	return a.exec();
}
