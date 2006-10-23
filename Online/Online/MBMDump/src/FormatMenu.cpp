#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace MBMDump;

static char *col1_list[3] = {" nothing","  offset","line no."};

FormatMenu::FormatMenu(BaseMenu* par,int cmd_id) : BaseMenu(par)  {
  strcpy(m_col1Value,col1_list[1]);
  strcpy(m_fmtString,"XXXX");
  handleMenu(C_COL1);
  handleMenu(C_NW);
  handleMenu(C_FMT);
  ::upic_open_detached_menu(id(),parent().id(),cmd_id,"Edit Bank format","",procName());
  ::upic_set_param(m_col1Value,2,"%8s",col1_list[1],0,0,col1_list,3,1);
  ::upic_add_command(C_COL1,"Column 1     ^^^^^^^^","");
  ::upic_set_param(&m_fmt.words_per_line,3,"%1d",m_fmt.words_per_line,1,MAX_WORDS_PER_LINE,0,0,0);
  ::upic_add_command(C_NW,"Words per line      ^  ","");
  ::upic_set_param(m_fmtString,5,"%8s","XXXX    ",0,0,0,0,0);    
  ::upic_add_command(C_FMT,"Format       ^^^^^^^^ ","");
  ::upic_enable_action_routine(id(),C_COL1,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_NW,  Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_FMT, Routine(BaseMenu::dispatch));
  ::upic_close_menu();
}

void FormatMenu::handleMenu(int cmd_id)  {
  int i, j;
  switch(cmd_id)    {
  case C_COL1:
    // 0=nothing,1=offset,2=line number
    for(i = 0, m_fmt.column_one_flag=1; i<4; i++) {
      if(strcmp(m_col1Value,col1_list[i]) == 0)  {
        m_fmt.column_one_flag = i;
        return;
      }
    }
    return;
  case C_NW:
    return;
  case C_FMT:
    // decode string
    for(i=MAX_WORDS_PER_LINE, j=0; i > 0; --i)  {
      switch(toupper(m_fmtString[MAX_WORDS_PER_LINE - i])){
      case 'D':
        m_fmt.fmt[j++] = Format::FMT_DEC10();
        break;
      default:
      case 'X':
        m_fmt.fmt[j++] = Format::FMT_HEX08();
        break;
      }
    }
    break;
  }
}


