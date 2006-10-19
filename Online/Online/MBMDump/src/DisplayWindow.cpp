#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace LHCb;
using namespace MBMDump;

DisplayWindow::DisplayWindow()  {
}

void DisplayWindow::buildMenu(BaseMenu* parent,int /* cmd_id */)    {
  const char* up_title="Offset  ------------ Hex Dump ------------- ---- Ascii dump ----  ";
  setParent(parent);
  ::upic_open_window();
  ::upic_open_menu(id(),0,0,"Display window",up_title,procName());
  for(int i=0; i < Constants::N_LINES; i++)
    ::upic_add_comment(i+1,"","");
  ::upic_close_menu();
}

void DisplayWindow::handleMenu(int /* cmd_id */ )    {
}
