#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace MBMDump;

static const char *pr_list[2] = {"    window","bank/event"};
PrintMenu::PrintMenu(BaseMenu* par, int cmd_id) : BaseMenu(par) {
  static char *mode_list[2] = {"w","a"};
  strcpy(m_prtFlag,"    window");
  strcpy(m_fileMode,"w");
  ::upic_open_detached_menu(id(),parent().id(),cmd_id,"Hardcopy parameters"," ",procName());
  ::upic_set_param  (m_prtFlag,1,"%10s",pr_list[0],0,0,pr_list,2,1);
  ::upic_add_command(C_PR,"What to dump        ^^^^^^^^^^"," ");
  ::upic_set_param  (&m_mxLines,4,"%5d",100,0,99999,0,0,0);
  ::upic_add_command(C_NL,"Maximum number of lines  ^^^^^"," ");
  ::upic_add_comment(73,"Enter output filename"," ");
  ::upic_set_param  (m_fileName,2,"%40s"," ",0,0,0,0,0);
  ::upic_add_command(C_FN,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"," ");
  ::upic_set_param  (m_fileMode,3,"%1s",mode_list[0],0,0,mode_list,2,1);
  ::upic_add_command(C_AP,"Write or  append to file     ^"," a => append to file");
  ::upic_close_menu();
  ::upic_enable_action_routine(id(),C_PR,(Routine)BaseMenu::dispatch);
  ::upic_enable_action_routine(id(),C_NL,(Routine)BaseMenu::dispatch);
  ::upic_enable_action_routine(id(),C_FN,(Routine)BaseMenu::dispatch);
  ::upic_enable_action_routine(id(),C_AP,(Routine)BaseMenu::dispatch);
  m_mxLines = 100;
}

void PrintMenu::handleMenu(int cmd_id)  {
  switch(cmd_id){
    case C_PR:
      print_flag = strcmp(m_prtFlag,pr_list[0])==0 ? WINDOW : C_DATA;
      break;
    case C_FN:
      /* terminate m_fileName */
      if(strchr(m_fileName,' ')) *strchr(m_fileName,' ')=0;
      break;
    case C_AP:
      break;
  }
}

