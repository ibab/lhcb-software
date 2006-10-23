#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace LHCb;
using namespace MBMDump;

DisplayWindow::DisplayWindow(BaseMenu* par,int /* cmd_id */) : BaseMenu(par) {
  ::upic_open_window();
  ::upic_open_menu(id(),0,0,"Display window",
    "Offset  ------------ Hex Dump ------------- ---- Ascii dump ----  ",procName());
  for(int i=0; i < Constants::N_LINES; i++)
    ::upic_add_comment(i+1,"","");
  ::upic_close_menu();
}

DisplayWindow::~DisplayWindow()  {
  ::upic_delete_menu(id());
}

void DisplayWindow::handleMenu(int /* cmd_id */ )    {
}
