// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/MultiFragmentWindow.cpp,v 1.8 2010-09-07 13:50:43 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: MultiFragmentWindow.cpp,v 1.8 2010-09-07 13:50:43 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "NET/IPHeader.h"
#ifdef _WIN32
#include "winsock.h"
#else
#include "arpa/inet.h"
#endif
using namespace LHCb;
using namespace MBMDump;

MultiFragmentWindow::MultiFragmentWindow(BaseMenu* par,int cmd_id, const Format& fmt, MEPMultiFragment* mf)
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(fmt), m_frag(mf), 
  m_fragWindow(0), m_banksWindow(0)
{
  char txt[256];
  openDetached(0,0,"Display window"," MEP Multi Fragment structure ",procName());
  addCommand(C_DISMISS,"Dismiss");
  addCommand(C_SHOWBANKS,"Show all banks");
  ::snprintf(txt,sizeof(txt),"Multi event fragment with partition ID: %4X",mf->partitionID());
  addComment(C_COM1,txt);
  addComment(C_COM2,"    Size Packing  EidH Start      End");
  ::snprintf(txt,sizeof(txt)," %7ld %7ld %5u %10p %10p",
	     long(mf->size()),long(mf->packing()),mf->eventID(),
	     (const void*)mf->start(),(const void*)mf->end());
  addComment(C_COM2,txt);
  RTL::IPHeader* ip = (RTL::IPHeader*)mf->ipHeader();
  ::snprintf(txt,sizeof(txt),"    IP header: HLV:%02X TOS:%02X Len:%04X ID:%04X FragOff:%04X TTL:%02X",
	     ip->hlv,ip->tos,ip->tot_len,ip->id, ip->frag_off,ip->ttl);
  addComment(C_COM3,txt);
  ::snprintf(txt,sizeof(txt),"               Protocol:%02X Chk:%04X Saddr:%-15s Daddr:%-15s",
	     ip->protocol,ip->check,inet_ntoa(*(in_addr*)&ip->saddr),inet_ntoa(*(in_addr*)&ip->daddr));
  addComment(C_COM4, txt);
  addComment(C_COM5, "");
  addComment(C_COM6, " Hit return on fragment to see banks");
  addComment(C_COM7, "");
  addComment(C_COM8, "+-----------------------------------------+");
  addComment(C_COM9, "| #    Size  EidH Start      End          |");
  addComment(C_COM10,"+-----------------------------------------+");
  int cnt = 0;
  for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f), ++cnt) {
    ::snprintf(txt,sizeof(txt),"%3d:%7ld %5d %10p %10p",
	      cnt,long(f->size()),f->eventID(),
	       (const void*)f->start(),(const void*)f->end());
    addCommand(C_FRAGS+cnt,txt);
  }
  closeMenu();
  setCursor(C_DISMISS,1);
}

MultiFragmentWindow::~MultiFragmentWindow()  {
  drop(m_fragWindow);
  deleteMenu();
}

void MultiFragmentWindow::handleMenu(int cmd_id)    {
  MEPFragment* f;
  unsigned int pid = 0;
  BankListWindow::Banks banks;
  switch(cmd_id)  {
    case C_DISMISS:
      hideMenu();
      parent().setCursor(m_parentCmd,1);
      break;
    case C_SHOWBANKS:
      if ( decodeMultiFragment2Banks(m_frag,pid,banks).isSuccess() )  {
        replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmt,banks));
      }
      break;
    default:
      if ( cmd_id >= C_FRAGS )  {
        int cnt = C_FRAGS;
        for (f = m_frag->first(); f<m_frag->last(); f=m_frag->next(f), ++cnt) {
          if ( cmd_id == cnt )   {
            output("Found MEP Fragment:Size:%7d EID:%5d Start:%10p End:%10p",
		   f->size(),f->eventID(),f->start(),f->end());
            replace(m_fragWindow,new FragmentWindow(this,cmd_id,m_fmt,f));
            return;
          }
        }
      }
      break;
  }
}

