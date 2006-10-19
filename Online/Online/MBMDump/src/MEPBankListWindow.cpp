#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace LHCb;
using namespace MBMDump;

MEPBankListWindow::MEPBankListWindow(BaseMenu* par,int cmd_id, const Format& f, Banks& b)
: m_banks(b), m_bankWindow(0), m_fmt(f)
{
  char txt[256];
  setParent(par);
  m_parentCmd = cmd_id;
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Fragment structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::upic_add_comment(C_COM2,"","");
  ::upic_add_comment(C_COM3," Hit return on bank to see data","");
  ::upic_add_comment(C_COM4,"","");
  ::upic_add_comment(C_COM5,"+-------------------------------------------------------+","");
  ::upic_add_comment(C_COM6,"|EventID Type     Type #  SourceID  Vsn    Size    Start|","");
  ::upic_add_comment(C_COM7,"+-------------------------------------------------------+","");
  for(size_t cnt=0; cnt<m_banks.size();++cnt)  {
    unsigned int eid = m_banks[cnt].first;
    const RawBank* b = m_banks[cnt].second;
    if ( b->magic() != RawBank::MagicPattern )  {
      sprintf(txt,"EID:%d -> Corrpted data structures at %p - No magic pattern",eid,b);
      ::upic_add_comment(C_BANKS+cnt,txt,"");
      break;
    }
    ::sprintf(txt," %8d %-12s %2d %8d %5d %7d %08p",
      eid,RawEventPrintout::bankType(b->type()).c_str(),
      b->type(),b->sourceID(),b->version(),b->size(),(void*)b);
    ::upic_add_command(C_BANKS+cnt,txt,"");
    ::upic_enable_action_routine(id(),C_BANKS+cnt, Routine(BaseMenu::dispatch));
  }
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

MEPBankListWindow::~MEPBankListWindow()  {
  drop(m_bankWindow);
  ::upic_delete_menu(id());
}

void MEPBankListWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
      ::upic_hide_menu(id());
      ::upic_set_cursor(parent().id(),m_parentCmd,1);
      break;
    default:
      if ( cmd_id >= C_BANKS )  {
        for(size_t cnt=0; cnt<m_banks.size();++cnt)  {
          const RawBank* b = m_banks[cnt].second;
          checkRawBank(b);
          if ( cnt+C_BANKS == cmd_id )  {
            ::upic_write_message(RawEventPrintout::bankHeader(b).c_str(),"");
            replace(m_bankWindow,new MEPBankWindow(this,cmd_id,m_fmt,b));
            return;
          }
        }
      }
      break;
  }
}

