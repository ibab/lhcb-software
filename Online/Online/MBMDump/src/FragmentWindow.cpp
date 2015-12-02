// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/FragmentWindow.cpp,v 1.6 2008-02-12 17:15:24 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: FragmentWindow.cpp,v 1.6 2008-02-12 17:15:24 frankb Exp $
//
// C++ include files
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "MBMDump/MBMDump.h"

using namespace LHCb;
using namespace MBMDump;

FragmentWindow::FragmentWindow(BaseMenu* par,int cmd_id, const Format& fmt, MEPFragment* f)
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(fmt), m_frag(f), m_bankWindow(0)
{
  char txt[256];
  openDetached(0,0,"Display window"," MEP Fragment structure ",procName());
  addCommand(C_DISMISS,"Dismiss");
  ::snprintf(txt,sizeof(txt)," MEP Fragment:Size:%7ld EID:%5d Start:0x%8p End:0x%8p",
	     long(f->size()),f->eventID(),(const void*)f->start(),(const void*)f->end());
  addComment(C_COM1,txt);
  addComment(C_COM2,"");
  addComment(C_COM3," Hit return on bank to see data");
  addComment(C_COM4,"");
  addComment(C_COM5,"+-----------------------------------------------+");
  addComment(C_COM6,"|Type     Type #  SourceID  Vsn    Size    Start|");
  addComment(C_COM7,"+-----------------------------------------------+");
  int cnt = 0;
  const RawBank* l = f->last();
  for(RawBank* b=f->first(); b<l; b=f->next(b), cnt++) {
    checkRawBank(b);
    ::snprintf(txt,sizeof(txt)," %-12s %2d %8d %5d %7d %p",
	       RawEventPrintout::bankType(b->type()).c_str(),
	       b->type(),b->sourceID(),b->version(),b->size(),(void*)b);
    addCommand(C_BANKS+cnt,txt);
  }
  addCommand(C_DISMISS2,"Dismiss");
  closeMenu();
  setCursor(C_DISMISS,1);
}

FragmentWindow::~FragmentWindow()  {
  drop(m_bankWindow);
  deleteMenu();
}

void FragmentWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
    case C_DISMISS2:
      hideMenu();
      parent().setCursor(m_parentCmd);
      break;
    default:
      if ( cmd_id >= C_BANKS )  {
        int cnt = C_BANKS;
        const RawBank* l = m_frag->last();
        for(RawBank* b=m_frag->first(); b<l; b=m_frag->next(b), cnt++) {
          checkRawBank(b);
          if ( cnt == cmd_id )  {
            output(RawEventPrintout::bankHeader(b).c_str());
            replace(m_bankWindow,new BankWindow(this,cmd_id,m_fmt,b));
            return;
          }
        }
      }
      break;
  }
}

