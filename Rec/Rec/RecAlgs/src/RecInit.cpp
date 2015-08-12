// Include files

// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/IOpaqueAddress.h"

// from EventBase
#include "Event/ProcessHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// from RecEvent
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"

// local
#include "RecInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecInit
//
// 2006-03-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecInit )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecInit::RecInit( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator )
  , m_memoryTool(0)
  , m_incidentSvc(0)
{
  declareProperty( "RawEventLocation",  m_rawEventLocation = "", 
                   "OBSOLETE. Use RawEventLocations instead" );
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");
  declareProperty( "AbortOnFID"  ,  m_abortOnFID = true, 
                   "If I can't find the raw file ID, do I abort? Default true=yes.");
}

//=============================================================================
// Destructor
//=============================================================================
RecInit::~RecInit() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecInit::initialize()
{
  const StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  m_memoryTool = tool<IGenericTool>( "MemoryTool", "BrunelMemory", this, true );

  // Pointer to IncidentSvc
  m_incidentSvc = svc<IIncidentSvc>("IncidentSvc",true);

  // Initialise the RawEvent locations
  bool usingDefaultLocation = m_rawEventLocations.empty() && m_rawEventLocation.empty();
  if (! m_rawEventLocation.empty()) {
    warning() << "The RawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }

  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Calo); // present on all DST types
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default); // RAW data
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecInit::execute()
{
  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the ODIN bank
  LHCb::ODIN* odin = 0;
  try
  {
    odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  }

  catch( const GaudiException& Exception )
  {
    m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
    this->setFilterPassed( false );
    //dummy printout to remove warning in windows..
    if(msgLevel(MSG::DEBUG)) debug() << Exception.message() << endmsg;
    return Error( "ODIN missing, skipping event", StatusCode::SUCCESS );
  }

  const unsigned int runNumber = odin->runNumber();
  const ulonglong    evtNumber = odin->eventNumber();

  this->printEventRun( evtNumber, runNumber, 0, odin->eventTime() );
  
  // Initialize the random number
  std::vector<long int> seeds = getSeeds( runNumber, evtNumber );
  sc = this->initRndm( seeds );
  if ( sc.isFailure() ) return sc;  // error printed already by initRndm
  
  // get the file ID from the raw event
  std::string event_fname="";

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Getting RawEvent" << endmsg;
  LHCb::RawEvent* rawEvent = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    rawEvent = getIfExists<LHCb::RawEvent>(*p);
    if( NULL != rawEvent ) break;
  }

  if ( NULL == rawEvent ) {
    if(m_abortOnFID) return Error("RawEvent cannot be loaded, fileID cannot be found");
    Warning("RawEvent cannot be loaded, fileID cannot be found",StatusCode::SUCCESS).ignore();
    event_fname = "ERROR, RawEvent not found";
  }
  else {
    IOpaqueAddress* eAddr = rawEvent->registry()->address();
    // obtain the fileID
    if ( eAddr ) {
      event_fname = eAddr->par()[0];
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record from Event: " << event_fname << endmsg;
    } 
    else {
      if(m_abortOnFID) return Error("Registry cannot be loaded from Event, fileID cannot be found");
      Warning("Registry cannot be loaded from Event, fileID cannot be found",StatusCode::SUCCESS).ignore();
    }
  }
  const std::string rawID=event_fname;
  
  // Create the Reconstruction event header
  LHCb::RecHeader* header = new LHCb::RecHeader();
  header->setApplicationName( this->appName() );
  header->setApplicationVersion( this->appVersion() );
  header->setRunNumber( runNumber );
  header->setEvtNumber( evtNumber );
  header->setCondDBTags( this->condDBTags() );
  header->setGpsTime( odin->gpsTime() );
  header->setRawID( rawID );
  put( header, LHCb::RecHeaderLocation::Default );

  // Create a ProcStatus if it does not already exist
  if( !exist<LHCb::ProcStatus>( LHCb::ProcStatusLocation::Default ) )
  {
    LHCb::ProcStatus* procStat = new LHCb::ProcStatus();
    put( procStat, LHCb::ProcStatusLocation::Default );
  }

  return sc;
}

//=============================================================================
