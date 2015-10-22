#include "VeloView.h"
#include <QtGui/QApplication>
#include <sstream>
#include <boost/python.hpp>
#include <stdlib.h>

int main(int argc, char *argv[]) {
//	std::vector<std::string> ops;
//	for (int i=1; i<argc; i++) {
//		std::string op(argv[i]);
//		std::cout<<"Adding option: "<<op<<std::endl;
//		ops.push_back(op);
//	}
//
//  QApplication a(argc, argv);
//  a.setStyle("plastique");
//
//  veloview w(0, &ops);
//  w.show();
//
//  std::cout<<"Done."<<std::endl;
//  return a.exec();
	std::cout<<"Hi! The method for starting the lovell offline gui has changed."<<std::endl;
	std::cout<<"If on plus, simply run: lovellpy"<<std::endl;
	std::cout<<"Otherwise, do (if in the top directory of Lovell):"<<std::endl;
	std::cout<<"./build.x86_64-slc6-gcc48-opt/run python VeloOfflineGUI/lovellGui/lovellGUI.py --run-data-dir=<VetraOutput>"<<std::endl;
}
