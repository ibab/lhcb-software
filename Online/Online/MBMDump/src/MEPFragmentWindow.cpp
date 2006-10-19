#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPEvent.h"
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace LHCb;
using namespace MBMDump;

MEPFragmentWindow::MEPFragmentWindow(BaseMenu* par,int cmd_id, const Format& fmt, MEPFragment* f)
: m_frag(f), m_bankWindow(0), m_fmt(fmt)
{
  char txt[256];
  setParent(par);
  m_parentCmd = cmd_id;
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Fragment structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::sprintf(txt," MEP Fragment:Size:%7d EID:%5d Start:0x%8p End:0x%8p",
    f->size(),f->eventID(),f->start(),f->end());
  ::upic_add_comment(C_COM1,txt,"");
  ::upic_add_comment(C_COM2,"","");
  ::upic_add_comment(C_COM3," Hit return on bank to see data","");
  ::upic_add_comment(C_COM4,"","");
  ::upic_add_comment(C_COM5,"+-----------------------------------------------+","");
  ::upic_add_comment(C_COM6,"|Type     Type #  SourceID  Vsn    Size    Start|","");
  ::upic_add_comment(C_COM7,"+-----------------------------------------------+","");
  int cnt = 0;
  const RawBank* l = f->last();
  for(RawBank* b=f->first(); b<l; b=f->next(b), cnt++) {
    checkRawBank(b);
    ::sprintf(txt," %-12s %2d %8d %5d %7d %08p",
      RawEventPrintout::bankType(b->type()).c_str(),
      b->type(),b->sourceID(),b->version(),b->size(),(void*)b);
    ::upic_add_command(C_BANKS+cnt,txt,"");
    ::upic_enable_action_routine(id(),C_BANKS+cnt, Routine(BaseMenu::dispatch));
  }
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

MEPFragmentWindow::~MEPFragmentWindow()  {
  drop(m_bankWindow);
  ::upic_delete_menu(id());
}

void MEPFragmentWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
      ::upic_hide_menu(id());
      ::upic_set_cursor(parent().id(),m_parentCmd,1);
      break;
    default:
      if ( cmd_id >= C_BANKS )  {
        int cnt = C_BANKS;
        const RawBank* l = m_frag->last();
        for(RawBank* b=m_frag->first(); b<l; b=m_frag->next(b), cnt++) {
          checkRawBank(b);
          if ( cnt == cmd_id )  {
            ::upic_write_message(RawEventPrintout::bankHeader(b).c_str(),"");
            replace(m_bankWindow,new MEPBankWindow(this,cmd_id,m_fmt,b));
            return;
          }
        }
      }
      break;
  }
}

