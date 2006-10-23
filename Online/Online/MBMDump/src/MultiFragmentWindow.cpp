#include "MBMDump/MBMDump.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "NET/IPHeader.h"
#include "UPI/upidef.h"
#ifdef _WIN32
#include "winsock.h"
#else
#include "arpa/inet.h"
#endif
using namespace LHCb;
using namespace MBMDump;

MultiFragmentWindow::MultiFragmentWindow(BaseMenu* par,int cmd_id, const Format& f, MEPMultiFragment* mf)
: BaseMenu(par), m_parentCmd(cmd_id), m_fmt(f), m_frag(mf), 
  m_fragWindow(0), m_banksWindow(0)
{
  char txt[256];
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Multi Fragment structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::upic_add_command(C_SHOWBANKS,"Show all banks","");
  ::upic_enable_action_routine(id(),C_SHOWBANKS, Routine(BaseMenu::dispatch));

  ::sprintf(txt,"Multi event fragment with partition ID: %4X",mf->partitionID());
  ::upic_add_comment(C_COM1,txt,"");
  ::upic_add_comment(C_COM2,"    Size Packing  EidH Start      End","");
  ::sprintf(txt," %7d %7d %5d 0x%8p 0x%8p",
    mf->size(),mf->packing(),mf->eventID(),mf->start(),mf->end());
  ::upic_add_comment(C_COM2,txt,"");
  RTL::IPHeader* ip = (RTL::IPHeader*)mf->ipHeader();
  ::sprintf(txt,"    IP header: HLV:%02X TOS:%02X Len:%04X ID:%04X FragOff:%04X TTL:%02X",
    ip->hlv,ip->tos,ip->tot_len,ip->id, ip->frag_off,ip->ttl);
  ::upic_add_comment(C_COM3,txt,"");
  ::sprintf(txt,"               Protocol:%02X Chk:%04X Saddr:%-15s Daddr:%-15s",
    ip->protocol,ip->check,inet_ntoa(*(in_addr*)&ip->saddr),inet_ntoa(*(in_addr*)&ip->daddr));
  ::upic_add_comment(C_COM4,txt,"");
  ::upic_add_comment(C_COM5,"","");
  ::upic_add_comment(C_COM6," Hit return on fragment to see banks","");
  ::upic_add_comment(C_COM7,"","");
  ::upic_add_comment(C_COM8,"+-----------------------------------------+","");
  ::upic_add_comment(C_COM9,"| #    Size  EidH Start      End          |","");
  ::upic_add_comment(C_COM10,"+-----------------------------------------+","");
  int cnt = 0;
  for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f), ++cnt) {
    ::sprintf(txt,"%3d:%7d %5d 0x%8p 0x%8p",cnt,f->size(),f->eventID(),f->start(),f->end());
    ::upic_add_command(C_FRAGS+cnt,txt,"");
    ::upic_enable_action_routine(id(),C_FRAGS+cnt, Routine(BaseMenu::dispatch));
  }
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

MultiFragmentWindow::~MultiFragmentWindow()  {
  drop(m_fragWindow);
  ::upic_delete_menu(id());
}

void MultiFragmentWindow::handleMenu(int cmd_id)    {
  MEPFragment* f;
  unsigned int pid = 0;
  BankListWindow::Banks banks;
  switch(cmd_id)  {
    case C_DISMISS:
      ::upic_hide_menu(id());
      ::upic_set_cursor(parent().id(),m_parentCmd,1);
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
            ::upic_write_message2("Found MEP Fragment:Size:%7d EID:%5d Start:0x%8p End:0x%8p",
              f->size(),f->eventID(),f->start(),f->end());
            replace(m_fragWindow,new FragmentWindow(this,cmd_id,m_fmt,f));
            return;
          }
        }
      }
      break;
  }
}

