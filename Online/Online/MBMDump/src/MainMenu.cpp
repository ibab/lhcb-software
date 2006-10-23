#include "MBMDump/MBMDump.h"
#include "UPI/upidef.h"

using namespace MBMDump;

static char* Unknown_type = "Unknown";
static char* mep_type = "MEP";
static char* raw_type = "RawEvent";
static char* dsc_type = "Descriptor";
static char* mdf_type = "MDF";

static char* buff_types[]={Unknown_type,mep_type,raw_type,dsc_type,mdf_type};
static char* buff_names[]={"MEP","EVENT","RESULT","OUTPUT","RAW","0","1","2","3","4"};

MainMenu::MainMenu() 
: m_bmID(MBM_INV_DESC), m_mepID(MEP_INV_DESC), 
  m_partID(0x14d), m_memory(0), m_dispMenu(0)
{
}

MainMenu::~MainMenu() {
  if ( m_memory ) delete [] m_memory;
  if ( m_dispMenu ) delete m_dispMenu;
}

void MainMenu::build()  {
  int num_types = sizeof(buff_types)/sizeof(buff_types[0]);
  int num_names = sizeof(buff_names)/sizeof(buff_names[0]);

  strcpy(m_name,procName());
  strcpy(m_buffName,buff_names[0]);
  strcpy(m_buffType,buff_types[0]);
  ::upic_open_menu  (id(),0,0,"Event Dump","General purpose MBM Dump",procName()); 
  ::upic_set_param  (m_name,    1,"%8s",procName(),0,0,0,0,0);
  ::upic_add_command(C_PROC,     "Process name :  ^^^^^^^^",      "");
  ::upic_set_param  (&m_partID, 2,"%3X",0x14d,0,0XFFF,0,0,0);
  ::upic_add_command(C_PART,     "Partition ID :     0X^^^",      "");
  ::upic_set_param  (m_buffName,3,"%9s",buff_names[0],0,0,buff_names,num_names,0);
  ::upic_add_command(C_BUF,      "Buffer name  : ^^^^^^^^^",      "");
  ::upic_set_param  (m_buffType, 4,"%10s",buff_types[0],0,0,buff_types,num_types,1);
  ::upic_add_command(C_TYP,      "Buffer type  :^^^^^^^^^^",      "");
  ::upic_add_command(C_INC_EXC,  "Include process         ",      "");
  ::upic_add_comment(C_COM1,     "                        ",      "");
  ::upic_add_comment(C_COM2,     "************************",      "");
  ::upic_add_comment(C_COM3,     "                        ",      "");
  ::upic_add_command(C_RQS,      "Set Requirements        ",      "");
  ::upic_add_command(C_CMD,      "Display menu            ",      "");
  ::upic_add_comment(C_COM4,     "                        ",      "");
  ::upic_add_comment(C_COM5,     "************************",      "");
  ::upic_add_comment(C_COM6,     "                        ",      "");
  //::upic_add_command(C_DEBUG,    "Debug                   ",      "");
  ::upic_add_command(C_EXIT,     "Exit                    ",      "");
  ::upic_enable_action_routine(id(),C_PROC,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_PART,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_BUF,    Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_TYP,    Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_INC_EXC,Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_RQS,    Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_CMD,    Routine(BaseMenu::dispatch)); 
  ::upic_enable_action_routine(id(),C_EXIT,   Routine(BaseMenu::dispatch));
  ::upic_enable_action_routine(id(),C_DEBUG,  Routine(BaseMenu::dispatch));
  ::upic_close_menu();
  ::upic_enable_commands (id(),3,C_PROC,C_PART,C_BUF);
  ::upic_disable_commands(id(),3,C_RQS,C_INC_EXC,C_CMD);
  m_req[0].build(0,id(),C_RQS);
  for(int i=1; i<8; ++i)
    m_req[i].build(i,m_req[i-1].id(),Requirement::C_PTO);
  ::upic_open_old_window(id());          // Set cursor back on menu window
  m_dispMenu = new DisplayMenu(this,id(),C_CMD);
  ::upic_set_cursor(id(),C_PROC,1);      // set cursor at top
}

