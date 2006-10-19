#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace LHCb;
using namespace MBMDump;

static char *rtype_list[2] = {"ANY", "ALL"};
static char *mode_list[3]  = {"NOT ALL", "    ALL", "    ONE" };

Requirement::Requirement() 
: m_evType(1), m_reqActive(false), m_trMask(~0x0), m_veto(0)
{
  m_bmID      = MBM_INV_DESC;
  m_reqTyp    = BM_MASK_ANY;
  m_reqMode   = BM_NOTALL;
  strcpy(m_mode_c,mode_list[0]);
  strcpy(m_rtype_c,rtype_list[0]);
}

void Requirement::buildMenu(int pg, int menu_id, int cmd_id)   {
  char title[20] = "Requirement no. 1";
  unsigned int* tr = (unsigned int*)m_trMask.bits();
  unsigned int* vt = (unsigned int*)m_veto.bits();
  title[16] = '1'+pg;
  ::upic_open_menu(id(),menu_id,cmd_id,title,"",procName());
  ::upic_add_command(C_ADD,"Add Requirement", "");
  ::upic_set_param(&m_evType,C_EVT,"%2d",1,0,10,0,0,0);
  ::upic_add_command(C_EVT,"Event type           ^^","");
  ::upic_set_param(tr+0,C_TMASK0,"%8x",*(tr+0),0,0,0,0,0);
  ::upic_add_command(C_TMASK0,"Trigger mask [0] 0X^^^^^^^^","");
  ::upic_set_param(tr+1,C_TMASK1,"%8x",*(tr+1),0,0,0,0,0);
  ::upic_add_command(C_TMASK1,"Trigger mask [1] 0X^^^^^^^^","");
  ::upic_set_param(tr+2,C_TMASK2,"%8x",*(tr+2),0,0,0,0,0);
  ::upic_add_command(C_TMASK2,"Trigger mask [2] 0X^^^^^^^^","");
  ::upic_set_param(tr+3,C_TMASK3,"%8x",*(tr+3),0,0,0,0,0);
  ::upic_add_command(C_TMASK3,"Trigger mask [3] 0X^^^^^^^^","");
  ::upic_set_param(vt+0,C_VETO0,"%8x",*(vt+0),0,0,0,0,0);
  ::upic_add_command(C_VETO0, "Veto    mask [0] 0X^^^^^^^^","");
  ::upic_set_param(vt+1,C_VETO1,"%8x",*(vt+1),0,0,0,0,0);
  ::upic_add_command(C_VETO1, "Veto    mask [1] 0X^^^^^^^^","");
  ::upic_set_param(vt+2,C_VETO2,"%8x",*(vt+2),0,0,0,0,0);
  ::upic_add_command(C_VETO2, "Veto    mask [2] 0X^^^^^^^^","");
  ::upic_set_param(vt+3,C_VETO3,"%8x",*(vt+3),0,0,0,0,0);
  ::upic_add_command(C_VETO3, "Veto    mask [3] 0X^^^^^^^^","");
  ::upic_set_param(m_rtype_c,C_RTYPE,"%3s",rtype_list[0],0,0,rtype_list,2,1);
  ::upic_add_command(C_RTYPE,"Requirement type    ^^^"," ");
  ::upic_set_param(m_mode_c,C_MODE,"%7s",mode_list[0],0,0,mode_list,3,1);
  ::upic_add_command(C_MODE,"Mode            ^^^^^^^"," ");
  ::upic_enable_action_routine(id(),C_ADD,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_EVT,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TMASK0,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TMASK1,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TMASK2,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TMASK3,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_VETO0, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_VETO1, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_VETO2, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_VETO3, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_RTYPE, Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_MODE,  Routine(BaseMenu::dispatch));
  if(  pg < BM_MAX_REQS )   {
    ::upic_add_command(C_PTO,"PTO","Make a new page");
    ::upic_enable_action_routine(id(),C_PTO, Routine(BaseMenu::dispatch));
  } 
  ::upic_close_menu();
}

