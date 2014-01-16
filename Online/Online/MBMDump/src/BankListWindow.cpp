// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/BankListWindow.cpp,v 1.6 2010-08-31 16:02:34 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: BankListWindow.cpp,v 1.6 2010-08-31 16:02:34 frankb Exp $
//
// C++ include files
#include <string>

// Framework include files
#include "Event/RawBank.h"
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MBMDump/MBMDump.h"
using namespace LHCb;
using namespace MBMDump;

BankListWindow::BankListWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& b, bool bld)
: BaseMenu(par), m_fmt(f), m_banks(b), m_bankWindow(0)
{
  m_parentCmd = cmd_id;
  if ( bld ) build();
}

BankListWindow::~BankListWindow()  {
  drop(m_bankWindow);
  deleteMenu();
}

void BankListWindow::build()  {
  char txt[256];
  openDetached(0,0,"Display window"," Bank list ",procName());
  addCommand(C_DISMISS,"Dismiss");
  addComment(C_COM2,"");
  addComment(C_COM3," Hit return on bank to see data");
  addComment(C_COM4,"");
  addComment(C_COM5,"+------------------------------------------------------------+");
  addComment(C_COM6,"| EventID Type         Type # SourceID   Vsn    Size    Start|");
  addComment(C_COM7,"+------------------------------------------------------------+");
  for(size_t cnt=0; cnt<m_banks.size();++cnt)  {
    unsigned int eid = m_banks[cnt].first;
    const RawBank* b = m_banks[cnt].second;
    if ( b->magic() != RawBank::MagicPattern )  {
      ::snprintf(txt,sizeof(txt),"EID:%u -> Corrpted data structures at %p - No magic pattern",eid,(void*)b);
      addComment(C_BANKS+cnt,txt);
      break;
    }
    ::snprintf(txt,sizeof(txt)," %8u %-16s %2d %8d %5d %7d %8p",
	      eid,RawEventPrintout::bankType(b->type()).c_str(),
	      b->type(),b->sourceID(),b->version(),b->size(),(void*)b);
    //if ( (cnt%10) != 0 )  {
    //  addComment(C_BANKS+cnt,txt);
    //  continue;
    //}
    addCommand(C_BANKS+cnt,txt);
  }
  addCommand(C_DISMISS2,"Dismiss");
  closeMenu();
  setCursor(C_DISMISS,1);
}

void BankListWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
    case C_DISMISS2:
      hideMenu();
      parent().setCursor(m_parentCmd,1);
      break;
    default:
      if ( cmd_id >= C_BANKS )  {
        for(size_t cnt=0; cnt<m_banks.size();++cnt)  {
          const RawBank* b = m_banks[cnt].second;
          checkRawBank(b);
          if ( cnt+C_BANKS == size_t(cmd_id) )  {
            output(RawEventPrintout::bankHeader(b).c_str());
            replace(m_bankWindow,new BankWindow(this,cmd_id,m_fmt,b));
            return;
          }
        }
      }
      break;
  }
}

