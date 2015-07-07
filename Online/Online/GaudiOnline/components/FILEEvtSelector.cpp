// $Id: MBMEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
//====================================================================
//  MBMEvtSelector
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The MBMEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria" with input from MBM buffers.
//
//  Author     : M.Frank
//  Created    : 4/01/99
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
#ifndef GAUDIONLINE_FILEEVTSELECTOR_H
#define GAUDIONLINE_FILEEVTSELECTOR_H 1

// Include files
#include "GaudiOnline/OnlineBaseEvtSelector.h"
#include "GaudiOnline/FileScanner.h"
#include "GaudiOnline/MEPManager.h"
#include "MBM/Consumer.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {
  // Forward declarations
  class FILEEvtSelector;
  class FILEContext;


  /** @class FILEContext
   *
   * @author M.Frank
   */
  class FILEContext : public OnlineContext  {
  private:
    /// The event descriptor
    FILEEvtSelector*   m_fileSel;
    MBM::EventDesc&    m_dsc;

    bool handleCancel();
    StatusCode error(const std::string& msg)   {
      return m_sel->error(msg);
    }
  public:
    /// Standard constructor
    FILEContext(FILEEvtSelector* pSelector);
    /// Standard destructor 
    virtual ~FILEContext();
    /// Connect to event data source
    virtual StatusCode connect(const std::string& input);

    /// Receive event information
    virtual StatusCode receiveEvent();
    /// Rearm event data request
    virtual StatusCode rearmEvent();
    /// Free event resources
    virtual StatusCode freeEvent();
    /// Force release of event resources
    virtual StatusCode releaseEvent();
    /// Flag event resources
    virtual StatusCode flagEvent(int flag);
    /// Close connection to event data source
    virtual void close();
  };

  /** @class FILEEvtSelector  FILEEvtSelector.h
   *
   * @author  M.Frank
   * @vrsion  1.0
   */
  class FILEEvtSelector : public OnlineBaseEvtSelector, public FileScanner  {
    friend class FILEContext;
  public:
    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService overload: start MEP manager service
    virtual StatusCode start();

    /// IService overload: stop MEP manager service
    virtual StatusCode stop();

    // IService implementation: Service finalization
    virtual StatusCode finalize();

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
     * 
     * @return StatusCode indicating success or failure
     */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /// Release existing event iteration context
    /** @param refCtxt   [IN/OUT]  Reference to the context
     * 
     * @return StatusCode indicating success or failure
     */
    virtual StatusCode releaseContext(Context*& ctxt) const;

    /** Will set a new criteria for the selection of the next list of events and will change
     * the state of the context in a way to point to the new list.
     * 
     * @param cr The new criteria string.
     * @param c  Reference pointer to the Context object.
     */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

    /// Service Constructor
    FILEEvtSelector(const std::string& name, ISvcLocator* svcloc);

    /// Standard destructor
    virtual ~FILEEvtSelector()    {}

    /// Allocate space when reading events
    virtual MBM::EventDesc& allocateSpace(size_t buffer_size);

    /// Read next event
    StatusCode readNext();
    StatusCode scan(const std::string& input);

  protected:
    /// Data Members

    /// Current input context
    mutable FILEContext* m_currContext;
    /// Handle of currently read file.
    int                  m_file;
    /// Property: Max. number of events to be processed. Taken from ApplicationMgr
    int                  m_evtMax;
    /// Current event counter
    int                  m_numEvt;
    /// Event descriptor
    MBM::EventDesc       m_event;
    /// Buffer of the event data
    int*                 m_buff;
    /// Maximal size of the buffered event data
    size_t               m_buffSize;
  };
}
#endif // GAUDIONLINE_FILEEVTSELECTOR_H

// $Id: FILEEvtSelector.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
//====================================================================
//  FILEEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The GaudiOnline::FILEEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank, B.Jost
//====================================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FILEEvtSelector)

using namespace std;
using namespace LHCb;

/// Standard constructor
FILEContext::FILEContext(FILEEvtSelector* s)
  : OnlineContext(s), m_fileSel(s), m_dsc(s->m_event)
{
  m_baseAddr = 0;
}

