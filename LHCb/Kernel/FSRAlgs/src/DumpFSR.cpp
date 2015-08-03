// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/LumiFSR.h"
#include "Event/EventCountFSR.h"
#include "Event/TimeSpanFSR.h"

// local
#include "DumpFSR.h"

// to write a file
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

//-----------------------------------------------------------------------------
// Implementation file for class : DumpFSR
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpFSR )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpFSR::DumpFSR( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(NULL),
    m_current_fname(""),
    m_count_files(0),
    m_count_events(0),
    m_events_in_file(0),
    m_navigatorTool(NULL),
    m_incSvc(NULL)
{
  // need to get the registry
  declareProperty( "RawEventLocation"   , m_rawEventLocation = LHCb::RawEventLocation::Default );
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"  );
  declareProperty( "FSRName"            , m_FSRName           = "/LumiFSR"     );
  declareProperty( "LowFSRName"         , m_LowFSRName        = "/LumiLowFSR"     );
  declareProperty( "EventCountFSRName"  , m_EventCountFSRName = "/EventCountFSR");
  declareProperty( "TimeSpanFSRName"    , m_TimeSpanFSRName   = "/TimeSpanFSR");
  declareProperty( "AsciiFileName"      , m_ascii_fname       = "");
  declareProperty( "DumpRequests"       , m_dumprequests      = "F");
}
//=============================================================================
// Destructor
//=============================================================================
DumpFSR::~DumpFSR() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpFSR::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  // prepare navigator tool
  m_navigatorTool = tool<IFSRNavigator>( "FSRNavigator" , "FSRNavigator" );
  // incident service
  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true );
  
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  m_incSvc->addListener( this, IncidentType::BeginInputFile);
  m_incSvc->addListener( this, IncidentType::EndInputFile);
  if ( msgLevel(MSG::DEBUG) ) debug() << "registered with incSvc" << endmsg;
  //if not then the counting is not reliable
#else
  warn() << "cannot register with incSvc" << endmsg;