void Requirement::handleMenu(int cmd_id)   {
  switch(cmd_id){
    case C_ADD:
      m_reqTyp  = (0==strcmp(m_rtype_c,rtype_list[0])) ? BM_MASK_ANY : BM_MASK_ALL;
      m_reqMode = (0==strcmp(m_mode_c,mode_list[0]))   ? BM_REQ_USER
        : (0==strcmp(m_mode_c,mode_list[1])) ? BM_REQ_ALL : BM_REQ_ONE;
      m_reqActive ? remove() : add();
      break;
    case C_EVT:
    case C_TMASK0:
    case C_TMASK1:
    case C_TMASK2:
    case C_TMASK3:
    case C_VETO0:
    case C_VETO1:
    case C_VETO2:
    case C_VETO3:
    case C_RTYPE:
      ::upic_set_cursor(id(),cmd_id+1,cmd_id+1);
      break;
    case C_MODE:
      ::upic_set_cursor(id(),C_ADD,0);
      break; 
    case C_PTO:
      ::upic_set_cursor(id()+1,C_ADD,1);
      break;   
  }
}

void Requirement::add()  {
  char msg[132];
  int status = MBM_ERROR;
  if(!m_reqActive){
    status = mbm_add_req(m_bmID,m_evType,
                         m_trMask.bits(),m_veto.bits(),
                         m_reqTyp,m_reqMode,BM_FREQ_PERC,100.0);
    switch(status){
    case MBM_NORMAL:
      m_reqActive = true;
      ::upic_replace_command(id(),C_ADD,"Remove requirement","");
      upic_disable_commands(id(),11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
                          C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
      ::sprintf(msg,"Requirement added: Event type:%03d Req type:%s Mode:%s",
        m_evType,rtype_list[m_reqTyp],mode_list[m_reqMode]);
      ::upic_write_message(msg,"");
      ::sprintf(msg,"  -->Trigger mask: %08X %08X %08X %08X",
        m_trMask.word(0),m_trMask.word(1),m_trMask.word(2),m_trMask.word(3));
      ::upic_write_message(msg,"");
      ::sprintf(msg,"  -->Veto    mask: %08X %08X %08X %08X",
        m_veto.word(0),m_veto.word(1),m_veto.word(2),m_veto.word(3));
      ::upic_write_message(msg,"");
      return;
    default:
      ::upic_write_message("Cannot add requirement added","");
      return;
    }
  }
  ::upic_write_message("[Internal Error] requirement not active!","");
}

void Requirement::remove()  {
  char msg[132];
  int status = MBM_ERROR;
  if(m_reqActive){
    status = mbm_del_req(m_bmID,m_evType,m_trMask.bits(),m_veto.bits(),m_reqTyp,m_reqMode);
    switch(status){
    case MBM_NORMAL:
      m_reqActive = false;
      ::upic_replace_command(id(),C_ADD,"Add requirement","");
      upic_enable_commands(id(),11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
                            C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
      ::sprintf(msg,"Requirement removed: Event type:%03d Req type:%s Mode:%s",
        m_evType,rtype_list[m_reqTyp],mode_list[m_reqMode]);
      ::upic_write_message(msg,"");
      ::sprintf(msg,"  -->Trigger mask: %08X %08X %08X %08X",
        m_trMask.word(0),m_trMask.word(1),m_trMask.word(2),m_trMask.word(3));
      ::upic_write_message(msg,"");
      ::sprintf(msg,"  -->Veto    mask: %08X %08X %08X %08X",
        m_veto.word(0),m_veto.word(1),m_veto.word(2),m_veto.word(3));
      ::upic_write_message(msg,"");
      return;
    case MBM_ILL_REQ:  // In this case toggle anyway to free page
      m_reqActive = false;
      ::upic_write_message("Requirement was non existent","");
      return;
    default:
      ::upic_write_message("Failed to remove requirement","");
      return;
    }
  }
  ::upic_write_message("[Internal Error] requirement already active!","");
}

