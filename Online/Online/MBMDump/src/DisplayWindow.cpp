// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/DisplayWindow.cpp,v 1.5 2008-02-12 17:15:24 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DisplayWindow.cpp,v 1.5 2008-02-12 17:15:24 frankb Exp $
//
// C++ include files
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
