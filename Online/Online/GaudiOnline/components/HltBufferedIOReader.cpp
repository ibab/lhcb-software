// $Id: HltBufferedIOReader.h,v 1.12 2009-12-02 10:32:35 apuignav Exp $
#ifndef GAUDIONLINE_DIMRPCFILEREADER_H
#define GAUDIONLINE_DIMRPCFILEREADER_H

// Framework includes
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IEventProcessor.h"
#include "MBM/BufferInfo.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <sys/stat.h>
#include <ctime>
#include <set>

// Forward declarations
class IIncidentSvc;

/*
 *   LHCb namespace declaration
 */
namespace LHCb
{

  // Forward declarations
  class MEPManager;

  /** Class definition of HltBufferedIOReader.
   *
   * This is the online extension of the runnable of the application manager.
   * The runable handles the actual run() implementation of the
   * ApplicationMgr object.
   *
   *  @author Markus Frank
   *  @modified Albert Puig
   *
   *  @version 1.5
   */
  class HltBufferedIOReader: public OnlineService, virtual public IRunable
  {
  public:
    enum {
      GO_DONT_PROCESS=0,
      GO_PROCESS=1,
      GO_GENTLE_STOP=2
    };

  protected:
    /// Flag indicating that MBM event retrieval is active
    bool                     m_receiveEvts;
    /// Lock handle
    lib_rtl_lock_t           m_lock;
    /// Reference to MEP manager service
    MEPManager*              m_mepMgr;
    /// Reference to buffer manager producer
    MBM::Producer*           m_producer;
    /// Current file name
    std::string              m_current;
    /// List of files to process
    std::set<std::string>    m_files;

    /// Property: Name of the 'GO' command which should be used. If empty feature is disabled.
    std::string              m_goService;
    /// Property: Buffer name for data output
    std::string              m_buffer;
    /// Property: Data directory name
    std::string              m_directory;
    /// Property: File prefix to select files from data directory
    std::string              m_filePrefix;
    /// Property: Path to the file containing broken nodes, where no reading should happen
    std::string              m_brokenHostsFile;
    /// Property: List of runs to be processed (as strings!)
    std::vector<std::string> m_allowedRuns;
    /// Property: Maximum number of seconds to wait for consumers (default: 20 seconds)
    int                      m_maxConsWait;
    /// Property: Time for initial sleep before starting to process files (default: 10 sec)
    int                      m_initialSleep;
    /// Property: Time for sleep before finishing the PAUSE transition (default: 5 sec)
    int                      m_pauseSleep;
    /// Property: Inhibit rescan of the source directory
    int                      m_rescan;
    /// Property: Maximum number of events to be read from 1 single input file
    int                      m_max_events_per_file;
    /// Property: Buffer names to be checked on special transitions
    std::vector<std::string> m_mbmNames;

    /// Monitoring quantity: Number of events processed
    int                      m_evtCount;
    /// Monitoring quantity: Number of events per current input file
    int                      m_evtCountFile;
    /// DIM service ID for publishing currentRun
    int                      m_runSvcID;
    /// Monitoring quantity: Currently processed run number
    int                      m_currentRun;
    /// DIM service ID for 'GO' command
    int                      m_goSvcID;
    /// DIM value of GO service
    int                      m_goValue;
    /// Flag to indicate if files should be deleted
    bool                     m_deleteFiles;
    /// Flag to indicate if the node is in the broken hosts file and hence disabled
    bool                     m_disabled;
    /// Flag to indicate if PAUSE incident was received or should be sent.
    bool                     m_paused;
    /// Buffer information blocks to be checked before special transitions are completed
    MBM::BufferInfo*         m_mbmInfos;
    /// String keyed map of buffer information blocks.
    std::map<std::string,MBM::BufferInfo*> m_mbmInfo;
    

    // Helper routines

    /// DIM command service callback
    static void go_handler(void* tag, void* address, int* size);
    /// DIM service update handler
    static void run_no_update(void* tag, void** buf, int* size, int* first);

