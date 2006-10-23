#include "MBMDump/MBMDump.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "UPI/upidef.h"
#include "MBM/mepdef.h"
using namespace LHCb;
using namespace MBMDump;

MEPWindow::MEPWindow(DisplayMenu* par,int cmd_id, const Format& f) 
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(f), 
  m_banksWindow(0), m_multiFrags(0)
{
  char txt[256];
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Header structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::upic_add_command(C_SHOWBANKS,"Show all banks","");
  ::upic_enable_action_routine(id(),C_SHOWBANKS, Routine(BaseMenu::dispatch));

  const DataBlock& d = par->evtData();
  MEPEVENT* e  = (MEPEVENT*)d.start;
  MEPEvent* me = (MEPEvent*)e->data;
  ::sprintf(txt,"MEP Event: size %d Bytes  [MBM size:%d Bytes]",
    me->size(), d.length*sizeof(int));
  upic_add_comment(C_COM1,txt,"");
  unsigned int eid_h = 0, pid = 0, cnt = 0;
  for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf),++cnt) {
    eid_h = mf->eventID();
    if ( pid == 0 )  {
      pid = mf->partitionID();
      sprintf(txt,"Partition ID: %4X",pid);
      ::upic_add_comment(C_COM2,"","");
      ::upic_add_comment(C_COM3," Hit return on Multi Event Fragment for details","");
      ::upic_add_comment(C_COM4,"","");
      ::upic_add_comment(C_COM5,"List of Multi Event Fragments:","");
      ::upic_add_comment(C_COM6,"+--------------------------------------------------+","");
      ::upic_add_comment(C_COM7,"| #    Size Packing  EidH Start      End           |","");
      ::upic_add_comment(C_COM8,"+--------------------------------------------------+","");
    }
    sprintf(txt,"%3d:%7d %7d %5d 0x%8p 0x%8p",
      cnt,mf->size(),mf->packing(),eid_h,mf->start(),mf->end());
    ::upic_add_command(C_MULTIFRAGS+cnt,txt,"");
    ::upic_enable_action_routine(id(),C_MULTIFRAGS+cnt, Routine(BaseMenu::dispatch));
  }
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

MEPWindow::~MEPWindow()  {
  drop(m_multiFrags);
  ::upic_delete_menu(id());
}

void MEPWindow::handleMenu(int cmd_id)    {
  const DataBlock& d = dynamic_cast<DisplayMenu*>(&parent())->evtData();
  MEPEVENT* e  = (MEPEVENT*)d.start;
  MEPEvent* me = (MEPEvent*)e->data;
  unsigned int pid = 0;
  MEPBankListWindow::Banks banks;
  switch(cmd_id)  {
    case C_DISMISS:
      ::upic_hide_menu(id());
      ::upic_set_cursor(parent().id(),m_parentCmd,1);
      return;
    case C_SHOWBANKS:
      if ( decodeMEP2Banks(me,pid,banks).isSuccess() )  {
        replace(m_banksWindow,new MEPBankListWindow(this,cmd_id,m_fmt,banks));
      }
      return;
    default:
      if ( cmd_id >= C_MULTIFRAGS )  {
        MEPMultiFragment* mf;
        unsigned int cnt = 0;
        for( mf = me->first(); mf<me->last(); mf=me->next(mf),++cnt) {
          if( cmd_id-C_MULTIFRAGS == int(cnt) )  {
            replace(m_multiFrags,new MEPMultiFragmentWindow(this, cmd_id, m_fmt, mf));
            return;
          }
        }
      }
      return;
  }
}

