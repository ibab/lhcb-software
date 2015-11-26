// Include files 
 
// event model
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/EventCountFSR.h"

// local
#include "LumiMergeFSR.h"
#include "FSRNavigator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiMergeFSR
// 
// 2010-10-05 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiMergeFSR )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiMergeFSR::LumiMergeFSR( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(0),
    m_BXTypes(0),
    m_subtractBXTypes(0),
    m_lumiFSRsVec(0),
    m_FSRNameVec(0),
    m_timeSpanFSRs(NULL),
    m_navigatorTool(NULL)
{
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"   );
  declareProperty( "FSRName"            , m_FSRName           = "/LumiFSR"       );
  declareProperty( "TimeSpanFSRName"    , m_TimeSpanFSRName   = "/TimeSpanFSR"   ); //LHCb::TimeSpanFSRLocation::Default );
  declareProperty( "EventCountFSRName"  , m_EventCountFSRName = "/EventCountFSR" ); //LHCb::EventCountFSRLocation::Default );
  declareProperty( "PrimaryBXType"      , m_PrimaryBXType     = "BeamCrossing"   );
  declareProperty( "SubtractBXTypes"    , m_subtractBXTypes ) ;
  declareProperty( "NavigatorToolName"  , m_ToolName          = "FSRNavigator"   );
  
}
//=============================================================================
// Destructor
//=============================================================================
LumiMergeFSR::~LumiMergeFSR() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiMergeFSR::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // initialize lists - the order is important: the first is the one to normalize to
  if (m_subtractBXTypes.empty()) {
    m_subtractBXTypes.push_back("Beam1");
    m_subtractBXTypes.push_back("Beam2");
    m_subtractBXTypes.push_back("NoBeam");
  }
  // ensure consistency
  m_BXTypes.push_back(m_PrimaryBXType);
  info() << "Primary  BXType " << m_PrimaryBXType << endmsg;
  for(std::vector< std::string >::iterator bx = m_subtractBXTypes.begin() ; 
      bx!= m_subtractBXTypes.end() ; ++bx ){  
    info() << "Subtract BXType " << (*bx) << endmsg;
    if ( (*bx) != "None" ) m_BXTypes.push_back(*bx);
  }
   
  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  
  // prepare navigator tool
  m_navigatorTool = tool<IFSRNavigator>( "FSRNavigator" , m_ToolName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiMergeFSR::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiMergeFSR::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << "========== Merging FSRs ==========" << endmsg;

  // integrate all FSRs in one go
  const StatusCode mergeSC = merge();
  return ( mergeSC && GaudiAlgorithm::finalize() ); // must be called after all other actions
}

