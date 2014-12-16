#include "headers/VeloView.h"
#include <QApplication>
#include <sstream>

int main(int argc, char *argv[]) {
  int mode = 0; // 0 for VeloView, 1 for CombatDQM.
  if (argc == 2) mode = atoi(argv[1]);
  QApplication a(argc, argv);
  a.setStyle("plastique");

  veloview w(mode);
  w.show();

  std::cout<<"Done."<<std::endl;
  return a.exec();
}
