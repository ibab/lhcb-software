// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/Requirement.cpp,v 1.7 2010-09-07 13:50:43 frankb Exp $
//  ====================================================================
//  Requirements.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Requirement.cpp,v 1.7 2010-09-07 13:50:43 frankb Exp $
// C++ include files
#include <string>

// Framework include files
#include "Event/RawBank.h"
#include "MBMDump/MBMDump.h"

using namespace LHCb;
using namespace MBMDump;

static const char *rtype_list[] = {"ANY", "ALL"};
static const char *mode_list[]  = {"NOT ALL", "    ALL", "    ONE" };

Requirement::Requirement() 
: m_reqActive(false),m_bmID(MBM_INV_DESC),m_evType(2),m_trMask(~0x0),m_veto(0),
  m_reqTyp(BM_MASK_ANY), m_reqMode(BM_NOTALL)
{
  ::snprintf(m_mode_c,sizeof(m_mode_c),"%s",mode_list[0]);
  ::snprintf(m_rtype_c,sizeof(m_rtype_c),"%s",rtype_list[0]);
}

void Requirement::build(int pg, int menu_id, int cmd_id)   {
  char title[20] = "Requirement no. 1";
  unsigned int* tr = (unsigned int*)m_trMask.bits();
  unsigned int* vt = (unsigned int*)m_veto.bits();
  title[16] = char('1'+pg);
  openMenu(menu_id,cmd_id,title,"",procName());
  addCommand(C_ADD,"Add Requirement");
  setParam(&m_evType,C_EVT,"%2d",2,0,10,0,0,0);
  addCommand(C_EVT,"Event type           ^^");
  setParam(tr+0,C_TMASK0,"%8x",*(tr+0),0,0,0,0,0);
  addCommand(C_TMASK0,"Trigger mask [0] 0X^^^^^^^^");
  setParam(tr+1,C_TMASK1,"%8x",*(tr+1),0,0,0,0,0);
  addCommand(C_TMASK1,"Trigger mask [1] 0X^^^^^^^^");
  setParam(tr+2,C_TMASK2,"%8x",*(tr+2),0,0,0,0,0);
  addCommand(C_TMASK2,"Trigger mask [2] 0X^^^^^^^^");
  setParam(tr+3,C_TMASK3,"%8x",*(tr+3),0,0,0,0,0);
  addCommand(C_TMASK3,"Trigger mask [3] 0X^^^^^^^^");
  setParam(vt+0,C_VETO0,"%8x",*(vt+0),0,0,0,0,0);
  addCommand(C_VETO0, "Veto    mask [0] 0X^^^^^^^^");
  setParam(vt+1,C_VETO1,"%8x",*(vt+1),0,0,0,0,0);
  addCommand(C_VETO1, "Veto    mask [1] 0X^^^^^^^^");
  setParam(vt+2,C_VETO2,"%8x",*(vt+2),0,0,0,0,0);
  addCommand(C_VETO2, "Veto    mask [2] 0X^^^^^^^^");
  setParam(vt+3,C_VETO3,"%8x",*(vt+3),0,0,0,0,0);
  addCommand(C_VETO3, "Veto    mask [3] 0X^^^^^^^^");
  setParam(m_rtype_c,C_RTYPE,"%3s",rtype_list[0],0,0,rtype_list,2,1);
  addCommand(C_RTYPE,"Requirement type    ^^^");
  setParam(m_mode_c,C_MODE,"%7s",mode_list[0],0,0,mode_list,3,1);
  addCommand(C_MODE,"Mode            ^^^^^^^");
  if(  pg < BM_MAX_REQS )   {
    addCommand(C_PTO,"New page");
  } 
  closeMenu();
}

void Requirement::handleMenu(int cmd_id)   {
  switch(cmd_id){
    case C_ADD:
      m_reqTyp  = (0==::strcmp(m_rtype_c,rtype_list[0])) ? BM_MASK_ANY : BM_MASK_ALL;
      m_reqMode = (0==::strcmp(m_mode_c,mode_list[0]))   ? BM_REQ_USER
        : (0==::strcmp(m_mode_c,mode_list[1])) ? BM_REQ_ALL : BM_REQ_ONE;
      m_reqActive ? this->remove() : this->add();
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
      setCursor(cmd_id+1,cmd_id+1);
      break;
    case C_MODE:
      setCursor(C_ADD,0);
      break; 
    case C_PTO:
      setCursor(id()+1,C_ADD,1);
      break;   
  }
}

void Requirement::add()  {
  int status = MBM_ERROR;
  if(!m_reqActive){
    status = ::mbm_add_req(m_bmID,m_evType,
                           m_trMask.bits(),m_veto.bits(),
                           m_reqTyp,m_reqMode,BM_FREQ_PERC,100.0);
    switch(status){
    case MBM_NORMAL:
      m_reqActive = true;
      replaceCommand(C_ADD,"Remove requirement");
      disableCommands(11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
                          C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
      output("Requirement added: Event type:%03d Req type:%s Mode:%s",
        m_evType,rtype_list[m_reqTyp],mode_list[m_reqMode]);
      output("  -->Trigger mask: %08X %08X %08X %08X",
        m_trMask.word(0),m_trMask.word(1),m_trMask.word(2),m_trMask.word(3));
      output("  -->Veto    mask: %08X %08X %08X %08X",
        m_veto.word(0),m_veto.word(1),m_veto.word(2),m_veto.word(3));
      return;
    default:
      output("Cannot add requirement added");
      return;
    }
  }
  output("[Internal Error] requirement not active!");
}

void Requirement::setBufferID(BMID bm)  {
  m_bmID = bm;
  if ( (m_reqActive=(m_bmID==MBM_INV_DESC)) )  {
    replaceCommand(C_ADD,"Remove requirement");
    disableCommands(11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
                        C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
  }
  else {
    replaceCommand(C_ADD,"Add requirement");
    enableCommands(11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
      C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
  }
}

void Requirement::remove()  {
  int status = MBM_ERROR;
  if(m_reqActive){
    status = ::mbm_del_req(m_bmID,m_evType,m_trMask.bits(),m_veto.bits(),m_reqTyp,m_reqMode);
    switch(status){
    case MBM_NORMAL:
      m_reqActive = false;
      replaceCommand(C_ADD,"Add requirement");
      enableCommands(11,C_EVT,C_TMASK0,C_TMASK1,C_TMASK2,C_TMASK3,
        C_VETO0,C_VETO1,C_VETO2,C_VETO3,C_RTYPE,C_MODE);
      output("Requirement removed: Event type:%03d Req type:%s Mode:%s",
        m_evType,rtype_list[m_reqTyp],mode_list[m_reqMode]);
      output("  -->Trigger mask: %08X %08X %08X %08X",
        m_trMask.word(0),m_trMask.word(1),m_trMask.word(2),m_trMask.word(3));
      output("  -->Veto    mask: %08X %08X %08X %08X",
        m_veto.word(0),m_veto.word(1),m_veto.word(2),m_veto.word(3));
      return;
    case MBM_ILL_REQ:  // In this case toggle anyway to free page
      m_reqActive = false;
      output("Requirement was non existent");
      return;
    default:
      output("Failed to remove requirement");
      return;
    }
  }
  output("[Internal Error] requirement already active!");
}
