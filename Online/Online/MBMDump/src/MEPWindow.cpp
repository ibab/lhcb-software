// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/MEPWindow.cpp,v 1.10 2010-08-31 16:02:34 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: MEPWindow.cpp,v 1.10 2010-08-31 16:02:34 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "MBM/mepdef.h"
using namespace LHCb;
using namespace MBMDump;

MEPWindow::MEPWindow(DisplayMenu* par,int cmd_id, const Format& f) 
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(f), 
  m_banksWindow(0), m_multiFrags(0)
{
  char txt[256];
  openDetached(0,0,"Display window"," MEP Header structure ",procName());
  addCommand(C_DISMISS,"Dismiss");
  addCommand(C_SHOWBANKS,"Show all banks");

  const DataBlock& d = par->evtData();
  MEPEVENT* e  = (MEPEVENT*)d.start;
  MEPEvent* me = (MEPEvent*)e->data;
  ::snprintf(txt,sizeof(txt),"MEP Event: size %ld Bytes  [MBM size:%ld Bytes]",
	    long(me->size()), long(d.length*sizeof(int)));
  addComment(C_COM1,txt);
  unsigned int eid_h = 0, pid = 0, cnt = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf),++cnt) {
    eid_h = mf->eventID();
    if ( pid == 0 )  {
      pid = mf->partitionID();
      ::snprintf(txt,sizeof(txt),"Partition ID: %4X",pid);
      addComment(C_COM2,"");
      addComment(C_COM3," Hit return on Multi Event Fragment for details");
      addComment(C_COM4,"");
      addComment(C_COM5,"List of Multi Event Fragments:");
      addComment(C_COM6,"+--------------------------------------------------+");
      addComment(C_COM7,"| #    Size Packing  EidH Start      End           |");
      addComment(C_COM8,"+--------------------------------------------------+");
    }
    snprintf(txt,sizeof(txt),"%3u:%7ld %7ld %5u %10p %10p",
	    cnt,long(mf->size()),long(mf->packing()),eid_h,
	     (const void*)mf->start(),(const void*)mf->end());
    addCommand(C_MULTIFRAGS+cnt,txt);
  }
  closeMenu();
  setCursor(C_DISMISS,1);
}

MEPWindow::~MEPWindow()  {
  drop(m_multiFrags);
  deleteMenu();
}

void MEPWindow::handleMenu(int cmd_id)    {
  const DataBlock& d = dynamic_cast<DisplayMenu*>(&parent())->evtData();
  MEPEVENT* e  = (MEPEVENT*)d.start;
  MEPEvent* me = (MEPEvent*)e->data;
  unsigned int pid = 0;
  BankListWindow::Banks banks;
  switch(cmd_id)  {
    case C_DISMISS:
      hideMenu();
      parent().setCursor(m_parentCmd,1);
      return;
    case C_SHOWBANKS:
      if ( decodeMEP2Banks(me,pid,banks).isSuccess() )  {
        replace(m_banksWindow,new BankListWindow(this,cmd_id,m_fmt,banks));
      }
      return;
    default:
      if ( cmd_id >= C_MULTIFRAGS )  {
        MEPMultiFragment* mf;
        unsigned int cnt = 0;
        for( mf = me->first(); mf<me->last(); mf=me->next(mf),++cnt) {
          if( cmd_id-C_MULTIFRAGS == int(cnt) )  {
            replace(m_multiFrags,new MultiFragmentWindow(this, cmd_id, m_fmt, mf));
            return;
          }
        }
      }
      return;
  }
}

