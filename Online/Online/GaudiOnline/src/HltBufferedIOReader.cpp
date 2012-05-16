// $Id: HltBufferedIOReader.h,v 1.12 2009-12-02 10:32:35 apuignav Exp $
#ifndef GAUDIONLINE_DIMRPCFILEREADER_H
#define GAUDIONLINE_DIMRPCFILEREADER_H

// Framework includes
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IEventProcessor.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <ctime>
#include <set>

// Forward declarations
class IIncidentSvc;

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

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
  class HltBufferedIOReader : public OnlineService, virtual public IRunable  {
  protected:
    /// Flag indicating that MBM event retrieval is active
    bool                   m_receiveEvts;
    /// Lock handle
    void*                  m_lock;
    MEPManager*            m_mepMgr;
    MBM::Producer*         m_producer;
    std::string            m_buffer;
    std::string            m_current;
    std::string            m_directory;
    std::set<std::string>  m_files;

    /// Monitoring quantity: Number of events processed
    int                    m_evtCount;
    /// Flag to indicate if files should be deleted
    bool                   m_deleteFiles;

    // Helper routines
    size_t                 scanFiles();
    int                    openFile();
    void                   safeRestOfFile(int file_handle);

    /// IRunable implementation : Run the class implementation
    virtual StatusCode i_run();

  public:
    /// Standard Constructor
    HltBufferedIOReader(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~HltBufferedIOReader();
    /// IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
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
}      // End namespace LHCb
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
#include "RTL/rtl.h"
#include "RTL/readdir.h"
#include <fcntl.h>
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

/// Standard Constructor
HltBufferedIOReader::HltBufferedIOReader(const string& nam, ISvcLocator* svcLoc)   
  : OnlineService(nam, svcLoc), m_receiveEvts(false), m_lock(0), m_producer(0), m_evtCount(0)
{
  declareProperty("Buffer",     m_buffer = "Mep");
  declareProperty("Directory",  m_directory = "/localdisk");
  declareProperty("DeleteFiles",m_deleteFiles = true);
  ::lib_rtl_create_lock(0,&m_lock);
}

/// Standard Destructor
HltBufferedIOReader::~HltBufferedIOReader()  {
  ::lib_rtl_delete_lock(m_lock);
}

/// IInterface implementation : queryInterface
StatusCode HltBufferedIOReader::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IRunable::interfaceID().versionMatch(riid) ) {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// IService implementation: initialize the service
StatusCode HltBufferedIOReader::initialize()   {
  StatusCode sc;
  if ( !(sc=OnlineService::initialize()).isSuccess() )
    return error("Failed to initialize service base class.");
  else if ( !(sc=service("MEPManager",m_mepMgr)).isSuccess() )
    return error("Failed to access MEP manager service.");
  m_producer = m_mepMgr->createProducer(m_buffer,RTL::processName());
  if ( 0 == m_producer ) {
    return error("Fatal error: Failed to create MBM producer object.");
  }
  incidentSvc()->addListener(this,"DAQ_CANCEL");
  declareInfo("EvtCount",m_evtCount=0,"Number of events processed");
  ::lib_rtl_lock(m_lock);
  m_files.clear();
  m_current = "";
  return sc;
}

/// IService implementation: finalize the service
StatusCode HltBufferedIOReader::finalize()     {
  if ( m_producer ) {
    if ( m_mepMgr->FSMState() != Gaudi::StateMachine::OFFLINE ) {
      m_producer->exclude();
    }
    delete m_producer;
    m_producer = 0;
  }
  m_files.clear();
  releaseInterface(m_mepMgr);
  return OnlineService::finalize();
}

StatusCode HltBufferedIOReader::sysStart()   {
#if 0
  if ( m_producer ) {
    m_producer->exclude();
    delete m_producer;
    m_producer = 0;
  }
  m_producer = m_mepMgr->createProducer(m_buffer,RTL::processName());
#endif
  m_receiveEvts = true;
  ::lib_rtl_unlock(m_lock);
  return StatusCode::SUCCESS;
}

/// IService implementation: finalize the service
StatusCode HltBufferedIOReader::sysStop()     {
  incidentSvc()->removeListener(this);
  if ( m_receiveEvts ) {
    if ( m_mepMgr ) {
      m_mepMgr->cancel();
    }
  }
  ::lib_rtl_unlock(m_lock);
  m_receiveEvts = false;
  return StatusCode::SUCCESS;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void HltBufferedIOReader::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_receiveEvts = false;
    if ( m_mepMgr ) {
      m_mepMgr->cancel();
    }
    ::lib_rtl_unlock(m_lock);
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_receiveEvts = true;
  }
}

static int file_read(int file, char* p, int len)  {
  int tmp = 0;
  while ( tmp < len )  {
    int sc = ::read(file,p+tmp,len-tmp);
    if ( sc >  0 ) tmp += sc;
    else if ( sc == 0 ) return 0;
    else                return -1;
  }
  return 1;
}

static bool file_write(int file, const void* data, int len)  {
  const char* p = (const char*)data;
  int tmp = len;
  while ( tmp>0 )  {
    int sc = ::write(file,p+len-tmp,tmp);
    if ( sc > 0 ) tmp -= sc;
    else          return false;
  }
  return true;
}

size_t HltBufferedIOReader::scanFiles() {
  m_files.clear();
  DIR* dir = opendir(m_directory.c_str());
  if ( dir ) {
    struct dirent *entry;
    while ( (entry=::readdir(dir)) != 0 ) {
      string fname = entry->d_name;
      //cout << "File:" << fname << endl;
      if ( fname == "." || fname == ".." )   {
	continue;
      }
      m_files.insert(m_directory+"/"+entry->d_name);
    }
    ::closedir(dir);
    return m_files.size();
  }
  const char* err = RTL::errorString();
  error("Failed to open directory:"+string(err ? err : "????????"));
  return 0;
}

