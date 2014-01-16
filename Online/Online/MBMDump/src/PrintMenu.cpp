// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/PrintMenu.cpp,v 1.4 2008-02-12 17:15:24 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: PrintMenu.cpp,v 1.4 2008-02-12 17:15:24 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"

using namespace MBMDump;

static const char *pr_list[] = {"    window","bank/event"};
static const char *mode_list[] = {"w","a"};
PrintMenu::PrintMenu(BaseMenu* par, int cmd_id) : BaseMenu(par) {
  ::snprintf(m_prtFlag,sizeof(m_prtFlag),"    window");
  ::snprintf(m_fileMode,sizeof(m_fileMode),"w");
  openDetached(parent().id(),cmd_id,"Hardcopy parameters"," ",procName());
  setParam  (m_prtFlag,1,"%10s",pr_list[0],0,0,pr_list,2,1);
  addCommand(C_PR,"What to dump        ^^^^^^^^^^");
  setParam  (&m_mxLines,4,"%5d",100,0,99999,0,0,0);
  addCommand(C_NL,"Maximum number of lines  ^^^^^");
  addComment(C_COM1,"Enter output filename");
  setParam  (m_fileName,2,"%40s"," ",0,0,0,0,0);
  addCommand(C_FN,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  setParam  (m_fileMode,3,"%1s",mode_list[0],0,0,mode_list,2,1);
  addCommand(C_AP,"Write or  append to file     ^");
  closeMenu();
  m_mxLines = 100;
}

void PrintMenu::handleMenu(int cmd_id)  {
  switch(cmd_id){
    case C_PR:
      print_flag = strcmp(m_prtFlag,pr_list[0])==0 ? WINDOW : C_DATA;
      break;
    case C_FN:
      /* terminate m_fileName */
      if(strchr(m_fileName,' ')) *strchr(m_fileName,' ')=0;
      break;
    case C_AP:
      break;
  }
}

