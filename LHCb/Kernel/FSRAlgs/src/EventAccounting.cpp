// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// event model
#include "Event/EventCountFSR.h"

// local
#include "EventAccounting.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventAccounting
//
// 2009-11-11 : Rob Lambert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventAccounting )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventAccounting::EventAccounting( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(0),
    m_FSRName(""),
    m_eventFSR(0),
    m_count_files(),
    m_count_events(0),
    m_count_output(0),
    m_defaultStatusStr("UNCHECKED"),
    m_defaultStatus(LHCb::EventCountFSR::UNCHECKED),
    m_incSvc(0)
{
  // it is assumed that we are only called for a single BXType and that the
  // output data container gets this name
  declareProperty( "OutputDataContainer", m_FSRName = LHCb::EventCountFSRLocation::Default );
  //Override the status by the DefaultStatus in the FSR at the end of the job
  declareProperty("OverrideStatus",
                  m_overrideStatus=false);
  //The status to start with if nothing else is known
  declareProperty("DefaultStatus",
                  m_defaultStatusStr="UNCHECKED");
  
}
//=============================================================================
// Destructor
//=============================================================================
EventAccounting::~EventAccounting() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventAccounting::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  
  m_defaultStatus=LHCb::EventCountFSR::StatusFlagToType(m_defaultStatusStr);
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Default status: " << m_defaultStatus << endmsg;

  // prepare TDS for FSR
  m_eventFSR = new LHCb::EventCountFSR();
  m_eventFSR->setStatusFlag(m_defaultStatus);
  put(m_fileRecordSvc, m_eventFSR, m_FSRName);

  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true );
  
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  m_incSvc->addListener( this, IncidentType::WroteToOutputFile);
  if ( msgLevel(MSG::DEBUG) ) debug() << "registered with incSvc" << endmsg;
  //if not then the counting is not reliable
#else
  m_eventFSR->setStatusFlag(LHCb::EventCountFSR::UNRELIABLE);
  warn() << "cannot register with incSvc" << endmsg;
#endif //GAUDI_FILE_INCIDENTS

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventAccounting::execute() {
  //maintain a simple event count
  m_count_events++;
  m_eventFSR->setInput(m_count_events);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventAccounting::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  //if more than one file is written, the count is unreliable
  if (m_count_files.size()!=1) 
    m_eventFSR->setStatusFlag(LHCb::EventCountFSR::UNRELIABLE);
  
  //this can be overwritten with options for production
  if (m_overrideStatus)
  {
    info() << "Overriding status: " << m_eventFSR->statusFlag() << " with: " << m_defaultStatus << endmsg;
    m_eventFSR->setStatusFlag(m_defaultStatus);
  }
  
  
  // some printout of FSRs
  if ( msgLevel(MSG::DEBUG) ) 
  {
    debug() << "number of output files seen: " << m_count_files.size() << endmsg;
    // FSR - use the class method which prints it
    debug() << "FSR: " << *m_eventFSR << endmsg; 
  }

  // check if the FSRs can be retrieved from the TS
  if ( msgLevel(MSG::DEBUG) ) 
  {
    LHCb::EventCountFSR* readFSR = get<LHCb::EventCountFSR>(m_fileRecordSvc, m_FSRName);
    debug() << "READ FSR: " << *readFSR << endmsg; 

  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

// ==========================================================================
// IIncindentListener interface
// ==========================================================================
void EventAccounting::handle( const Incident& incident )
{
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  if(incident.type()==IncidentType::WroteToOutputFile)
  {
    //maintain a per-file counter for now... could be used later to write many FSRs
    m_count_files[incident.source()]+=1;
    //this number will be incorrect if there is more than one file produced
    m_count_output++;
    //set every time, and worry about it only in finalize
    m_eventFSR->setOutput(m_count_output);
  }
#endif

}