    /// Scan directory for matching files
    size_t scanFiles();
    /// Open a new data file
    int openFile();
    /// Save remainder of currently read file
    void safeRestOfFile(int file_handle);

    /// Wait until event buffers are empty before finishing....
    void waitForPendingEvents(int seconds=100);

    /// IRunable implementation : Run the class implementation
    virtual StatusCode i_run();

  public:
    /// Standard Constructor
    HltBufferedIOReader(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~HltBufferedIOReader();
    /// IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid,
                                      void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// Low level overload from Service base class: sysStart
    virtual StatusCode sysStart();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// Low level overload from Service base class: sysStop
    virtual StatusCode sysStop();
    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
  };
} // End namespace LHCb
#endif // GAUDIONLINE_DIMRPCFILEREADER_H
// $Id: HltBufferedIOReader.cpp,v 1.29 2010-09-21 14:28:25 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiOnline/HltBufferedIOReader.h"
#include "GaudiOnline/MEPManager.h"
#include "MDF/MEPEvent.h"
#include "MDF/MDFHeader.h"
#include "RTL/rtl.h"
#include "RTL/readdir.h"
#include <fcntl.h>
#include <cerrno>
#include <sstream>
#include "dic.h"
#include "dis.h"
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

// Instantiation of a static factory class used by clients to create instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,HltBufferedIOReader)

using namespace LHCb;
using namespace std;

static int file_read(int file, char* p, int len) {
  int tmp = 0;
  while (tmp < len)   {
    int sc = ::read(file, p + tmp, len - tmp);
    if (sc > 0)
      tmp += sc;
    else if (sc == 0)
      return 0;
    else return -1;
  }
  return 1;
}

static bool file_write(int file, const void* data, int len)   {
  const char* p = (const char*) data;
  int tmp = len;
  while (tmp > 0)  {
    int sc = ::write(file, p + len - tmp, tmp);
    if (sc > 0)
      tmp -= sc;
    else return false;
  }
  return true;
}

static bool check_consumers(const MBM::BufferInfo& info, int pid, int evtyp)   {
  return info.num_consumers_partid_evtype(pid,evtyp) != 0;
}
static bool check_consumers_partid(const MBM::BufferInfo& info, int pid)   {
  return info.num_consumers_partid(pid) != 0;
}

/// DIM command service callback
void HltBufferedIOReader::go_handler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    HltBufferedIOReader* rdr = *(HltBufferedIOReader**)tag;
    rdr->m_goValue = *(int*)address;
  }
}

/// DIM service update handler
void HltBufferedIOReader::run_no_update(void* tag, void** buf, int* size, int* first)  {
  HltBufferedIOReader* h = *(HltBufferedIOReader**)tag;  
  if ( *first )  {
  }
  if ( h )  {
    *size = sizeof(h->m_currentRun);
    *buf  = &h->m_currentRun;
  }
}

/// Standard Constructor
HltBufferedIOReader::HltBufferedIOReader(const string& nam, ISvcLocator* svcLoc)
  : OnlineService(nam, svcLoc), m_receiveEvts(false), m_lock(0), m_mepMgr(0), 
    m_producer(0), m_evtCount(0), m_evtCountFile(0), m_runSvcID(0), m_currentRun(0),
    m_goSvcID(0), m_goValue(0), m_disabled(false), m_paused(false), m_mbmInfos(0)
{
  declareProperty("Buffer",         m_buffer          = "Mep");
  declareProperty("Directory",      m_directory       = "/localdisk");
  declareProperty("FilePrefix",     m_filePrefix      = "Run_");
  declareProperty("BrokenHosts",    m_brokenHostsFile = "");
  declareProperty("DeleteFiles",    m_deleteFiles     = true);
  declareProperty("ConsumerWait",   m_maxConsWait     = 20);
  declareProperty("AllowedRuns",    m_allowedRuns);
  declareProperty("InitialSleep",   m_initialSleep    = 10);
  declareProperty("PauseSleep",     m_pauseSleep      = 5);
  declareProperty("Rescan",         m_rescan          = 1);
  declareProperty("GoService",      m_goService       = "");
  declareProperty("EventsPerFile",  m_max_events_per_file = -1);
  declareProperty("CheckedBuffers", m_mbmNames);
  
  m_allowedRuns.push_back("*");
  ::lib_rtl_create_lock(0, &m_lock);
}

