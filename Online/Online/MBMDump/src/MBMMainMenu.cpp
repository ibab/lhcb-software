// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/MBMMainMenu.cpp,v 1.8 2009-10-21 17:10:33 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: MBMMainMenu.cpp,v 1.8 2009-10-21 17:10:33 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"

using namespace MBMDump;

static const char* Unknown_type = "Unknown";
static const char* mep_type = "MEP";
static const char* raw_type = "RawEvent";
static const char* dsc_type = "Descriptor";
static const char* mdf_type = "MDF";

static const char* buff_types[]={Unknown_type,mep_type,raw_type,dsc_type,mdf_type};
static const char* buff_names[]={"MEP","EVENT","RESULT","SEND","Events_","OUTPUT","RAW","0","1","2","3","4"};

MBMMainMenu::MBMMainMenu() 
: m_bmID(MBM_INV_DESC), m_partID(0x14d), m_memory(0), m_dispMenu(0)
{
  int num_types = sizeof(buff_types)/sizeof(buff_types[0]);
  int num_names = sizeof(buff_names)/sizeof(buff_names[0]);

  ::snprintf(m_name,sizeof(m_name)-1,"%s",procName());
  ::snprintf(m_buffName,sizeof(m_buffName)-1,"%s",buff_names[4]);
  ::snprintf(m_buffType,sizeof(m_buffType)-1,"%s",buff_types[1]);
  openMenu  (0,0,"Event Dump","General purpose MBM Dump",procName()); 
  setParam(m_name,    1,"%16s",procName(),0,0,0,0,0);
  addCommand(C_PROC,     "Process name :  ^^^^^^^^^^^^^^^^");
  setParam(&m_partID, 2,"%6X",0x7FFF,0,0XFFFFFF,0,0,0);
  addCommand(C_PART,     "Partition ID :     0X^^^^^^     ");
  setParam(m_buffName,3,"%18s",buff_names[4],0,0,buff_names,num_names,0);
  addCommand(C_BUF,      "Buffer name  :^^^^^^^^^^^^^^^^^^");
  setParam(m_buffType, 4,"%18s",buff_types[1],0,0,buff_types,num_types,1);
  addCommand(C_TYP,      "Buffer type  :^^^^^^^^^^^^^^^^^^");
  addCommand(C_INC_EXC,  "Include process                 ");
  addComment(C_COM1,     "                                ");
  addComment(C_COM2,     "********************************");
  addComment(C_COM3,     "                                ");
  addCommand(C_RQS,      "Set Requirements                ");
  addCommand(C_CMD,      "Display menu                    ");
  addComment(C_COM4,     "                                ");
  addComment(C_COM5,     "********************************");
  addComment(C_COM6,     "                                ");
  //addCommand(C_DEBUG,    "Debug                           ");
  addCommand(C_EXIT,     "Exit                            ");
  closeMenu();
  enableCommands(3,C_PROC,C_PART,C_BUF,C_TYP);
  disableCommands(3,C_RQS,C_INC_EXC,C_CMD);
  m_req[0].build(0,id(),C_RQS);
  for(int i=1; i<8; ++i)
    m_req[i].build(i,m_req[i-1].id(),Requirement::C_PTO);
  openOldWindow();          // Set cursor back on menu window
  m_dispMenu = new DisplayMenu(this,C_CMD);
  setCursor(C_PROC,1);      // set cursor at top
}

MBMMainMenu::~MBMMainMenu() {
  if ( m_memory ) delete [] m_memory;
  if ( m_dispMenu ) delete m_dispMenu;
}

int MBMMainMenu::includeMBM() {
  if( m_bmID == MBM_INV_DESC )   {
    m_bmID = mbm_include(m_buffName,m_name,m_partID);
    if(m_bmID != MBM_INV_DESC){
      replaceCommand(C_INC_EXC,"Exclude process");
      enableCommands(2, C_RQS,  C_CMD);
      disableCommands(3, C_PROC, C_PART, C_BUF, C_TYP);
      setCursor(C_RQS,1);
      output("Process %s included into buffer %s",m_name,m_buffName);
      return MBM_NORMAL;
    }
    output("Failed to include process %s into buffer %s",m_name,m_buffName);
    return MBM_ERROR;
  }
  output("Process already included in MBM");
  return MBM_ERROR;
}

