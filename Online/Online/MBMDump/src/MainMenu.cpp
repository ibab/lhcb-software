#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace MBMDump;

MainMenu::MainMenu() 
: m_bmID(MBM_INV_DESC), m_partID(0x14d), m_memory(0)
{
}

MainMenu::~MainMenu() {
  if ( m_memory ) delete [] m_memory;
}

void MainMenu::buildMenu()  {
  char* s_list[]={"MEP","EVENT","RESULT","OUTPUT","RAW","0","1","2","3","4"};
  strcpy(m_buffName,s_list[0]);
  strcpy(m_name,"EvtDump");
  ::upic_open_menu  (id(),0,0,"Event Dump","General purpose MBM Dump",procName()); 
  ::upic_set_param  (m_name,    1,"%8s","EvtDump",0,0,0,0,0);
  ::upic_add_command(C_PROC,     "Process name : ^^^^^^^^",      "");
  ::upic_set_param  (&m_partID, 2,"%3X",0x14d,0,0XFFF,0,0,0);
  ::upic_add_command(C_PART,     "Partition ID :    0X^^^",      "");
  ::upic_set_param  (m_buffName,3,"%9s",s_list[0],0,0,s_list,10,1);
  ::upic_add_command(C_BUF,      "Buffer name  :^^^^^^^^^",      "");
  ::upic_add_command(C_INC_EXC,  "Include process        ",      "");
  ::upic_add_comment(72,         "                       ",      "");
  ::upic_add_comment(73,         "***********************",      "");
  ::upic_add_comment(74,         "                       ",      "");
  ::upic_add_command(C_RQS,      "Set Requirements       ",      "");
  ::upic_add_command(C_CMD,      "Display menu           ",      "");
  ::upic_add_comment(75,         "                       ",      "");
  ::upic_add_comment(76,         "***********************",      "");
  ::upic_add_comment(77,         "                       ",      "");
  //::upic_add_command(C_DEBUG,    "Debug                  ",      "");
  ::upic_add_command(C_EXIT,     "Exit                   ",      "");
  ::upic_enable_action_routine(id(),C_PROC,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_PART,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_BUF,    Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_INC_EXC,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_RQS,    Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_CMD,    Routine(BaseMenu::dispatch)); 
  ::upic_enable_action_routine(id(),C_EXIT,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_DEBUG,  Routine(BaseMenu::dispatch));
  ::upic_close_menu();
  upic_enable_commands (id(),3,C_PROC,C_PART,C_BUF);
  upic_disable_commands(id(),3,C_RQS,C_INC_EXC,C_CMD);
  m_req[0].buildMenu(0,id(),C_RQS);
  for(int i=1; i<8; ++i)
    m_req[i].buildMenu(i,m_req[i-1].id(),Requirement::C_PTO);
  m_dispMenu.buildMenu(this,id(),C_CMD); // Build link to display menu
  ::upic_open_old_window(id());          // Set cursor back on menu window
  ::upic_set_cursor(id(),C_PROC,1);      // set cursor at top
}

int MainMenu::include() {
  if( m_bmID == MBM_INV_DESC )   {
    m_bmID = mbm_include(m_buffName,m_name,m_partID);
    if(m_bmID != MBM_INV_DESC){
      ::upic_replace_command(id(),C_INC_EXC,"Exclude process","");
      upic_enable_commands (id(),2, C_RQS,  C_CMD);
      upic_disable_commands(id(),3, C_PROC, C_PART, C_BUF);
      ::upic_set_cursor(id(),C_RQS,1);
      ::upic_write_message2("Process %s included into buffer %s",m_name,m_buffName);
      return MBM_NORMAL;
    }
    ::upic_write_message2("Failed to include process %s into buffer %s",m_name,m_buffName);
    return MBM_ERROR;
  }
  ::upic_write_message2("Process already included in MBM");
  return MBM_ERROR;
}

int MainMenu::exclude() {
  if( m_bmID != MBM_INV_DESC )   {
    int status = mbm_exclude(m_bmID);     // Try to exclude from the buffer
    switch(status){
    case MBM_NORMAL:
      m_bmID = MBM_INV_DESC;
      upic_disable_commands(id(),2, C_RQS,  C_CMD);
      upic_enable_commands (id(),3, C_PROC, C_PART, C_BUF);
      ::upic_replace_command(id(),C_INC_EXC,"Include process","");
      ::upic_write_message2("Process excluded from buffer manager");
      return MBM_NORMAL;
    default:
      ::upic_write_message2("Failed to exclude : Unknown error");
      return status;
    }
  }
  return MBM_ERROR;
}

void MainMenu::handleMenu(int cmd_id)    {
  char* ptr;
  switch(cmd_id){
  case C_PROC:
    ::upic_set_cursor(id(),C_PART,2);
    break;
  case C_PART:
    ::upic_set_cursor(id(),C_BUF,3);
    break;
  case C_BUF:
    upic_enable_commands(id(),1,C_INC_EXC);
    ::upic_set_cursor(id(),C_INC_EXC,1);
    break;
  case C_INC_EXC: // Toggle buffer inclusion/exclusion
    m_name[sizeof(m_name)-1] = 0;
    m_buffName[sizeof(m_buffName)-1] = 0;
    if ( (ptr=strchr(m_name,' ')) ) *ptr = 0;
    if ( (ptr=strchr(m_buffName,' ')) ) *ptr = 0;
    (m_bmID == MBM_INV_DESC) ? include() : exclude();
    for(int i=0; i<8; ++i) m_req[i].m_bmID = m_bmID;
    break;
  case C_RQS:
    ::upic_set_cursor(m_req[0].id(),Requirement::C_ADD,1);
    break;
  case C_DEBUG:
    upic_write_message2("Starting debugger....");
    ::lib_rtl_start_debugger();
    break;
  case C_EXIT:
    if( m_bmID != MBM_INV_DESC ) mbm_exclude(m_bmID);
    ::exit(::upic_quit() != UPI_NORMAL ? EXIT_FAILURE : EXIT_SUCCESS);
    break; 
  }
}

int MainMenu::getEvent(struct DataBlock *event)    {
  int *p, len, evtyp, partID=0, status;
  static int evt = 0;
  unsigned int tr[4];
  // returned length is in bytes...div by 4 to get ints !!!
  status = mbm_get_event(m_bmID,&p,&len,&evtyp,tr,partID);  
  switch(status){
    case MBM_NORMAL:
      if(m_memory){                 // free any previously reserved memory
        delete [] m_memory;
        m_memory = 0;
      }
      m_memory = new int[len/4+10]; // reserve memory
      if(m_memory != 0)    {
        memcpy(m_memory,p,len);
        event->start = m_memory;
        mbm_pause(m_bmID);          // maybe should be a call to bm_pause
      }
      else    {
        event->start = p;
      }
      memcpy(event->name,tr,sizeof(tr));    // trigger type is in. name
      evt++;
      event->number = evtyp; // event type is in .number
      event->length = len/4;
      upic_write_message2("Got event....   %d",evt);
      break;
    case MBM_INACTIVE:
      upic_write_message2("Failed to get event : Buffer manager inactive");
      break;
    case MBM_REQ_CANCEL:
      upic_write_message2("Failed to get event : Request cancelled");
      break;
  }
  return status;
}