/// Standard Destructor
HltBufferedIOReader::~HltBufferedIOReader()  {
  if ( m_lock ) {
    ::lib_rtl_delete_lock(m_lock);
  }
}

/// IInterface implementation : queryInterface
StatusCode HltBufferedIOReader::queryInterface(const InterfaceID& riid, void** ppIf)    {
  if (IRunable::interfaceID().versionMatch(riid))  {
    *ppIf = (IRunable*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// IService implementation: initialize the service
StatusCode HltBufferedIOReader::initialize()   {
  StatusCode sc;
  string broken_hosts;
  if (!(sc = OnlineService::initialize()).isSuccess())
    return error("Failed to initialize service base class.");
  else if (!(sc = service("MEPManager", m_mepMgr)).isSuccess())
    return error("Failed to access MEP manager service.");
  broken_hosts = m_brokenHostsFile;
  if ( !broken_hosts.empty() ) {
    struct stat file;
    if ( 0 == ::stat(broken_hosts.c_str(),&file) ) {
      const std::string node = RTL::nodeNameShort();
      int   fd   = ::open(broken_hosts.c_str(),O_RDONLY);
      if ( -1 == fd )  {
        return error("Failed to access broken node file:"+broken_hosts+" [Error ignored]");
      }
      char* data = new char[file.st_size+1];
      int rc = file_read(fd,data,file.st_size);
      if ( 1 == rc ) {
        data[file.st_size] = 0;
        for(int i=0; i<file.st_size; ++i) 
          data[i] = ::toupper(data[i]);
        for(char* ptr=(char*)node.c_str(); *ptr; ++ptr)
          *ptr = ::toupper(*ptr);
        if ( ::strstr(data,node.c_str()) ) {
          warning("Node is disabled and will not process any deferred files.");
          m_disabled = true;
        }
      }
      delete [] data;
      ::close(fd);
    }
    else {
      error("Failed to access broken node file:"+broken_hosts+" [Error ignored]");
    }
  }

  m_producer = m_mepMgr->createProducer(m_buffer, RTL::processName());
  if (0 == m_producer)  {
    return error("Fatal error: Failed to create MBM producer object.");
  }
  incidentSvc()->addListener(this, "DAQ_PAUSE");
  incidentSvc()->addListener(this, "DAQ_CANCEL");
  incidentSvc()->addListener(this, "DAQ_STOP_TRIGGER");
  declareInfo("EvtCount", m_evtCount = 0, "Number of events processed");
  declareInfo("EvtCountFile", m_evtCountFile = 0, "Number of events processed from current input");
  m_currentRun = 0;
  ::lib_rtl_lock(m_lock);
  m_files.clear();
  m_current = "";
  m_paused = false;
  m_goValue = GO_PROCESS;
  if ( !m_goService.empty() )  {
    m_goValue = GO_DONT_PROCESS;
    m_goSvcID = ::dic_info_service(m_goService.c_str(),MONITORED,
                                   0,0,0,go_handler,(long)this,0,0);
  }
  string nam = RTL::processName() + "/CurrentRunNumber";
  m_runSvcID = ::dis_add_service(nam.c_str(),"I",0,0,run_no_update,(long)this);

  // Buffer information blocks
  if ( m_mbmNames.empty() ) m_mbmNames.push_back(m_buffer);
  m_mbmInfos = new MBM::BufferInfo[m_mbmNames.size()];
  for(size_t i=0; i<m_mbmNames.size(); ++i)  {
    string nam = m_mbmNames[i];
    string bm_name = m_mepMgr->bufferName(nam);
    m_mbmInfo[nam] = &m_mbmInfos[i];
    m_mbmInfos[i].attach(bm_name.c_str());
  }
  // Patch the string array with allowed runs
  for(size_t i=0; i<m_allowedRuns.size(); ++i)   {
    const char* crun = m_allowedRuns[i].c_str();
    if ( ::isdigit(crun[0]) && crun[0] != '*' )   {
      char text[PATH_MAX];
      long runno = ::strtol(crun,0,10);
      ::snprintf(text,sizeof(text),"%s%07ld_",m_filePrefix.c_str(),runno);
      m_allowedRuns[i] = text;
      info("Add run "+m_allowedRuns[i]+" to allowed run-list.");
    }
  }  
  return sc;
}

/// IService implementation: finalize the service
StatusCode HltBufferedIOReader::finalize()  {
  m_mbmInfo.clear();
  if ( m_mbmInfos )  {
    delete [] m_mbmInfos;
    m_mbmInfos = 0;
  }
  
  if ( m_runSvcID )  {
    ::dis_remove_service(m_runSvcID);
    m_runSvcID = 0;
  }

  if ( m_goSvcID )  {
    ::dic_release_service(m_goSvcID);
    m_goSvcID = 0;
  }

  m_paused = false;
  m_goValue = GO_DONT_PROCESS;
  if ( m_producer )  {
    if (m_mepMgr->FSMState() != Gaudi::StateMachine::OFFLINE)    {
      m_producer->exclude();
    }
    delete m_producer;
    m_producer = 0;
  }
  m_files.clear();
  releaseInterface(m_mepMgr);
  return OnlineService::finalize();
}

StatusCode HltBufferedIOReader::sysStart()  {
#if 0
  if ( m_producer )  {
    m_producer->exclude();
    delete m_producer;
    m_producer = 0;
  }
  m_producer = m_mepMgr->createProducer(m_buffer,RTL::processName());
#endif
  m_paused = false;
  m_receiveEvts = true;
  ::lib_rtl_unlock(m_lock);
  return StatusCode::SUCCESS;
}

/// IService implementation: finalize the service
StatusCode HltBufferedIOReader::sysStop()   {
  m_paused = false;
  m_goValue = GO_DONT_PROCESS;
  incidentSvc()->removeListener(this);
  if (m_receiveEvts)  {
    if (m_mepMgr)    {
      m_mepMgr->cancel();
    }
  }
  ::lib_rtl_unlock(m_lock);
  m_receiveEvts = false;
  return StatusCode::SUCCESS;
}

void HltBufferedIOReader::waitForPendingEvents(int seconds)    {
  size_t count;
  do    {
    count = 0;
    for(auto i=m_mbmInfo.begin(); i!=m_mbmInfo.end(); ++i)
      count += (*i).second->num_events();
    if ( count > 0 ) ::lib_rtl_sleep(1000);
  } while ( count > 0 && --seconds >= 0 );
  ::lib_rtl_sleep(1000);
}

/// Incident handler implemenentation: Inform that a new incident has occured
void HltBufferedIOReader::handle(const Incident& inc)
{
  info("Got incident:" + inc.source() + " of type " + inc.type());
  if (inc.type() == "DAQ_CANCEL" )  {
    m_goValue = GO_DONT_PROCESS;
    m_receiveEvts = false;
    if (m_mepMgr)    {
      m_mepMgr->cancel();
    }
    ::lib_rtl_unlock(m_lock);
  }
  else if ( m_mepMgr && inc.type() == "DAQ_PAUSE" )  {
    // This is not terribly elegant, but we have to stop the DIM callback
    // and wait until the pipeline is empty....
    MBM::BufferInfo mbmInfo;
    m_paused = true;
    m_goValue = GO_DONT_PROCESS;
    m_receiveEvts = false;
    if (m_mepMgr)    {
      info("Waiting until event pipeline is empty.....");
      waitForPendingEvents();
      m_mepMgr->cancel();
    }
    ::lib_rtl_unlock(m_lock);
    ::lib_rtl_sleep(1000*m_pauseSleep);
    info("No events pending anymore...continue with PAUSE processing.....");
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_receiveEvts = true;
  }
  else if (inc.type() == "DAQ_STOP_TRIGGER")  {
    m_receiveEvts = false;
  }
  else if (inc.type() == "DAQ_START_TRIGGER")  {
    m_receiveEvts = true;
  }
}

/// Scan directory for matching files
size_t HltBufferedIOReader::scanFiles()   {
  m_files.clear();
  if ( !m_disabled )   {
    DIR* dir = opendir(m_directory.c_str());
    if (dir)  {
      struct dirent *entry;
      bool take_all = (m_allowedRuns.size() > 0 && m_allowedRuns[0]=="*");
      while ((entry = ::readdir(dir)) != 0)    {
        //cout << "File:" << entry->d_name << endl;
        if ( 0 != ::strncmp(entry->d_name,m_filePrefix.c_str(),m_filePrefix.length()) ) {
          continue;
        }
        else if ( !take_all )  {
          bool take_run = false;
          for(vector<string>::const_iterator i=m_allowedRuns.begin(); i!=m_allowedRuns.end(); ++i) {
            if ( ::strstr(entry->d_name,(*i).c_str()) == entry->d_name ) {
              take_run = true;
              break;
            }
          }
          if ( !take_run ) continue;
        }
        //cout << "SELECT:" << entry->d_name << endl;
        m_files.insert(m_directory + "/" + entry->d_name);
      }
      ::closedir(dir);
      if ( !m_rescan ) m_disabled = true;
      return m_files.size();
    }
    const char* err = RTL::errorString();
    info("Failed to open directory:" + string(err ? err : "????????"));
  }
  return 0;
}

/// Open a new data file
int HltBufferedIOReader::openFile()   {
  while (m_files.size() > 0)  {
    set<string>::iterator i = m_files.begin();
    string fname = *i;
    m_files.erase(i);
    int fd = ::open(fname.c_str(), O_RDONLY | O_BINARY, S_IREAD);
    if ( -1 != fd )    {
      const char* ptr = fname.c_str()+m_directory.length()+1+m_filePrefix.length();
      m_currentRun = 0;
      ::sscanf(ptr,"%07d",&m_currentRun);
      if ( m_runSvcID ) {
        ::dis_update_service(m_runSvcID);
      }
      
      if (m_deleteFiles)      {
        int sc = ::unlink(fname.c_str());
        if (sc != 0)        {
          error("CANNOT UNLINK file: " + fname + ": " + RTL::errorString());
          //::exit(EBADF);
          m_receiveEvts = false;
          incidentSvc()->fireIncident(Incident(name(),"DAQ_ERROR"));
          ::close(fd);
          return 0;
        }
      }
      m_current = fname;
      if ( m_max_events_per_file < 0 )  {
        info("Opened file: " + fname + " for HLT processing");
      }
      else  {
        stringstream s;
        s << "Opened file: " << fname << " for HLT processing [" 
          << m_max_events_per_file << " events]";
        info(s.str());
      }
      return fd;
    }
    error("FAILD to open file: " + fname + " for deferred HLT processing: "
          + RTL::errorString());
  }
  return 0;
}

/// Save remainder of currently read file
void HltBufferedIOReader::safeRestOfFile(int file_handle)     {
  if (m_deleteFiles && file_handle)  {
    char buffer[10 * 1024];
    info("Saving rest of file[%d]: %s",file_handle,m_current.c_str());
    int cnt = 0, ret, fd = ::open(m_current.c_str(), O_CREAT | O_BINARY | O_WRONLY, 0777);
    if ( -1 == fd )    {
      error("CANNOT Create file: " + m_current + ": " + RTL::errorString());
      return;
    }
    while ((ret = ::read(file_handle, buffer, sizeof(buffer))) > 0)  {
      if (!file_write(fd, buffer, ret))      {
        error("CANNOT write file: " + m_current + ": " + RTL::errorString());
      }
      cnt += ret;
    }
    info("Wrote %d bytes to file:%s fd:%d",cnt,m_current.c_str(),fd);
    ::close(fd);
  }
  if ( file_handle )  {
    ::close(file_handle);
    file_handle = 0;
  }
  m_current = "";
}

/// IRunable implementation : Run the class implementation
StatusCode HltBufferedIOReader::i_run()  {
  const RawBank* bank = (RawBank*)this;
  int cons_wait;
  int file_handle = 0;
  int event_number = 0;
  int partid = m_mepMgr->partitionID();
  bool files_processed = false;
  bool m_goto_paused = true;
  MBM::BufferInfo* mbmInfo = m_mbmInfo[m_buffer];

  if ( m_initialSleep > 0 )   {
    ::lib_rtl_sleep(1000*m_initialSleep);
  }
  m_receiveEvts = true;
  if ( !m_goService.empty() )   {
    while( m_receiveEvts && (GO_PROCESS != m_goValue) )  {
      ::lib_rtl_sleep(100);
    }
    if ( !m_receiveEvts || (GO_PROCESS != m_goValue) )  {
      // If a 'stop' came in the meantime we have to terminate.
      return StatusCode::SUCCESS;
    }
  }

  /// Before we start, we need to check if there are consumers present:
  for(cons_wait = m_maxConsWait; !check_consumers_partid(*mbmInfo,partid) && --cons_wait>=0; )
    ::lib_rtl_sleep(1000);

  if ( cons_wait <= 0 )   {
    incidentSvc()->fireIncident(Incident(name(),"DAQ_ERROR"));
    return error("No consumers present to consume event. ERROR & Return!");
  }
  
  while (1)  {
    files_processed = (GO_PROCESS==m_goValue) && (scanFiles() == 0);

    // Handle case to go to paused or change of the GO value
    if ( (m_goto_paused && files_processed) || (m_goValue != GO_PROCESS) )   {
      if ( file_handle )  {
        safeRestOfFile(file_handle);
        file_handle = 0;
        m_currentRun = 0;
        if ( m_runSvcID ) ::dis_update_service(m_runSvcID);
      }
      /// Before actually declaring PAUSED, we wait until no events are pending anymore.
      waitForPendingEvents();
      /// Go to state PAUSED, all the work is done
      if ( !m_paused )  {
        // Sleep a bit before goung to pause
        info("Sleeping before going to PAUSE....");
        ::lib_rtl_sleep(1000*m_pauseSleep);
        incidentSvc()->fireIncident(Incident(name(),"DAQ_PAUSE"));
      }
      info("Quitting...");
      break;
    }
    else if ( files_processed ) {
      info("Locking event loop. Waiting for work....");
      m_currentRun = 0;
      if ( m_runSvcID ) ::dis_update_service(m_runSvcID);
      ::lib_rtl_lock(m_lock);
    }
    // Again check if the GO value changed, since we may have been waiting in the lock!
    if ( !m_receiveEvts || (m_goValue == GO_DONT_PROCESS) )    {
      if ( file_handle )  {
        safeRestOfFile(file_handle);
        file_handle = 0;
      }
      info("Quitting...");
      break;
    }
    m_currentRun = 0;
    if ( m_runSvcID ) ::dis_update_service(m_runSvcID);
    m_evtCount = 0;
    m_evtCountFile = 0;
    files_processed = (GO_PROCESS != m_goValue) && scanFiles() == 0;
    if ( files_processed )    {
      info("Exit event loop. No more files to process.");
      break;
    }
    // loop over the events
    while ( m_receiveEvts && (m_goValue != GO_DONT_PROCESS) )   {
      if (file_handle == 0 && (m_goValue == GO_PROCESS) )  {
        file_handle = openFile();
        m_evtCountFile = 0;
        if ( file_handle == 0 )   {
          files_processed = (GO_PROCESS!=m_goValue) || (scanFiles() == 0);
          if ( files_processed )    {
            break;
          }
        }
        event_number = 0;
      }
      if ( file_handle != 0 )  {
        int size_buf[3];
        off_t file_position = ::lseek(file_handle, 0, SEEK_CUR);
        int status = ::file_read(file_handle, (char*)size_buf, sizeof(size_buf));
        if (status <= 0)   {
          ::close(file_handle);
          file_handle = 0;
          m_current = "";
          continue;
        }
        bool is_mdf   = size_buf[0] == size_buf[1] && size_buf[0] == size_buf[2];
        int  evt_size = size_buf[0];
        int  buf_size = evt_size + (is_mdf ? bank->hdrSize() : sizeof(MEPEVENT) + sizeof(int));
        // Careful here: sizeof(int) MUST match me->sizeOf() !
        // The problem is that we do not (yet) have a valid data pointer!
        ++event_number;
        try   {
          status = m_producer->spaceRearm(buf_size);
        }
        catch (const exception& e)        {
          error("Exception while reading Input files (spaceRearm): %s "
                "Event:%d file:%s BuffSize:%d. ",
                e.what(),event_number,m_current.c_str(),buf_size);
          // Set back the file position to the beginning of the event and continue.
          // If there was a cancel in between, the file shall be saved.
          ::lseek(file_handle, file_position, SEEK_SET);
          continue;
        }
        if (status == MBM_NORMAL)        {
          MBM::EventDesc& dsc = m_producer->event();
	  RawBank* mdf_bank = 0;
          char*  read_ptr = 0;
          size_t read_len = 0;
          if ( is_mdf ) {
            RawBank* b = (RawBank*)dsc.data;
            b->setMagic();
            b->setType(RawBank::DAQ);
            b->setSize(MDFHeader::sizeOf(3));
            b->setVersion(DAQ_STATUS_BANK);
            read_ptr = b->begin<char>();
            ::memcpy(read_ptr,size_buf,sizeof(size_buf));
            read_ptr += sizeof(size_buf);
            read_len = evt_size-sizeof(size_buf);
            dsc.len  = evt_size+b->hdrSize();
            dsc.type = EVENT_TYPE_EVENT;
	    mdf_bank = b;
          }
          else {
            static int id = -1;
            MEPEVENT* e = (MEPEVENT*) dsc.data;
            MEPEvent* me = (MEPEvent*) e->data;
            me->setSize(evt_size);
            e->refCount = m_refCount;
            e->evID = ++id;
            e->begin = long(e) - long(m_producer->bufferAddress());
            e->packing = -1;
            e->valid = 1;
            e->magic = mep_magic_pattern();
            for (size_t j = 0; j < MEP_MAX_PACKING; ++j)   {
              e->events[j].begin = 0;
              e->events[j].evID = 0;
              e->events[j].status = EVENT_TYPE_OK;
              e->events[j].signal = 0;
            }
            read_ptr = (char*)me->start();
            ::memcpy(read_ptr,size_buf+1,2*sizeof(size_buf[0]));
            read_ptr += 2*sizeof(size_buf[0]);
            read_len = me->size() - 2*sizeof(int);
            dsc.len  = sizeof(MEPEVENT) + me->sizeOf() + me->size();
            dsc.type = EVENT_TYPE_MEP;
          }
          status = ::file_read(file_handle,read_ptr, read_len);
          if (status <= 0)   {  // End-of file, continue with next file
            ::close(file_handle);
            file_handle = 0;
            m_current = "";
            continue;
          }
          else   {
            // Check if there are consumers pending before declaring the event.
            // This should be a rare case, since there ARE (were?) consumers.
            // Though: In this case the event is really lost!
            // But what can I do...
            for(cons_wait = m_maxConsWait; !check_consumers(*mbmInfo,partid,dsc.type) && --cons_wait>=0; )
              ::lib_rtl_sleep(1000);

            if ( cons_wait <= 0 )  {
              error("No consumers (partition:%d event type:%d) present to consume event %d",
                    partid,dsc.type,event_number);
              error("Safe rest of file and finish. Skipping rest of file: %s",m_current.c_str());
              // If we did not see an MDF header, the file may also be corrupted:
              if ( dsc.type == EVENT_TYPE_MEP )  {
                error("If Moores are alive, this file may be corrupted: %s",m_current.c_str());
              }
              m_receiveEvts = false;
              ::lseek(file_handle, file_position, SEEK_SET);
              safeRestOfFile(file_handle);
              file_handle = 0;
              /// Before actually declaring ERROR, we wait until no events are pending anymore.
              waitForPendingEvents();
              /// Go to state PAUSED, all the work is done
              incidentSvc()->fireIncident(Incident(name(),"DAQ_ERROR"));
              return StatusCode::FAILURE;
            }
            //cout << "Event length:" << dsc.len << endl;
	    if ( 0 != mdf_bank )  {
	      // If the first bank is a MDF bank, we copy the real trigger mask
	      MDFHeader* h = (MDFHeader*)mdf_bank->data();
	      ::memcpy(dsc.mask,h->subHeader().H1->triggerMask(),sizeof(dsc.mask));	      
	    }
	    else  {
	      // If MEP, we emulate a trigger mask with the partition ID
	      dsc.mask[0] = partid;
	      dsc.mask[1] = ~0x0;
	      dsc.mask[2] = ~0x0;
	      dsc.mask[3] = ~0x0;
	    }
            //
            // Declare the event to the buffer manager
            //
            try {
              status = m_producer->declareEvent();
            }
            catch (const exception& e)        {
              info("Exception while delareEvent: %s Event:%d File:%s.",
                   e.what(),event_number,m_current.c_str());
              status = MBM_ERROR;
            }
            catch(...)   {
              info("UNKNOWN Exception while delareEvent: Event:%d File:%s.",
                   event_number,m_current.c_str());
              status = MBM_ERROR;
            }
            if (status != MBM_NORMAL)    {
              continue;
            }
            //
            // Now send space
            //
            try  {
              status = m_producer->sendSpace();
            }
            catch (const exception& e)        {
              info("Exception while sendSpace: %s Event:%d File:%s.",
                   e.what(),event_number,m_current.c_str());
              status = MBM_ERROR;
            }
            catch(...)   {
              info("UNKNOWN Exception while sendSpace: Event:%d File:%s.",
                   event_number,m_current.c_str());
              status = MBM_ERROR;
            }
            if (status != MBM_NORMAL)    {
              continue;
            }
            else  {
              m_evtCount++;
              m_evtCountFile++;
              // If we have exceeded the total number of events per file, close it!
              if ( m_max_events_per_file>0 && m_evtCountFile>m_max_events_per_file )  {
                ::close(file_handle);
                file_handle = 0;
                m_current = "";
              }
            }
          }
        }
        else if (file_handle)   {
          // undo reading of the first integer before saving rest of file
          ::lseek(file_handle, -3*sizeof(int), SEEK_CUR);
        }
      }
    }
    safeRestOfFile(file_handle);
    file_handle = 0;
    // Bad file: Cannot read input (m_evtCount==0)
    m_currentRun = 0;
    if ( m_runSvcID ) ::dis_update_service(m_runSvcID);
    m_evtCount = 0;
    m_evtCountFile = 0;
  }
  /// Before actually declaring PAUSED, we wait until no events are pending anymore.
  waitForPendingEvents();
  return StatusCode::SUCCESS;
}

/// IRunable implementation : Run the class implementation
StatusCode HltBufferedIOReader::run()   {
  try  {
    StatusCode sc = i_run();
    return sc;
  }
  catch (const exception& e)  {
    error("Exception while reading MEP files:" + string(e.what()));
    throw;
  }
  catch (...)  {
    error("UNKNWON Exception while reading MEP files.");
    throw;
  }
}
