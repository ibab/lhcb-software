#include "MBMDump/MBMDump.h"
using namespace LHCb;
using namespace MBMDump;

DisplayWindow::DisplayWindow(BaseMenu* par,int /* cmd_id */) : BaseMenu(par) {
  openWindow();
  openMenu(0,0,"Display window",
    "Offset  ------------ Hex Dump ------------- ---- Ascii dump ----  ",procName());
  for(int i=0; i < Constants::N_LINES; i++)
    addComment(i+1,"");
  closeMenu();
}

DisplayWindow::~DisplayWindow()  {
  deleteMenu();
}

void DisplayWindow::handleMenu(int /* cmd_id */ )    {
}
