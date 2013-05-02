// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/FileMenu.cpp,v 1.3 2008-02-12 17:15:24 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: FileMenu.cpp,v 1.3 2008-02-12 17:15:24 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
using namespace MBMDump;

FileMenu::FileMenu(FileMainMenu* par,int cmd_id) : BaseMenu(par), m_parentCmd(cmd_id)
{
  m_file = dynamic_cast<FileMainMenu*>(par);
  ::strncpy(m_fileName,par->fileName(),sizeof(m_fileName));
  openDetached(parent().id(),m_parentCmd,"Supply file name","",procName());
  setParam(m_fileName,1,"%96s",m_fileName,0,0,0,0,0);
  addCommand(1,"File Name:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  closeMenu();
}

FileMenu::~FileMenu() {
  deleteMenu();
}

void FileMenu::handleMenu(int cmd_id)   {
  if ( cmd_id == 1 ) m_file->acceptFile(m_fileName);
}

