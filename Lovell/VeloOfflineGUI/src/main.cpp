#include "VeloView.h"
#include <QtGui/QApplication>
#include <sstream>

int main(int argc, char *argv[]) {
	std::vector<std::string> ops;
	for (int i=1; i<argc; i++) {
		std::string op(argv[i]);
		std::cout<<"Adding option: "<<op<<std::endl;
		ops.push_back(op);
	}

  QApplication a(argc, argv);
  a.setStyle("plastique");

  veloview w(0, &ops);
  w.show();

  std::cout<<"Done."<<std::endl;
  return a.exec();
}