int HltBufferedIOReader::openFile() {
  while ( m_files.size()>0 ) {
    set<string>::iterator i=m_files.begin();
    string fname = *i;
    m_files.erase(i);
    int fd = ::open(fname.c_str(),O_RDONLY|O_BINARY,S_IREAD);
    if ( fd ) {
      if ( m_deleteFiles )   {
	int sc = ::unlink(fname.c_str());
	if ( sc != 0 ) {
	  error("CANNOT UNLINK file: "+fname+": "+RTL::errorString());
	  ::exit(0);
	}
      }
      m_current = fname;
      info("Opened file: "+fname+" for deferred HLT processing");
      return fd;
    }
    error("FAILD to open file: "+fname+" for deferred HLT processing: "+RTL::errorString());
  }
  return 0;
}

void HltBufferedIOReader::safeRestOfFile(int file_handle) {
  if ( file_handle ) {
    char buffer[10*1024];
    cout << "Saving rest of file[" << file_handle << "]:" << m_current << endl;
    int cnt=0, ret, fd = ::open(m_current.c_str(),O_CREAT|O_BINARY|O_WRONLY,0777);
    if ( fd < 0 ) {
      error("CANNOT Create file: "+m_current+": "+RTL::errorString());
    }
    while( (ret=::read(file_handle,buffer,sizeof(buffer))) > 0 ) {
      if ( !file_write(fd,buffer,ret) ) {
	error("CANNOT write file: "+m_current+": "+RTL::errorString());
      }
      cnt += ret;
    }
    cout << "Wrote " << cnt << " bytes to file:" << m_current << " fd:" << fd << endl;
    ::close(fd);
    ::close(file_handle);
    m_current = "";
    file_handle = 0;
  }
}

/// IRunable implementation : Run the class implementation
StatusCode HltBufferedIOReader::i_run()   {  
  int file_handle = 0;
  m_receiveEvts = true;
  ::lib_rtl_sleep(10000);
  while(1) {
    info("Locking event loop. Waiting for work....");
    ::lib_rtl_lock(m_lock);
    if ( !m_receiveEvts ) {
      info("Quitting...");
      break;
    }
    m_evtCount = 0;

    if ( scanFiles() == 0 ) {
      info("Exit event loop. No more files to process.");
      break;
    }
    // loop over the events
    while ( m_receiveEvts )   {
      if ( 0 == file_handle )  {
	file_handle = openFile();
	if ( 0 == file_handle ) {
	  if ( scanFiles() == 0 ) {
	    break;
	  }
	}
      }
      if ( file_handle ) {
	int evt_size;
	int status = ::file_read(file_handle, (char*)&evt_size, sizeof(evt_size));
	if ( status <= 0 )  {
	  ::close(file_handle);
	  file_handle = 0;
	  m_current = "";
	  continue;
	}
	// Careful here: sizeof(int) MUST match me->sizeOf() !
	// The problem is that we do not (yet) have a valid data pointer!
	status = m_producer->spaceRearm(sizeof(MEPEVENT)+sizeof(int)+evt_size);
	if ( status == MBM_NORMAL )  {
	  static int id = -1;
	  MBM::EventDesc& dsc = m_producer->event();
	  MEPEVENT* e = (MEPEVENT*)dsc.data;
	  MEPEvent* me = (MEPEvent*)e->data;
	  me->setSize(evt_size);
	  e->refCount    = m_refCount;
	  e->evID        = ++id;
	  e->begin       = long(e)-long(m_producer->bufferAddress());
	  e->packing     = -1;
	  e->valid       = 1;
	  e->magic       = mep_magic_pattern();
	  for(size_t j=0; j<MEP_MAX_PACKING; ++j) {
	    e->events[j].begin  = 0;
	    e->events[j].evID   = 0;
	    e->events[j].status = EVENT_TYPE_OK;
	    e->events[j].signal = 0;
	  }
	  //::memset(e->events,0,sizeof(e->events));
	  //char* ptr = ((char*))+me->sizeOf()+sizeof(MEPEVENT);
	  status = ::file_read(file_handle,(char*)me->start(),me->size());
	  if ( status <= 0 )  {
	    ::close(file_handle);
	    file_handle = 0;
	    m_current = "";
	    continue;
	  }
	  if ( status == MBM_NORMAL )  {
	    dsc.len = sizeof(MEPEVENT)+me->sizeOf()+me->size();
	    //cout << "Event length:" << dsc.len << endl;
	    dsc.mask[0] = m_mepMgr->partitionID();
	    dsc.mask[1] = ~0x0;
	    dsc.mask[2] = ~0x0;
	    dsc.mask[3] = ~0x0;
	    dsc.type    = EVENT_TYPE_MEP;
	    m_producer->declareEvent();
	    status =  m_producer->sendSpace();
	    if ( status == MBM_NORMAL )  {
	      m_evtCount++;
	    }
	  }
	}
	else if ( file_handle )  {
	  // undo reading of the first integer before saving rest of file
	  ::lseek(file_handle,-sizeof(evt_size),SEEK_CUR);
	}
      }
    }
    safeRestOfFile(file_handle);
    // Bad file: Cannot read input (m_evtCount==0)
    m_evtCount = 0;
  }
  return StatusCode::SUCCESS;
}

/// IRunable implementation : Run the class implementation
StatusCode HltBufferedIOReader::run()   {  
  try {
    return i_run();
  }
  catch(const exception& e) {
    error("Exception while reading MEP files:"+string(e.what()));
    throw;
  }
  catch(...) {
    error("UNKNWON Exception while reading MEP files.");
    throw;
  }
}
