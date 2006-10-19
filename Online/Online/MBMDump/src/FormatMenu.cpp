#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"
#include <cctype>
using namespace LHCb;
using namespace MBMDump;
#define FMT_HEX08    " %08X "
#define FMT_DEC10      "%10d"

static char *col1_list[3] = {" nothing","  offset","line no."};

FormatMenu::FormatMenu()  {
  m_fmt.words_per_line  = 4;
  m_fmt.column_one_flag = 1;
  m_fmt.ascii_flag      = true;
  strcpy(m_col1Value,col1_list[1]);
  strcpy(m_fmtString,"XXXX");
  handleMenu(C_COL1);
  handleMenu(C_NW);
  handleMenu(C_FMT);
}

void FormatMenu::buildMenu(BaseMenu* par,int cmd_id)  {
  setParent(par);
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
    // default all words to hex....in case of dud string
    for(i=0;i<MAX_WORDS_PER_LINE;i++)
      m_fmt.fmt[i] = FMT_HEX08;
    // decode string
    i=MAX_WORDS_PER_LINE;
    j=0;
    while(i > 0){
      switch(toupper(m_fmtString[MAX_WORDS_PER_LINE - i])){
      case 'X':
        m_fmt.fmt[j++] = FMT_HEX08;
        break;
      case 'D':
        m_fmt.fmt[j++] = FMT_DEC10;
        break;
      }
      i--;
    }
    break;
  }
}


