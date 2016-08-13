#include <QApplication>
#include <iostream>
#include "CWindow.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		std::cerr << "No mask file!!!" << std::endl;
		return -1;
	}

	QApplication app(argc, argv);

	CWindow *window = new CWindow(argv[1]);
	window->show();

	return qApp->exec();
}