#endif //GAUDI_FILE_INCIDENTS

  // counting 
  m_current_fname = "";
  m_count_files = 0;
  m_count_events = 0;
  m_events_in_file = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpFSR::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_count_events++;
  m_events_in_file++;

  // wait after exactly one event on a file
  if ( m_events_in_file == 1 ) {
    if ( m_dumprequests.find("E") != std::string::npos ) dump_file( "After First Event" );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DumpFSR::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << "number of files seen: " << m_count_files << endmsg;
  info() << "number of events seen: " << m_count_events << endmsg;

  if ( m_dumprequests.find("F") != std::string::npos ) dump_file("Finalize");

  if ( m_ascii_fname != "" ) {
    write_file();
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

// ==========================================================================
// IIncindentListener interface
// ==========================================================================
void DumpFSR::handle( const Incident& incident )
{
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  if(incident.type()==IncidentType::BeginInputFile)
  {
    m_current_fname = incident.source();
    if ( msgLevel(MSG::DEBUG) ) debug() << "==>from handle " << m_current_fname << endmsg;
    m_count_files++;
    m_events_in_file = 0;
    if ( m_dumprequests.find("B") != std::string::npos ) dump_file( "BeginInputFile" );

  }
  if(incident.type()==IncidentType::EndInputFile)
  {
    m_current_fname = incident.source();
    if ( msgLevel(MSG::DEBUG) ) debug() << "==>from handle " << m_current_fname << endmsg;
    if ( m_dumprequests.find("C") != std::string::npos ) dump_file( "EndInputFile" );

  }
#endif

}

//=============================================================================
void DumpFSR::dump_file( std::string txt ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==>" << txt << " " << m_current_fname << endmsg;

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName;
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  for(std::vector< std::string >::iterator iAddr = addresses.begin() ; 
      iAddr != addresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::INFO) ) {
      info() << "lu address: " << (*iAddr) << endmsg;
      std::string lumiRecordAddress = *iAddr;
      // read LumiFSR 
      LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, lumiRecordAddress);
      if ( NULL == lumiFSRs ) {
        Warning("A lumi record was not found").ignore();
        if ( msgLevel(MSG::DEBUG) ) debug() << lumiRecordAddress << " not found" << endmsg ;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << lumiRecordAddress << " found" << endmsg ;
        // look at all LumiFSRs (normally only one)
        LHCb::LumiFSRs::iterator lufsr;
        for ( lufsr = lumiFSRs->begin(); lufsr != lumiFSRs->end(); lufsr++ ) {
          info() << lumiRecordAddress << ": LumiFSR: " << *(*lufsr) << endmsg;
        }
      }
    }
  }  

  // make an inventory of the FileRecord store (LowLumi)
  std::vector< std::string > loAddresses = m_navigatorTool->navigate(fileRecordRoot, m_LowFSRName);
  for(std::vector< std::string >::iterator iAddr = loAddresses.begin() ; 
      iAddr != loAddresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::INFO) ) {
      info() << "lo address: " << (*iAddr) << endmsg;
      std::string lumiRecordAddress = *iAddr;
      // read LumiLowFSR 
      LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, lumiRecordAddress);
      if ( NULL == lumiFSRs ) {
        Warning("A lumi record was not found").ignore();
        if ( msgLevel(MSG::DEBUG) ) debug() << lumiRecordAddress << " not found" << endmsg ;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << lumiRecordAddress << " found" << endmsg ;
        // look at all LumiFSRs (normally only one)
        LHCb::LumiFSRs::iterator lufsr;
        for ( lufsr = lumiFSRs->begin(); lufsr != lumiFSRs->end(); lufsr++ ) {
          info() << lumiRecordAddress << ": LumiLowFSR: " << *(*lufsr) << endmsg;
        }
      }
    }
  }  
  
  //touch all EventCountFSRs
  std::vector< std::string > evAddresses = m_navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; 
      iAddr != evAddresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::INFO) ) {
      info() << "ev address: " << (*iAddr) << endmsg;
      std::string eventCountRecordAddress = *iAddr;
      // read EventCountFSR 
      LHCb::EventCountFSR* eventCountFSR = getIfExists<LHCb::EventCountFSR>(m_fileRecordSvc, eventCountRecordAddress);
      if ( NULL == eventCountFSR ) {
        Warning("An EventCount Record was not found").ignore();
        if ( msgLevel(MSG::DEBUG) ) debug() << eventCountRecordAddress << " not found" << endmsg ;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << eventCountRecordAddress << " found" << endmsg ;
        // look at the EventCountFSR
        info() << eventCountRecordAddress << ": EventCountFSR: " << *eventCountFSR << endmsg;
      }
    }
  }  
  
  //touch all TimeSpanFSRs (independently of the LumiFSRs)
  std::vector< std::string > tsAddresses = m_navigatorTool->navigate(fileRecordRoot, m_TimeSpanFSRName);
  for(std::vector< std::string >::iterator iAddr = tsAddresses.begin() ; 
      iAddr != tsAddresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::INFO) ) {
      info() << "ts address: " << (*iAddr) << endmsg;
      std::string timeSpanRecordAddress = *iAddr;
      // read TimeSpanFSR 
      LHCb::TimeSpanFSRs* timeSpanFSRs = getIfExists<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
      if ( NULL == timeSpanFSRs ) {
        Warning("A TimeSpan Record was not found").ignore();
        if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " not found" << endmsg ;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
        // look at all TimeSpanFSRs (normally only one)
        LHCb::TimeSpanFSRs::iterator tsfsr;
        for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
          info() << timeSpanRecordAddress << ": TimeSpanFSR: " << *(*tsfsr) << endmsg;
          ulonglong t0 = (*tsfsr)->earliest();
          ulonglong t1 = (*tsfsr)->latest();
          if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << "interval: " << t0 << "-" << t1 << endmsg;
        }
      }
    }
  }
}