int MBMMainMenu::excludeMBM() {
  if( m_bmID != MBM_INV_DESC )   {
    int status = ::mbm_exclude(m_bmID);     // Try to exclude from the buffer
    switch(status){
    case MBM_NORMAL:
      m_bmID = MBM_INV_DESC;
      disableCommands(2, C_RQS,  C_CMD);
      enableCommands(3, C_PROC, C_PART, C_BUF, C_TYP);
      replaceCommand(C_INC_EXC,"Include process");
      output("Process excluded from buffer manager");
      return MBM_NORMAL;
    default:
      output("Failed to exclude : Unknown error");
      return status;
    }
  }
  return MBM_ERROR;
}

void MBMMainMenu::handleMenu(int cmd_id)    {
  char* ptr;
  bool  including = false;
  int b_type = DisplayMenu::B_UNKNOWN;
  
  switch(cmd_id){
  case C_PROC:
    setCursor(C_PART,2);
    return;
  case C_PART:
    setCursor(C_BUF,3);
    return;
  case C_BUF:
    m_buffName[sizeof(m_buffName)-1] = 0;
    if ( (ptr=::strchr(m_buffName,' ')) ) *ptr = 0;
    setCursor(C_TYP,4);
    return;
  case C_TYP:
    enableCommands(1,C_INC_EXC);
    setCursor(C_INC_EXC,1);
    return;
  case C_INC_EXC: // Toggle buffer inclusion/exclusion
    m_name[sizeof(m_name)-1] = 0;
    m_buffName[sizeof(m_buffName)-1] = 0;
    m_buffType[sizeof(m_buffType)-1] = 0;
    if ( (ptr=::strchr(m_name,' ')) ) *ptr = 0;
    if ( (ptr=::strchr(m_buffName,' ')) ) *ptr = 0;
    if ( (ptr=::strchr(m_buffType,' ')) ) *ptr = 0;
    if (m_bmID == MBM_INV_DESC) {
      includeMBM();
      including=true;
    }
    else  {
      excludeMBM();
      including=false;
    }
    setCursor(m_bmID == MBM_INV_DESC ? C_PART : C_RQS,1);
    for(int i=0; i<8; ++i) m_req[i].setBufferID(m_bmID);
    if ( (ptr=::strchr(m_buffType,' ')) ) *ptr = 0;
    if ( !including )                          b_type = DisplayMenu::B_UNKNOWN;
    else if ( !::strcmp(m_buffType,mep_type) ) b_type = DisplayMenu::B_MEP;
    else if ( !::strcmp(m_buffType,raw_type) ) b_type = DisplayMenu::B_RAW;
    else if ( !::strcmp(m_buffType,dsc_type) ) b_type = DisplayMenu::B_DESC;
    else if ( !::strcmp(m_buffType,mdf_type) ) b_type = DisplayMenu::B_MDF;
    else                                       b_type = DisplayMenu::B_UNKNOWN;
    output("Show display menu... %s buffer B type:%d",including ? "included in" : "excluded from",int(b_type));
    m_dispMenu->update(b_type);
    break;
  case C_RQS:
    m_req[0].setCursor(Requirement::C_ADD);
    break;
  case C_CMD:
    m_dispMenu->show();
    break;
  case C_DEBUG:
    output("Starting debugger....");
    ::lib_rtl_start_debugger();
    break;
  case C_EXIT:
    if ( m_bmID != MBM_INV_DESC ) ::mbm_exclude(m_bmID);
    ::exit(quit());
    break; 
  }
}

int MBMMainMenu::getEvent(struct DataBlock *event)    {
  int *p, len, evtyp, partID=0, status;
  unsigned int tr[4];
  // returned length is in bytes...div by 4 to get ints !!!
  output("Wait for next event (blocking)...");
  status = ::mbm_get_event(m_bmID,&p,&len,&evtyp,tr,partID);  
  switch(status){
    case MBM_NORMAL:
      if(m_memory){                 // free any previously reserved memory
        delete [] m_memory;
        m_memory = 0;
      }
      m_memory = new int[len/sizeof(int)+10]; // reserve memory
      if(m_memory != 0)    {
        ::memcpy(m_memory,p,len);
        event->start = m_memory;
        mbm_pause(m_bmID);          // maybe should be a call to bm_pause
      }
      else    {
        event->start = p;
      }
      ::memcpy(event->mask,tr,sizeof(tr));    // trigger type is in. name
      event->number = evtyp; // event type is in .number
      event->length = len/sizeof(int);
      break;
    case MBM_INACTIVE:
      output("Failed to get event : Buffer manager inactive");
      break;
    case MBM_REQ_CANCEL:
      output("Failed to get event : Request cancelled");
      break;
  }
  return status;
}
