#include "VeloView.h"
#include <QtGui/QApplication>
#include <sstream>

int main(int argc, char *argv[]) {
  int mode = 0; // 0 for VeloView
  if (argc == 2) mode = atoi(argv[1]);
  QApplication a(argc, argv);
  a.setStyle("plastique");

  veloview w(mode);
  w.show();

  std::cout<<"Done."<<std::endl;
  return a.exec();
}