//=============================================================================
void DumpFSR::write_file( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "write to file: " << m_ascii_fname << endmsg;

  if ( m_ascii_fname != "" ) {
    std::ofstream outfile( m_ascii_fname.c_str() );
    if (outfile) {
      always() << "asciifile: " << m_ascii_fname << " - opened" << endmsg;


      // root of store
      std::string fileRecordRoot = m_FileRecordName;
      // make an inventory of the FileRecord store
      std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
      for(std::vector< std::string >::iterator iAddr = addresses.begin() ; 
          iAddr != addresses.end() ; ++iAddr ){
        std::string lumiRecordAddress = *iAddr;
        // read LumiFSR 
        LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, lumiRecordAddress);
        if ( NULL == lumiFSRs ) {
          Warning("A lumi record was not found").ignore();
          if ( msgLevel(MSG::DEBUG) ) debug() << lumiRecordAddress << " not found" << endmsg ;
        } else {
          LHCb::LumiFSRs::iterator lufsr;
          for ( lufsr = lumiFSRs->begin(); lufsr != lumiFSRs->end(); lufsr++ ) {
            outfile << lumiRecordAddress << ": LumiFSR: " << *(*lufsr) << std::endl;
          }
        }
      }

      // make an inventory of the FileRecord store (LowLumi)
      addresses = m_navigatorTool->navigate(fileRecordRoot, m_LowFSRName);
      for(std::vector< std::string >::iterator iAddr = addresses.begin() ; 
          iAddr != addresses.end() ; ++iAddr ){
        std::string lumiRecordAddress = *iAddr;
        // read LumiFSR 
        LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, lumiRecordAddress);
        if ( NULL == lumiFSRs ) {
          Warning("A lumi record was not found").ignore();
          if ( msgLevel(MSG::DEBUG) ) debug() << lumiRecordAddress << " not found" << endmsg ;
        } else {
          LHCb::LumiFSRs::iterator lufsr;
          for ( lufsr = lumiFSRs->begin(); lufsr != lumiFSRs->end(); lufsr++ ) {
            outfile << lumiRecordAddress << ": LumiLowFSR: " << *(*lufsr) << std::endl;
          }
        }
      }
  
      // EventCountFSRs
      std::vector< std::string > evAddresses = m_navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
      for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; 
          iAddr != evAddresses.end() ; ++iAddr ){
        std::string eventCountRecordAddress = *iAddr;
        // read EventCountFSR 
        LHCb::EventCountFSR* eventCountFSR = getIfExists<LHCb::EventCountFSR>(m_fileRecordSvc, eventCountRecordAddress);
        if ( NULL == eventCountFSR ) {
          Warning("An EventCount Record was not found").ignore();
          if ( msgLevel(MSG::DEBUG) ) debug() << eventCountRecordAddress << " not found" << endmsg ;
        } else {
          // look at the EventCountFSR
          outfile << eventCountRecordAddress << ": EventCountFSR: " << *eventCountFSR << std::endl;
        }
      }  
  
      // TimeSpanFSRs
      std::vector< std::string > tsAddresses = m_navigatorTool->navigate(fileRecordRoot, m_TimeSpanFSRName);
      for(std::vector< std::string >::iterator iAddr = tsAddresses.begin() ; 
          iAddr != tsAddresses.end() ; ++iAddr ){
        std::string timeSpanRecordAddress = *iAddr;
        // read TimeSpanFSR 
        LHCb::TimeSpanFSRs* timeSpanFSRs = getIfExists<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
        if ( NULL == timeSpanFSRs ) {
          Warning("A TimeSpan Record was not found").ignore();
          if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " not found" << endmsg ;
        } else {
          // look at all TimeSpanFSRs (normally only one)
          LHCb::TimeSpanFSRs::iterator tsfsr;
          for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
            outfile << timeSpanRecordAddress << ": TimeSpanFSR: " << *(*tsfsr) << std::endl;
          }
        }
      }
  
      // close output file
      outfile.close();
      always() << "asciifile: " << m_ascii_fname << " - closed" << endmsg;
      
    } else {
      always() << "asciifile: " << m_ascii_fname << " - not opened" << endmsg;
    }
  }
}


//=============================================================================
std::string DumpFSR::fileID() {
  // get the fileID from the event store
  std::string event_fname("");

  // get ODIN
  LHCb::ODIN* odin = getIfExists<LHCb::ODIN> (LHCb::ODINLocation::Default);
  if( NULL == odin ){
    // should remain flagged as an error and stop the run
    error() << "ODIN cannot be loaded" << endmsg;
    return event_fname;
  }
  // obtain the run number from ODIN
  unsigned int run = odin->runNumber();  
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "ODIN RunNumber: " << run << endmsg;

  // registry from raw data - only correct if file catalogue used 
  IOpaqueAddress* eAddr = 0;
  LHCb::RawEvent* rawEvt = getIfExists<LHCb::RawEvent>(m_rawEventLocation);
  if( NULL == rawEvt ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << m_rawEventLocation << " not found" << endmsg ;
    // then try from other bank
    DataObject* event = getIfExists<DataObject>("/Event");
    if ( NULL == event ){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "/Event" << " not found" << endmsg ;
      eAddr = odin->registry()->address();
    } else {
      // get the container
      eAddr = event->registry()->address();
    }
  } else {
    eAddr = rawEvt->registry()->address();
  }

  // obtain the fileID
  if ( eAddr ) {
    event_fname = eAddr->par()[0];
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "RunInfo record from Event: " << event_fname << endmsg;
  } else {
    error() << "Registry cannot be loaded from Event" << endmsg;
    return event_fname;
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "ODIN RunNumber: " << run 
                                          << " with RunInfo record: " << event_fname << endmsg;

  return event_fname;
}
