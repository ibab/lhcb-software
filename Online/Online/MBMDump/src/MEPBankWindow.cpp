#include "Event/RawBank.h"
#include "MDF/RawEventPrintout.h"
#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
using namespace LHCb;
using namespace MBMDump;

MEPBankWindow::MEPBankWindow(BaseMenu* par,int cmd_id, const Format& f, const RawBank* b)
: m_fmt(f) 
{
  char txt[256], tmp[32];
  setParent(par);
  m_parentCmd = cmd_id;
  ::upic_open_detached_menu(id(),0,0,"Display window"," MEP Fragment structure ",procName());
  ::upic_add_command(C_DISMISS,"Dismiss","");
  ::upic_enable_action_routine(id(),C_DISMISS, Routine(BaseMenu::dispatch));
  ::sprintf(txt,"RawBank  %s",RawEventPrintout::bankHeader(b).c_str());
  ::upic_add_comment(C_COM1,txt,"");
  const unsigned int *data = b->data(), *end = b->end<unsigned int>();
  txt[0] = 0;

  int nw = m_fmt.words_per_line;
  int cw = 10;  // Column 1 width
  int dw = 4;   // Distance between HEX and ascii dump
  int fw = 11;  // Width of HEX   format
  int aw = m_fmt.ascii_flag ? 4 : 0;  // Width of ASCII format
  int cnt, num;
  for(cnt = 0, num=C_DATA; data<end; ++cnt, ++data)  {
    if ( (cnt%nw) == 0 )  {
      ::upic_add_command(++num,txt,"");
      memset(txt,' ',sizeof(txt));
      // 0=nothing,1=offset,2=line number
      if(m_fmt.column_one_flag == 1)  {
        sprintf(tmp,"%4d",(char*)data-(char*)b->data());
        memcpy(txt+3,tmp,4);
      }
      else if(m_fmt.column_one_flag == 2)  {
        sprintf(tmp,"%4d",(data-b->data())/m_fmt.words_per_line);
        memcpy(txt+3,tmp,4);
      }
      else  {
        cw = 2;
      }
      txt[cw+nw*fw+dw+aw*nw] = 0;
      cnt = 0;
    }
    sprintf(tmp,m_fmt.fmt[cnt],*data);
    // sprintf(tmp,"%08X",*data);
    memcpy(&txt[cw+cnt*fw],tmp,strlen(tmp));
    if ( m_fmt.ascii_flag )  {
      rconv(tmp,*data);
      ::memcpy(&txt[cw+fw*nw+dw+aw*cnt],tmp,4);
    }
  }
  ::upic_add_command(++num,txt,"");
  ::upic_close_menu();
  ::upic_set_cursor(id(),C_DISMISS,1);
}

MEPBankWindow::~MEPBankWindow()  {
  ::upic_delete_menu(id());
}

void MEPBankWindow::handleMenu(int cmd_id)    {
  switch(cmd_id)  {
    case C_DISMISS:
      ::upic_hide_menu(id());
      ::upic_set_cursor(parent().id(),m_parentCmd,1);
      break;
    default:
      break;
  }
}