int MainMenu::includeMBM() {
  if( m_bmID == MBM_INV_DESC )   {
    m_bmID = mbm_include(m_buffName,m_name,m_partID);
    if(m_bmID != MBM_INV_DESC){
      ::upic_replace_command(id(),C_INC_EXC,"Exclude process","");
      ::upic_enable_commands (id(),2, C_RQS,  C_CMD);
      ::upic_disable_commands(id(),3, C_PROC, C_PART, C_BUF, C_TYP);
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

int MainMenu::includeMEP() {
  m_bmID = MBM_INV_DESC;
  if( m_mepID == MEP_INV_DESC )   {
    m_mepID = ::mep_include(m_name,m_partID, m_mepFlags);
    if( m_mepID != MEP_INV_DESC ){
      if ( m_mepFlags == USE_EVT_BUFFER ) m_bmID = m_mepID->evtBuffer;
      if ( m_mepFlags == USE_RES_BUFFER ) m_bmID = m_mepID->resBuffer;
      ::upic_replace_command(id(),C_INC_EXC,"Exclude process","");
      ::upic_enable_commands (id(),2, C_RQS,  C_CMD);
      ::upic_disable_commands(id(),3, C_PROC, C_PART, C_BUF, C_TYP);
      ::upic_set_cursor(id(),C_RQS,1);
      ::upic_write_message2("Process %s included into buffer %s",m_name,m_buffName);
      ::upic_write_message2(" MEP    buffer start: %p",m_mepID->mepStart);
      ::upic_write_message2(" EVENT  buffer start: %p",m_mepID->evtStart);
      ::upic_write_message2(" RESULT buffer start: %p",m_mepID->resStart);
      return MBM_NORMAL;
    }
    ::upic_write_message2("Failed to include process %s into buffer %s",m_name,m_buffName);
    return MBM_ERROR;
  }
  ::upic_write_message2("Process already included in MBM");
  return MBM_ERROR;
}

int MainMenu::excludeMBM() {
  if( m_bmID != MBM_INV_DESC )   {
    int status = mbm_exclude(m_bmID);     // Try to exclude from the buffer
    switch(status){
    case MBM_NORMAL:
      m_bmID = MBM_INV_DESC;
      ::upic_disable_commands(id(),2, C_RQS,  C_CMD);
      ::upic_enable_commands (id(),3, C_PROC, C_PART, C_BUF, C_TYP);
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

int MainMenu::excludeMEP() {
  if( m_mepID != MEP_INV_DESC )   {
    int status = mep_exclude(m_mepID);
    switch(status){
    case MBM_NORMAL:
      m_bmID = MBM_INV_DESC;
      m_mepID = MEP_INV_DESC;
      ::upic_disable_commands(id(),2, C_RQS,  C_CMD);
      ::upic_enable_commands (id(),3, C_PROC, C_PART, C_BUF, C_TYP);
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
  int b_type = DisplayMenu::B_UNKNOWN;
  switch(cmd_id){
  case C_PROC:
    ::upic_set_cursor(id(),C_PART,2);
    return;
  case C_PART:
    ::upic_set_cursor(id(),C_BUF,3);
    return;
  case C_BUF:
    m_mepFlags = 0;
    m_buffName[sizeof(m_buffName)-1] = 0;
    if ( (ptr=::strchr(m_buffName,' ')) ) *ptr = 0;
    if ( !::strcmp(m_buffName,"EVENT")  ) m_mepFlags = USE_EVT_BUFFER;
    if ( !::strcmp(m_buffName,"RESULT") ) m_mepFlags = USE_RES_BUFFER;
    ::upic_set_cursor(id(),C_TYP,4);
    return;
  case C_TYP:
    ::upic_enable_commands(id(),1,C_INC_EXC);
    ::upic_set_cursor(id(),C_INC_EXC,1);
    return;
  case C_INC_EXC: // Toggle buffer inclusion/exclusion
    m_name[sizeof(m_name)-1] = 0;
    m_buffName[sizeof(m_buffName)-1] = 0;
    m_buffType[sizeof(m_buffType)-1] = 0;
    if ( (ptr=strchr(m_name,' ')) ) *ptr = 0;
    if ( (ptr=strchr(m_buffName,' ')) ) *ptr = 0;
    if ( (ptr=strchr(m_buffType,' ')) ) *ptr = 0;
    if ( m_mepFlags == 0 )  {
      (m_bmID == MBM_INV_DESC) ? includeMBM() : excludeMBM();
    }
    else  {
      (m_mepID == MEP_INV_DESC) ? includeMEP() : excludeMEP();
    }
    ::upic_set_cursor(id(),m_bmID == MBM_INV_DESC ? C_PART : C_RQS,1);
    for(int i=0; i<8; ++i) m_req[i].setBufferID(m_bmID);
    ::upic_write_message("Show display menu...","");
    if ( (ptr=strchr(m_buffType,' ')) ) *ptr = 0;
    if      ( !strcmp(m_buffType,mep_type) ) b_type = DisplayMenu::B_MEP;
    else if ( !strcmp(m_buffType,raw_type) ) b_type = DisplayMenu::B_RAW;
    else if ( !strcmp(m_buffType,dsc_type) ) b_type = DisplayMenu::B_DESC;
    else if ( !strcmp(m_buffType,mdf_type) ) b_type = DisplayMenu::B_MDF;
    else                                     b_type = DisplayMenu::B_UNKNOWN;
    m_dispMenu->update(b_type);
    break;
  case C_RQS:
    ::upic_set_cursor(m_req[0].id(),Requirement::C_ADD,1);
    break;
  case C_CMD:
    m_dispMenu->show();
    break;
  case C_DEBUG:
    upic_write_message2("Starting debugger....");
    ::lib_rtl_start_debugger();
    break;
  case C_EXIT:
    if ( m_mepID != MEP_INV_DESC ) mep_exclude(m_mepID);
    else if ( m_bmID != MBM_INV_DESC ) mbm_exclude(m_bmID);
    ::exit(::upic_quit() != UPI_NORMAL ? EXIT_FAILURE : EXIT_SUCCESS);
    break; 
  }
}

int MainMenu::getEvent(struct DataBlock *event)    {
  int *p, len, evtyp, partID=0, status;
  static int evt = 0;
  unsigned int tr[4];
  // returned length is in bytes...div by 4 to get ints !!!
  status = ::mbm_get_event(m_bmID,&p,&len,&evtyp,tr,partID);  
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
