// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/MBMDump/src/FileMainMenu.cpp,v 1.6 2008-09-26 09:51:36 frankb Exp $
//  ====================================================================
//  BankListWindow.cpp
//  --------------------------------------------------------------------
//
//  Package   : MBMDump: Dump utility for MBM buffer events
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: FileMainMenu.cpp,v 1.6 2008-09-26 09:51:36 frankb Exp $
//
// C++ include files
#include "MBMDump/MBMDump.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFIO.h"

using namespace LHCb;
using namespace MBMDump;

static const char* Unknown_type = "Unknown";
static const char* mep_type = "MEP";
static const char* raw_type = "RawEvent";
static const char* dsc_type = "Descriptor";
static const char* mdf_type = "MDF";
static const char* buff_types[]={Unknown_type,mep_type,raw_type,dsc_type,mdf_type};


namespace MBMDump  {
  class MDFIOHelper : public LHCb::MDFIO  {
    typedef LHCb::StreamDescriptor   DSC;
    DSC::Access                m_bindDsc;
    DSC::Access                m_accessDsc;
    std::pair<char*,int>       m_data;
    /// Temporary Streambuffer to hold uncompressed data
    StreamBuffer               m_buff;
  public:
    MDFIOHelper(Writer_t typ, const std::string& nam) : MDFIO(typ,nam)  {}
    virtual ~MDFIOHelper()          { close();            }
    std::pair<char*,int>& data()    { return m_data;      }
    /// Set connection
    bool connect(const char* specs)  {
      m_bindDsc = DSC::bind(specs);
      if ( m_bindDsc.ioDesc > 0 )   {
        m_accessDsc = DSC::accept(m_bindDsc);
        return m_accessDsc.ioDesc > 0;
      }
      return false;
    }
    /// close connection
    void close()    {
      DSC::close(m_accessDsc);
      DSC::close(m_bindDsc);
    }
    /// Allocate space
    virtual std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_buff.reserve(len);
      return std::pair<char*,int>(m_buff.data(), m_buff.size());
    }
    /// Receive event and update communication structure
    virtual bool receiveData()  {
      m_data = std::pair<char*,int>(0,-1);
      if ( StreamDescriptor::seek(m_accessDsc,0,SEEK_CUR) != -1 )  {
        setupMDFIO(0,0);
        m_data = readBanks(&m_accessDsc,false);
        return m_data.second > 0;
      }
      return false;
    }
    /// Read raw byte buffer from input stream
    virtual StatusCode readBuffer(void* const ioDesc, void* const data, size_t len)  {
      DSC::Access* ent = (DSC::Access*)ioDesc;
      if ( ent && ent->ioDesc > 0 ) {
        if ( StreamDescriptor::read(*ent,data,len) )  {
          return StatusCode::SUCCESS;
        }
      }
      return StatusCode::FAILURE;
    }
  };
}

FileMainMenu::FileMainMenu() :  m_io(0), m_dispMenu(0)
{
  int num_types = sizeof(buff_types)/sizeof(buff_types[0]);
  ::strncpy(m_name,"file://mdf.dat",sizeof(m_name));
  ::strncpy(m_buffType,buff_types[2],sizeof(m_buffType));
  m_name[sizeof(m_name)-1] = 0;
  m_buffType[sizeof(m_buffType)-1] = 0;
  openMenu(0,0,"Event Dump","General purpose MBM Dump",procName()); 
  addCommand(C_PROC,     "Set file name           ");
  setParam  (m_buffType, 1,"%10s",buff_types[2],0,0,buff_types,num_types,1);
  addCommand(C_TYP,      "Input type   :^^^^^^^^^^");
  addComment(C_COM1,     "                        ");
  addComment(C_COM2,     "************************");
  addComment(C_COM3,     "                        ");
  addCommand(C_INC_EXC,  "Open file               ");
  addCommand(C_CMD,      "Display menu            ");
  addComment(C_COM4,     "                        ");
  addComment(C_COM5,     "************************");
  addComment(C_COM6,     "                        ");
  addCommand(C_EXIT,     "Exit                    ");
  closeMenu();
  enableCommand(C_PROC);
  enableCommand(C_TYP);
  disableCommands(2,C_INC_EXC,C_CMD);
  openOldWindow();          // Set cursor back on menu window
  m_dispMenu = new DisplayMenu(this,C_CMD);
  m_fileMenu = new FileMenu(this,C_PROC);
  m_io       = new MDFIOHelper(MDFIO::MDF_BANKS,"MBMDump");
  setCursor(C_PROC,1);      // set cursor at top
}

FileMainMenu::~FileMainMenu() {
  drop(m_dispMenu);
  drop(m_fileMenu);
}

void FileMainMenu::acceptFile(const char* fnam)  {
  char* ptr = 0;
  ::strncpy(m_name,fnam,sizeof(m_name));
  m_name[sizeof(m_name)-1] = 0;
  if ( (ptr=strchr(m_name,' ')) ) *ptr = 0;
  m_fileMenu->hideMenu();
  setCursor(C_TYP,1);
}

void FileMainMenu::handleMenu(int cmd_id)    {
  char* ptr;
  int b_type = DisplayMenu::B_UNKNOWN;
  switch(cmd_id){
  case C_PROC:
    m_fileMenu->setCursor(1);
    return;
  case C_TYP:
    enableCommands(1,C_INC_EXC);
    setCursor(C_INC_EXC,1);
    return;
  case C_INC_EXC: // open new file
    m_name[sizeof(m_name)-1] = 0;
    m_buffType[sizeof(m_buffType)-1] = 0;
    if ( (ptr=::strchr(m_name,' ')) ) *ptr = 0;
    if ( (ptr=::strchr(m_buffType,' ')) ) *ptr = 0;
    if ( m_io->connect(m_name) )  {
      if      ( !::strcmp(m_buffType,mep_type) ) b_type = DisplayMenu::B_MEP;
      else if ( !::strcmp(m_buffType,raw_type) ) b_type = DisplayMenu::B_RAW;
      else if ( !::strcmp(m_buffType,dsc_type) ) b_type = DisplayMenu::B_DESC;
      else if ( !::strcmp(m_buffType,mdf_type) ) b_type = DisplayMenu::B_MDF;
      else                                     b_type = DisplayMenu::B_UNKNOWN;
      m_dispMenu->update(b_type);
      output("Opened file: %s",m_name);
      enableCommand(C_CMD);
      m_dispMenu->update(b_type);
      m_dispMenu->show();
      return;
    }
    enableCommand(C_INC_EXC);
    setCursor(C_PROC,1);
    output("Failed to open file:%s",m_name);
    return;
  case C_CMD:
    m_dispMenu->show();
    return;
  case C_EXIT:
    ::exit(quit());
    return; 
  }
}

int FileMainMenu::getEvent(struct DataBlock *event)    {
  if ( m_io )  {
    if ( m_io->receiveData() )  {
      event->number = EVENT_TYPE_EVENT;
      event->start = (int*)m_io->data().first;
      event->length = m_io->data().second/sizeof(int);
      ::memset(event->mask,0,sizeof(event->mask));
      return MBM_NORMAL;
    }
  }
  return MBM_ERROR;
}