//=========================================================================
//  merge the FSR data
//=============================================================================
StatusCode LumiMergeFSR::merge() {
  // a file can contain multiple sets of LumiFSRs - typically after reprocessing multiple input files
  // merge the FSRs of all input files at the same time

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName; 
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  // print
  if ( msgLevel(MSG::DEBUG) ) {
    for(std::vector< std::string >::iterator iAddr = addresses.begin() ; iAddr != addresses.end() ; ++iAddr ){
      debug() << "address: " << (*iAddr) << endmsg;
    }
  }
  // get timespans for later removal - and store earliest and latest times
  unsigned long long all_earliest = 0;                         // earliest time seen
  unsigned long long all_latest = 0;                           // latest time seen
  std::vector< std::string > tsAddresses = m_navigatorTool->navigate(fileRecordRoot, m_TimeSpanFSRName);
  for(std::vector< std::string >::iterator iAddr = tsAddresses.begin() ; iAddr != tsAddresses.end() ; ++iAddr ){
    LHCb::TimeSpanFSRs* timeSpanFSRs = getIfExists<LHCb::TimeSpanFSRs>(m_fileRecordSvc, *iAddr);    
    LHCb::TimeSpanFSRs::iterator tsfsr;
    for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
      // take earliest and latest
      unsigned long long t0 = (*tsfsr)->earliest();
      unsigned long long t1 = (*tsfsr)->latest();
      if ( all_earliest == 0 ) all_earliest = t0;     
      all_latest = std::max(t1, all_latest);
    }
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "address: " << (*iAddr) << endmsg;
    }
  }
  
  // prepare TDS for FSR
  for ( unsigned int ibx = 0 ; ibx < m_BXTypes.size() ; ++ibx ){  
    std::string bx = m_BXTypes[ibx];
    if ( msgLevel(MSG::DEBUG) ) debug() << "BXType " << bx << endmsg;
    LHCb::LumiFSRs* fsrs = new LHCb::LumiFSRs(); // keyed container for FSRs
    fsrs->reserve(100);
    m_lumiFSRsVec.push_back(fsrs);               // vector of keyed containers
    std::string name = m_FSRName + bx;           // 
    m_FSRNameVec.push_back(name);                // vector of names
    put(m_fileRecordSvc, fsrs, m_FileRecordName + name); // TS address of keyed container
  }
  // prepare TDS for TimeSpanFSR
  m_timeSpanFSRs = new LHCb::TimeSpanFSRs();
  m_timeSpanFSRs->reserve(100);
  put(m_fileRecordSvc, m_timeSpanFSRs, m_FileRecordName + m_TimeSpanFSRName);

  // look first for a primary BX in the list and then look for the corresponding background types
  std::string primaryFileRecordAddress("undefined");
  for(std::vector< std::string >::iterator a = addresses.begin() ; a!= addresses.end() ; ++a ){  
    if ( a->find(m_FSRName + m_PrimaryBXType) != std::string::npos ) {
      primaryFileRecordAddress = (*a);   // a primary BX is found
      bool needNewLumiFSR = false;
      // get primary LumiFSR number
      LHCb::LumiFSRs* primaryFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, primaryFileRecordAddress);
      int n_primaryFSRs = primaryFSRs->size();
      if ( msgLevel(MSG::DEBUG) ) debug() << primaryFileRecordAddress << ": " << n_primaryFSRs
                                          << " primary FSRs in container " << endmsg; 

      // search for the TimeSpanFSR 
      std::string timeSpanRecordAddress(primaryFileRecordAddress);
      timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), "" );
      timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_FSRName), m_FSRName.size(), m_TimeSpanFSRName );
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed time span address" << timeSpanRecordAddress << endmsg; 
      // read TimeSpanFSR 
      unsigned long n_tsFSR = 0;
      LHCb::TimeSpanFSRs* timeSpanFSRs = getIfExists<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
      if ( NULL == timeSpanFSRs ) {
        if ( msgLevel(MSG::ERROR) ) error() << timeSpanRecordAddress << " not found" << endmsg ;
      } else {
        //if ( msgLevel(MSG::VERBOSE) ) 
        if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
        int n_timeSpanFSRs = timeSpanFSRs->size();
        if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << ": " << n_timeSpanFSRs
                                            << " time span FSRs in container " << endmsg; 

        // decide if fatal error
        bool needCorrection = false;
        bool needNewTsFSR = false;
        if (n_primaryFSRs > n_timeSpanFSRs && n_timeSpanFSRs > 0) {
          fatal() << " number of primary FSRs " << n_primaryFSRs << " larger than number of timeSpanFSRs " << n_timeSpanFSRs 
                  << " - this cannot be right!" << endmsg;
          return StatusCode::FAILURE;
        }
        if (n_primaryFSRs < n_timeSpanFSRs  && n_primaryFSRs != 1 && n_primaryFSRs != 0) {
          fatal() << " number of primary FSRs " << n_primaryFSRs << " smaller than number of timeSpanFSRs " << n_timeSpanFSRs 
                  << " and not equal to one - this cannot be right!" << endmsg;
          for ( unsigned int ibx = 0; ibx < m_BXTypes.size() ; ++ibx ){  
            std::string bx = m_BXTypes[ibx];
            // construct the right name of the containers
            std::string fileRecordAddress(primaryFileRecordAddress);
            fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), bx );
          	if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address: " << fileRecordAddress << endmsg; 
            // read LumiFSR 
            LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
            if ( NULL == lumiFSRs ) {
              if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
            } else {
              fatal() << "found " << fileRecordAddress << " size " << lumiFSRs->size() << endmsg;
            }
          }
          return StatusCode::FAILURE;
        }
        if (n_primaryFSRs < n_timeSpanFSRs  && n_primaryFSRs == 0) {
          warning() << " number of primary FSRs " << n_primaryFSRs << " smaller than number of timeSpanFSRs " << n_timeSpanFSRs 
                    << " and not equal to one - try to correct!" << endmsg;

          // rest is for debugging
          for ( unsigned int ibx = 0; ibx < m_BXTypes.size() ; ++ibx ){  
            std::string bx = m_BXTypes[ibx];
            info() << "BXType " << bx << endmsg;
            // construct the right name of the containers
            std::string fileRecordAddress(primaryFileRecordAddress);
            fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), bx );
          	if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address: " << fileRecordAddress << endmsg; 
            // read LumiFSR 
            LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
            if ( NULL == lumiFSRs ) {
              if ( msgLevel(MSG::FATAL) ) fatal() << fileRecordAddress << " not found" << endmsg ;
              return StatusCode::FAILURE;
            } else {
              // expect all empty
              if ( lumiFSRs->size() != 0 ) {
                fatal() << "found " << fileRecordAddress << " with different size " << lumiFSRs->size() << endmsg;
                return StatusCode::FAILURE;
              }
            }
          }
          // all are missing - can correct
          needNewLumiFSR = true;
        }
        // decide if corrective action needs to be taken
        if (n_primaryFSRs < n_timeSpanFSRs && n_primaryFSRs == 1) {
          warning() << " at address: " <<  primaryFileRecordAddress
                    << " number of primary FSRs " << n_primaryFSRs << " smaller than number of timeSpanFSRs " << n_timeSpanFSRs 
                    << " and exactly equal to one - corrective action being taken!" << endmsg;
          needCorrection = true;
        }
        if (n_primaryFSRs > n_timeSpanFSRs && n_timeSpanFSRs == 0) {
          warning() << " number of primary FSRs " << n_primaryFSRs << " larger than number of timeSpanFSRs " << n_timeSpanFSRs 
                    << " - corrective action taken!" << endmsg;
          needCorrection = true;
          needNewTsFSR = true;
        }

        // look at all TimeSpanFSRs (normally only one)
        LHCb::TimeSpanFSRs::iterator tsfsr;
        if ( needCorrection ) {
          // any correction
          if ( needNewTsFSR ) {
            // correction needed - create a new TimeSpanFSR with earliest and latest times
            LHCb::TimeSpanFSR* timeSpanFSR = new LHCb::TimeSpanFSR();
            (*timeSpanFSR) += all_earliest;
            (*timeSpanFSR) += all_latest;
            n_tsFSR++;
            m_timeSpanFSRs->insert(timeSpanFSR); // put a copy in TS container
            if ( msgLevel(MSG::INFO) ) info() << timeSpanRecordAddress << ": " << n_tsFSR 
                                                << " successfully created - total is now " 
                                                << m_timeSpanFSRs->size() << endmsg; 
          }
          else {
            // correction needed - take the sum of all timespanFSRs and create only one
            LHCb::TimeSpanFSR* timeSpanFSR = new LHCb::TimeSpanFSR();
            for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
              // prepare new time span FSR and put in TS
              *timeSpanFSR += *(*tsfsr);
            }
            n_tsFSR++;
            m_timeSpanFSRs->insert(timeSpanFSR); // put a copy in TS container
            if ( msgLevel(MSG::INFO) ) info() << timeSpanRecordAddress << ": " << n_tsFSR 
                                                << " successfully copied - total is now " 
                                                << m_timeSpanFSRs->size() << endmsg; 
          }
        }
        else {
          // no correction needed
          for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
            // prepare new time span FSR and put in TS
            n_tsFSR++;
            LHCb::TimeSpanFSR* timeSpanFSR = new LHCb::TimeSpanFSR();
            *timeSpanFSR = *(*tsfsr);
            m_timeSpanFSRs->insert(timeSpanFSR); // put a copy in TS container
          }
          if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << ": " << n_tsFSR 
                                              << " successfully copied - total is now " 
                                              << m_timeSpanFSRs->size() << endmsg; 
        }
      }    

      // now handle all Lumi FSRs
      std::string fileRecordAddress("undefined");
      // get all FSR objects
      for ( unsigned int ibx = 0; ibx < m_BXTypes.size() ; ++ibx ){  
        std::string bx = m_BXTypes[ibx];
        if ( msgLevel(MSG::DEBUG) ) debug() << "BXType " << bx << endmsg;
        LHCb::LumiFSRs* fsrs = m_lumiFSRsVec[ibx]; 
        unsigned long n_lumiFSR = 0;
        // construct the right name of the containers
        std::string fileRecordAddress(primaryFileRecordAddress);
        fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), bx );
      	if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address: " << fileRecordAddress << endmsg; 
        // read LumiFSR 
        LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
        if ( NULL == lumiFSRs ) {
          if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
        } else {
          if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
          // look at all FSRs for the BXType (normally only one)
          if (needNewLumiFSR ) {
            // create empty one
            n_lumiFSR++;
            LHCb::LumiFSR* lumiFSR = new LHCb::LumiFSR();
            fsrs->insert(lumiFSR); // insert empty FSR in TS
          } else {
            LHCb::LumiFSRs::iterator fsr;
            // just copy
            for ( fsr = lumiFSRs->begin(); fsr != lumiFSRs->end(); ++fsr ) {
              // create a new FSR and append to TS
              n_lumiFSR++;
              LHCb::LumiFSR* lumiFSR = new LHCb::LumiFSR();
              *lumiFSR = *(*fsr);
              fsrs->insert(lumiFSR); // insert in TS
            }  
          }
          // count the result
          if ( n_tsFSR == n_lumiFSR ) {
            if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << ": " << n_lumiFSR 
                                                << " successfully copied - total is now " 
                                                << fsrs->size() << endmsg; 
          } else {
            error() << fileRecordAddress << ": number of FSRs: " << n_lumiFSR 
                    << " did not match expected number: " << n_tsFSR << endmsg; 
          }
        }
      }
    }
  }

  // read back lumiFSR from TS
  for ( unsigned int ibx = 0 ; ibx < m_BXTypes.size() ; ++ibx ){  
    LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, m_FileRecordName + m_FSRNameVec[ibx]);
    info() << m_FileRecordName + m_FSRNameVec[ibx] << " " << lumiFSRs->size() << " FSRs" << endmsg;
    if ( msgLevel(MSG::DEBUG) ) {
      LHCb::LumiFSRs::iterator fsr;
      for ( fsr = lumiFSRs->begin(); fsr != lumiFSRs->end(); ++fsr ) {
        debug() << *(*fsr) << endmsg; 
      }
    }
  }
  // read back timeSpanFSR from TS
  LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, m_FileRecordName + m_TimeSpanFSRName);
  info() << m_FileRecordName + m_TimeSpanFSRName << " " << timeSpanFSRs->size() << " FSRs" << endmsg;
  if ( msgLevel(MSG::DEBUG) ) {
    LHCb::TimeSpanFSRs::iterator tsfsr;    
    for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); ++tsfsr ) {
      debug() << *(*tsfsr) << endmsg; 
    }
  }

  // clean up original FSRs
  for(std::vector< std::string >::iterator a = addresses.begin() ; a!= addresses.end() ; ++a ){  
    // get FSR as keyed object and cleanup the original ones - this only cleans lumiFSRs
    std::string fileRecordAddress = (*a);   
    LHCb::LumiFSRs* lumiFSRs = getIfExists<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
    if ( NULL == lumiFSRs ) {
      if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
    } else {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
      lumiFSRs->erase(lumiFSRs->begin(), lumiFSRs->end());  // release storage
      m_fileRecordSvc->unlinkObject( *a ).ignore();         // get it out of TS
    }
  }

  // clean up original tsFSRs
  for(std::vector< std::string >::iterator a = tsAddresses.begin() ; a!= tsAddresses.end() ; ++a ){  
    // get FSR as keyed object and cleanup the original ones - this only cleans tsFSRs
    std::string fileRecordAddress = (*a);   
    LHCb::TimeSpanFSRs* tsFSRs = getIfExists<LHCb::TimeSpanFSRs>(m_fileRecordSvc, fileRecordAddress);
    if ( NULL == tsFSRs ) {
      if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
    } else {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
      tsFSRs->erase(tsFSRs->begin(), tsFSRs->end());  // release storage
      m_fileRecordSvc->unlinkObject( *a ).ignore();   // get it out of TS
    }
  }

  // make a new inventory of the FileRecord store
  addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  for(std::vector< std::string >::iterator iAddr = addresses.begin() ; iAddr != addresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::DEBUG) )debug() << "address: " << (*iAddr) << endmsg;
  }
  // get timespans 
  tsAddresses = m_navigatorTool->navigate(fileRecordRoot, m_TimeSpanFSRName);
  for(std::vector< std::string >::iterator iAddr = tsAddresses.begin() ; iAddr != tsAddresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::DEBUG) )debug() << "address: " << (*iAddr) << endmsg;
  }

  // check if the original EventCount FSRs can be retrieved from the TS
  if ( msgLevel(MSG::DEBUG) ) 
  {
    LHCb::EventCountFSR* readFSR = get<LHCb::EventCountFSR>(m_fileRecordSvc, LHCb::EventCountFSRLocation::Default);
    debug() << "READ FSR: " << *readFSR << endmsg; 
  }

  // clean up eventCountFSRs, except the top level
  std::vector< std::string > evAddresses = m_navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator a = evAddresses.begin() ; a!= evAddresses.end() ; ++a ){  
    // get FSR as keyed object and cleanup the original ones - this only cleans evFSRs
    std::string fileRecordAddress = (*a);   
    if ( msgLevel(MSG::DEBUG) )debug() << "address in list: " << (*a) << endmsg;
    if ( !exist<LHCb::EventCountFSR>(m_fileRecordSvc, fileRecordAddress) ) {
      if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
    } else {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
      // the toplevel EventCountFSR should survive
      if ( fileRecordAddress.find(LHCb::EventCountFSRLocation::Default) == std::string::npos ) {
        m_fileRecordSvc->unlinkObject( *a ).ignore();   // get it out of TS
      }
    }
  }
  evAddresses = m_navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; iAddr != evAddresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::DEBUG) )debug() << "address: " << (*iAddr) << endmsg;
  }

  return StatusCode::SUCCESS;
}
