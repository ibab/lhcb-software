// Include files 
 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// event model
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"

// local
#include "LumiMergeFSR.h"
#include "FSRNavigator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiMergeFSR
// 
// 2010-10-05 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiMergeFSR );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiMergeFSR::LumiMergeFSR( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"  );
  declareProperty( "FSRName"            , m_FSRName           = "/LumiFSR"     );
  declareProperty( "TimeSpanFSRName"    , m_TimeSpanFSRName   = LHCb::TimeSpanFSRLocation::Default );
  declareProperty( "PrimaryBXType"      , m_PrimaryBXType     = "BeamCrossing" );
  declareProperty( "SubtractBXTypes"    , m_subtractBXTypes ) ;
  declareProperty( "NavigatorToolName"  , m_ToolName          = "FSRNavigator" );
  
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

  // prepare TDS for FSR
  for ( unsigned int ibx = 0 ; ibx < m_BXTypes.size() ; ++ibx ){  
    std::string bx = m_BXTypes[ibx];
    info() << "BXType " << bx << endmsg;
    LHCb::LumiFSRs* fsrs = new LHCb::LumiFSRs(); // keyed container for FSRs
    m_lumiFSRsVec.push_back(fsrs);               // vector of keyed containers
    std::string name = "LumiFSR" + bx;           // 
    m_FSRNameVec.push_back(name);                // vector of names
    put(m_fileRecordSvc, fsrs, name);            // TS address of keyed container
  }
  // prepare TDS for TimeSpanFSR
  m_timeSpanFSRs = new LHCb::TimeSpanFSRs();
  put(m_fileRecordSvc, m_timeSpanFSRs, m_TimeSpanFSRName);

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
  merge();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  merge the FSR data
//=============================================================================
StatusCode LumiMergeFSR::merge() {
  // merge the FSRs of all input files at the same time

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName; 
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  // print
  for(std::vector< std::string >::iterator iAddr = addresses.begin() ; 
  	  iAddr != addresses.end() ; ++iAddr ){
  	if ( msgLevel(MSG::DEBUG) ) debug() << "address: " << (*iAddr) << endmsg;
  }  

  // a file can contain multiple sets of LumiFSRs - typically after reprocessing multiple input files
  // look first for a primary BX in the list and then look for the corresponding background types
  std::string primaryFileRecordAddress("undefined");
  for(std::vector< std::string >::iterator a = addresses.begin() ; 
      a!= addresses.end() ; ++a ){  
    if ( a->find(m_FSRName + m_PrimaryBXType) != std::string::npos ) {
      // a primary BX is found
      primaryFileRecordAddress = (*a);   
      // search for the TimeSpanFSR 
      std::string timeSpanRecordAddress(primaryFileRecordAddress);
      timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), "" );
      timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_FSRName), m_FSRName.size(), m_TimeSpanFSRName );
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed time span address" << timeSpanRecordAddress << endmsg; 

      // read TimeSpanFSR 
      unsigned long n_tsFSR = 0;
      if ( !exist<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress) ) {
        if ( msgLevel(MSG::ERROR) ) error() << timeSpanRecordAddress << " not found" << endmsg ;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
        LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
        // look at all TimeSpanFSRs (normally only one)
        LHCb::TimeSpanFSRs::iterator tsfsr;
        for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
          if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " FOUND TimeSpanFSR: " << *(*tsfsr) << endmsg;
	  // put a copy in TS container
	  n_tsFSR++;
	  LHCb::TimeSpanFSR* timeSpanFSR = new LHCb::TimeSpanFSR();
	  *timeSpanFSR = **tsfsr;
	  m_timeSpanFSRs->insert(timeSpanFSR);
        }
	info() << timeSpanRecordAddress << "number of FSRs: " << n_tsFSR << " successfully copied" << endmsg; 
      }    

      // now handle all Lumi FSRs
      std::string fileRecordAddress("undefined");
      // get all FSR objects
      for ( unsigned int ibx = 0; ibx < m_BXTypes.size() ; ++ibx ){  
	std::string bx = m_BXTypes[ibx];
	debug() << "BXType " << bx << endmsg;
	LHCb::LumiFSRs* fsrs = m_lumiFSRsVec[ibx];         // vector of keyed containers
	unsigned long n_lumiFSR = 0;
        // construct the right name of the containers
	std::string fileRecordAddress(primaryFileRecordAddress);
	fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), bx );
      	if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address: " << fileRecordAddress << endmsg; 
  	// read LumiFSR 
  	if ( !exist<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress) ) {
  	  if ( msgLevel(MSG::ERROR) ) error() << fileRecordAddress << " not found" << endmsg ;
  	} else {
  	  LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
  	  if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
  	  debug() << fileRecordAddress << " found" << endmsg ;
  	  // look at all FSRs for the BXType (normally only one)
  	  LHCb::LumiFSRs::iterator fsr;
  	  for ( fsr = lumiFSRs->begin(); fsr != lumiFSRs->end(); fsr++ ) {
  	    // print the FSR just retrieved from TS
	    if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << "READ FSR: " << *(*fsr) << endmsg; 
	    // create a new FSR and append to TS
	    n_lumiFSR++;
	    LHCb::LumiFSR* lumiFSR = new LHCb::LumiFSR();
	    *lumiFSR = **fsr;
	    fsrs->insert(lumiFSR);
  	  }
	  if ( n_tsFSR == n_lumiFSR ) {
	    info() << fileRecordAddress << ": number of FSRs: " << n_lumiFSR << " successfully copied" << endmsg; 
	  } else {
	    fatal() << fileRecordAddress << ": number of FSRs: " << n_lumiFSR 
		    << " did not match expected number: " << n_tsFSR << endmsg; 
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}