/// Standard constructor
FILEContext::~FILEContext()  {
}

StatusCode FILEContext::releaseEvent()  {
  m_events.clear();
  return freeEvent();
}

StatusCode FILEContext::freeEvent()  {
  m_evdesc.setHeader(0);
  m_evdesc.setSize(0);
  return StatusCode::SUCCESS;
}

StatusCode FILEContext::flagEvent(int flag)  {
  if ( m_sel->eventContext() == this ) {
    // Can only flag Descriptor events!
    const MBM::EventDesc& e = m_dsc;
    if ( m_sel->mustDecode() && e.type == EVENT_TYPE_EVENT )  {
      MEP_SINGLE_EVT* sevt = (MEP_SINGLE_EVT*)e.data;
      MEPEVENT* m = (MEPEVENT*)(m_evdesc.buffer() + sevt->begin);
      m->events[sevt->evID].status = EVENT_TYPE_BADPROC;
      m->events[sevt->evID].signal = flag;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode FILEContext::rearmEvent()  {
  if ( !handleCancel() )   {
    if ( m_events.empty() ) {
      if ( m_fileSel->readNext().isSuccess() )
        return StatusCode::SUCCESS;
      releaseEvent();
      close();
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

bool FILEContext::handleCancel()   {
  if ( m_sel->isCancelled() ) {
    // In case we got cancelled, just return error and 
    // let upper layers handle the problem
    releaseEvent();
    return true;
  }
  return false;
}

StatusCode FILEContext::receiveEvent()  {
  m_banks.clear();
  int m_partID = 1;
  try  {
    bool decode = m_sel->mustDecode();
    m_sel->increaseReqCount();
    if ( m_events.empty() ) {
      if ( handleCancel() ) return StatusCode::FAILURE;
    }
    // The event is a descriptor event, which must be decoded using the MEP data:
    if ( decode && m_dsc.type == EVENT_TYPE_EVENT )
      return convertDescriptor(m_partID,m_dsc);
    // The event is a MEP with multiple events, which must be decoded:
    else if ( decode && m_dsc.type == EVENT_TYPE_MEP )
      return convertMEP(m_partID,m_dsc);
    // Or: simple case data are data - as it should be
    else
      return convertMDF(m_partID,m_dsc);
  }
  catch(const exception& e)  {
    if ( handleCancel() ) return StatusCode::FAILURE;
    return error("Failed to read next event:"+string(e.what()));
  }
  catch(...)  {
    if ( handleCancel() ) return StatusCode::FAILURE;
    return error("Failed to read next event - Unknown exception.");
  }
  return StatusCode::FAILURE;
}

StatusCode FILEContext::connect(const string& input)  {
  close();
  return m_fileSel->scan(input);
}

void FILEContext::close()  {
  m_events.clear();
}

FILEEvtSelector::FILEEvtSelector(const string& nam, ISvcLocator* svc)
  : OnlineBaseEvtSelector(nam,svc), m_currContext(0), 
    m_file(0), m_evtMax(-1), m_numEvt(0), m_buff(0), m_buffSize(0)
{
  m_input = "";
  m_decode = true;
  m_gotoPause = true;
  declareProperty("Directory",   m_directory = "/localdisk");
  declareProperty("FilePrefix",  m_filePrefix = "Run_");
  declareProperty("BrokenHosts", m_brokenHostsFile = "");
  declareProperty("DeleteFiles", m_deleteFiles = false);
  declareProperty("AllowedRuns", m_allowedRuns);
  declareProperty("EvtMax",      m_evtMax);
  m_allowedRuns.push_back("*");
}

// IService implementation: Db event selector override
StatusCode FILEEvtSelector::initialize()    {
  // Initialize base class
  StatusCode sc = OnlineBaseEvtSelector::initialize();
  // Patch the string array with allowed runs
  for(size_t i=0; i<m_allowedRuns.size(); ++i)   {
    const char* crun = m_allowedRuns[i].c_str();
    if ( ::isdigit(crun[0]) && crun[0] != '*' )   {
      char text[PATH_MAX];
      long runno = ::strtol(crun,0,10);
      ::snprintf(text,sizeof(text),"%s%07ld_",m_filePrefix.c_str(),runno);
      m_allowedRuns[i] = text;
    }
  }
  return sc;
}

/// IService overload: start MEP manager service
StatusCode FILEEvtSelector::start() {
  StatusCode sc = OnlineBaseEvtSelector::start();
  SmartIF<IProperty> appMgrProperty(serviceLocator());
  setProperty(appMgrProperty->getProperty("EvtMax")).ignore();  
  m_numEvt = 0; 
  if ( m_currContext ) {
    sc = m_currContext->connect(m_input);
    if ( sc.isSuccess() ) {
      return sc;
    }
  }
  return error("Attempt to connect FILE buffer "+input()+" with invalid context.");
}

/// IService overload: stop MEP manager service
StatusCode FILEEvtSelector::stop() {
  if ( m_currContext ) {
    m_currContext->close();
  }
  if ( m_buff ) ::free(m_buff);
  if ( m_file ) ::close(m_file);
  m_buff = 0;
  m_file = 0;
  m_buffSize = 0;
  return OnlineBaseEvtSelector::stop();
}

// IService implementation: Service finalization
StatusCode FILEEvtSelector::finalize()    {
  if ( m_currContext ) m_currContext->close();
  return OnlineBaseEvtSelector::finalize();
}

/// Allocate space when reading events
MBM::EventDesc& FILEEvtSelector::allocateSpace(size_t buffer_size)  {
  if ( m_buffSize < buffer_size )  {
    m_event.data = m_buff = (int*)::realloc(m_buff,m_buffSize=buffer_size);
    m_event.len  = m_buffSize;
  }
  return m_event;
}

/// Read next event
StatusCode FILEEvtSelector::readNext()  {
 NextFile:
  if ( 0 == m_file )  {
    m_file = openFile();
    if ( m_file <= 0 )
      return error(m_comment);
    info(m_comment);
  }
  bool stop_processing = m_evtMax > 0 && ++m_numEvt > m_evtMax;
  if ( !stop_processing )  {
    int ret = readEvent(m_file, 0);
    if ( ret == FileScanner::EVENT_OK )
      return StatusCode::SUCCESS;
  }
  if ( m_file ) ::close(m_file);
  m_file = 0;
  if ( !stop_processing && !m_files.empty() )   {
    goto NextFile;
  }
  return StatusCode::FAILURE;
}

StatusCode FILEEvtSelector::scan(const std::string& input)  {
  if ( m_file ) ::close(m_file);
  m_file = 0;
  if ( m_files.empty() )    {
    struct stat stat_buff;
    if ( 0 == ::stat(input.c_str(),&stat_buff) ) {
      if ( S_ISDIR(stat_buff.st_mode) )   {
        m_directory = input;
        scanFiles();
      }
      else  {
        m_files.insert(input);
      }
    }
  }
  // Must return SUCCESS.
  // Otherwise we will not get the PAUSE incident
  // when no events are present.
  return StatusCode::SUCCESS;
}

/// Create event selector iteration context
StatusCode FILEEvtSelector::createContext(Context*& refpCtxt) const  {
  FILEContext* ctxt = new FILEContext(const_cast<FILEEvtSelector*>(this));
  m_currContext = ctxt;
  refpCtxt = m_context = ctxt;
  return StatusCode::SUCCESS;
}

StatusCode FILEEvtSelector::releaseContext(Context*& ctxt) const  {
  m_currContext = 0;
  return OnlineBaseEvtSelector::releaseContext(ctxt);
}

StatusCode FILEEvtSelector::resetCriteria(const string& crit,Context& ct) const {
  OnlineContext* ctxt = dynamic_cast<OnlineContext*>(&ct);
  if ( ctxt )  {
    ctxt->close();
    info("Connect FILE buffer "+input()+" -- "+crit+" -- "+m_input);
    if ( ctxt->connect(crit).isSuccess() )  {
      return StatusCode::SUCCESS;
    }
    return error("Failed to connect to:"+crit);
  }
  return error("Invalid iteration context. Cannot connect to:"+crit);
}
