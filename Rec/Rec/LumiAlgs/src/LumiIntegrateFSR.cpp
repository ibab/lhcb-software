// $Id: LumiIntegrateFSR.cpp,v 1.2 2009-07-10 15:53:40 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"

// local
#include "LumiIntegrateFSR.h"
#include "LumiIntegrator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiIntegrateFSR
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiIntegrateFSR );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiIntegrateFSR::LumiIntegrateFSR( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // need to get the registry
  declareProperty( "RawEventLocation"   , m_rawEventLocation = LHCb::RawEventLocation::Default );
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName  = "/FileRecords"  );
  declareProperty( "FSRName"            , m_FSRName        = "/LumiFSR"     );
  declareProperty( "PrimaryBXType"      , m_PrimaryBXType  = "BeamCrossing" );
  declareProperty( "AddBXTypes"         , m_addBXTypes ) ;
  declareProperty( "SubtractBXTypes"    , m_subtractBXTypes ) ;
  declareProperty( "IntegratorToolName" , m_ToolName       = "LumiIntegrator" );
  
}
//=============================================================================
// Destructor
//=============================================================================
LumiIntegrateFSR::~LumiIntegrateFSR() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiIntegrateFSR::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // initialize lists - the order is important: the first is the one to normalize to
  if (m_addBXTypes.empty()) {
      m_addBXTypes.push_back("NoBeam");
  }
  if (m_subtractBXTypes.empty()) {
      m_subtractBXTypes.push_back("Beam1");
      m_subtractBXTypes.push_back("Beam2");
  }
  // ensure consistency
  m_BXTypes.push_back(m_PrimaryBXType);
  info() << "Primary  BXType " << m_PrimaryBXType << endmsg;
  for(std::vector< std::string >::iterator bx = m_addBXTypes.begin() ; 
          bx!= m_addBXTypes.end() ; ++bx ){  
    info() << "Add      BXType " << (*bx) << endmsg;
    if ( (*bx) != "None" ) m_BXTypes.push_back(*bx);
  }
  for(std::vector< std::string >::iterator bx = m_subtractBXTypes.begin() ; 
          bx!= m_subtractBXTypes.end() ; ++bx ){  
    info() << "Subtract BXType " << (*bx) << endmsg;
    if ( (*bx) != "None" ) m_BXTypes.push_back(*bx);
  }

  // get FileRecordDataSvc Stream
  sc = service("FileRecordDataSvc", m_fileRecordSvc, true);
  if( sc.isFailure() ) {
    if ( msgLevel(MSG::ERROR) ) error() << "Unable to retrieve run records service" << endreq;
    return sc;
  }

  // counting 
  m_current_fname = "";
  m_count_files = 0;
  m_count_events = 0;

  // prepare integrator tool
  m_integratorTool = tool<ILumiIntegrator>( "LumiIntegrator" , m_ToolName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiIntegrateFSR::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_count_events++;
  // use tool to count events for this file
  m_integratorTool->countEvents( );

  // check if the file ID is new
  std::string fname = fileID();
  // use tool to count events for this file
  m_integratorTool->events( );

  // check if this is a new file
  if ( fname != m_current_fname ) {
    m_count_files++;
    m_current_fname = fname;
    // and add contents to the integral
    add_file();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiIntegrateFSR::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // look at LumiAnalyser for examples
  info() << "number of files seen: " << m_count_files << endmsg;

  // use tool to count events for this file
  info() << "number of events seen: " << m_integratorTool->events( ) << endmsg;

  // use tool to get summary for this file
  info() << "integrated normalization: " << m_integratorTool->integral( ) << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
void LumiIntegrateFSR::add_file() {
  // add the FSRs of one input file

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName + "/" + m_current_fname;
  std::vector< std::string > addresses = navigate(fileRecordRoot, m_FSRName);
  for(std::vector< std::string >::iterator iAddr = addresses.begin() ; 
  	  iAddr != addresses.end() ; ++iAddr ){
  	if ( msgLevel(MSG::VERBOSE) ) verbose() << "address: " << (*iAddr) << endmsg;
  }  

  // a file can contain multiple sets of LumiFSRs - typically after reprocessing multiple input files
  // look first for a primary BX in the list and then look for the corresponding background types
  std::string primaryFileRecordAddress("undefined");
  for(std::vector< std::string >::iterator a = addresses.begin() ; 
      a!= addresses.end() ; ++a ){  
    if ( a->find(m_FSRName + m_PrimaryBXType) != std::string::npos ) {
      // a primary BX is found
      primaryFileRecordAddress = (*a);   
      // initialize with the primary BX
      LHCb::LumiIntegral* result = new LHCb::LumiIntegral();
      add_fsr(result, primaryFileRecordAddress, 0);
      // get the background to be subtracted/added
      std::string fileRecordAddress("undefined");
      // get all FSR objects - this is anyway needed to instantiate them on the TDS
      for(std::vector< std::string >::iterator bx = m_BXTypes.begin() ; bx!= m_BXTypes.end() ; ++bx ){  
        // construct the right name of the containers
	std::string fileRecordAddress(primaryFileRecordAddress);
	fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), (*bx) );
      	if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address" << fileRecordAddress << endmsg; 
        float factor = 0;  // indicates the primary BX - already used
        if ( m_addBXTypes.end() != find( m_addBXTypes.begin(), m_addBXTypes.end(), (*bx) ) ) 
          factor = 1.;
        if ( m_subtractBXTypes.end() != find( m_subtractBXTypes.begin(), m_subtractBXTypes.end(), (*bx) ) ) 
          factor = -1.;
        if ( factor != 0) add_fsr(result, fileRecordAddress, factor);
      }
      // use tool to integrate the background subtracted result for the whole job
      if ( m_integratorTool->integrate( (*result) ) == StatusCode::FAILURE ) {
      	warning() << "fail to integrate fsr using tool" << endmsg; 
      }
      info() << "Result for this file: " << *result << endmsg;
      delete result;
    }
  }
}

//=============================================================================
std::string LumiIntegrateFSR::fileID() {
  // get the fileID from the event store
  std::string event_fname("");

  // get ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error and stop the run
    error() << "ODIN cannot be loaded" << endmsg;
    return event_fname;
  }
  // obtain the run number from ODIN
  unsigned int run = odin->runNumber();  
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "ODIN RunNumber: " << run << endmsg;

  // registry from raw data - only correct if file catalogue used 
  IOpaqueAddress* eAddr = 0;
  if( !exist<LHCb::RawEvent>(m_rawEventLocation) ){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << m_rawEventLocation << " not found" << endmsg ;
    // then try from other bank
    if ( !exist<DataObject>("/Event") ){
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "/Event" << " not found" << endmsg ;
      eAddr = odin->registry()->address();
    } else {
      // get the container
      DataObject* event = get<DataObject>("/Event");
      eAddr = event->registry()->address();
      }
  } else {
    LHCb::RawEvent* event = get<LHCb::RawEvent>(m_rawEventLocation);
    eAddr = event->registry()->address();
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

//=============================================================================
void LumiIntegrateFSR::add_fsr(LHCb::LumiIntegral* result, std::string fileRecordAddress, float factor) {
  // add or subtract this crossing type
  if ( msgLevel(MSG::DEBUG) ) debug() << "using the container: " << fileRecordAddress << endmsg;
  if ( msgLevel(MSG::DEBUG) ) {
    // check if FSR can be found as DataObject (debugging only)
    DataObject* pObj=0;
    StatusCode sc = m_fileRecordSvc->retrieveObject(fileRecordAddress, pObj);
    if ( !sc.isSuccess() ) {
      debug() << "retrieveObject did not get: " << fileRecordAddress << endmsg;
    } else {
      verbose() << "retrieveObject: " << fileRecordAddress << endmsg;
    }
  }

  // read FSR 
  if ( !exist<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress) ) {
    if ( msgLevel(MSG::WARNING) ) warning() << fileRecordAddress << " not found" << endmsg ;
  } else {
    if ( msgLevel(MSG::DEBUG) ) verbose() << fileRecordAddress << " found" << endmsg ;
    LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);

    // prepare an empty summary for this BXType
    LHCb::LumiFSR bxFSR;
    // look at all FSRs for the BXType (normally only one)
    LHCb::LumiFSRs::iterator fsr;
    for ( fsr = lumiFSRs->begin(); fsr != lumiFSRs->end(); fsr++ ) {
      // print the FSR just retrieved from TS
      if ( msgLevel(MSG::VERBOSE) ) {
        // print also the contents using the builtin lookup tables
        LHCb::LumiFSR::ExtraInfo::iterator infoIter;
        LHCb::LumiFSR::ExtraInfo  fsrInfo = (*fsr)->extraInfo();
        for (infoIter = fsrInfo.begin(); infoIter != fsrInfo.end(); infoIter++) {
  	// get the key and value of the input info
  	int key = infoIter->first;
  	LHCb::LumiFSR::ValuePair values = infoIter->second;
  	int incr = values.first;
  	longlong count = values.second;
  	const std::string keyName = LHCb::LumiCounters::counterKeyToString(key);
  	int keyInt = LHCb::LumiCounters::counterKeyToType(keyName);
  	verbose() << "READ key: " << key 
  		  << " name: " << keyName << " KeyInt: " << keyInt 
  		  << " increment: " << incr << " integral: " << count << endmsg;
        }
      } else {
        if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << "READ FSR: " << *(*fsr) << endmsg; 
      }
      // sum all FSRs per input file (should normally be one only)
      bxFSR += (*fsr);
    }
    // add or subtract the normalized result
    if ( factor == 0 ) {    
      // primary crossing;
      (*result) += bxFSR;
      if ( msgLevel(MSG::DEBUG) ) debug() << "addPrimary   : " << *result << endmsg;
    } else {
      // correction
      result->addNormalized(bxFSR, factor);
      if ( msgLevel(MSG::DEBUG) ) debug() << "addNormalized: " << *result << endmsg;
    }
  }
}


//=============================================================================
std::vector< std::string > LumiIntegrateFSR::navigate(std::string rootname, std::string tag) {
  // navigate recursively through the FileRecord store and report addresses which contain the tag
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Explore: " << rootname << " for " << tag << endmsg;
  std::vector< std::string > addresses;
  SmartDataPtr<DataObject>   root(m_fileRecordSvc, rootname);
  explore(root->registry(), tag, addresses);
  return addresses;
}

//=============================================================================
void LumiIntegrateFSR::explore(IRegistry* pObj, std::string tag, std::vector< std::string >& addresses) {
  // add the addresses which contain the tag to the list and search through the leaves
  if ( 0 != pObj )    {
    std::string name = pObj->name();
    std::string::size_type f = name.find(tag);
    std::string id = pObj->identifier();

    // add this address to the list
    if ( f != std::string::npos ) addresses.push_back(id);

    // search through the leaves
    SmartIF<IDataManagerSvc> mgr(m_fileRecordSvc);
    if ( mgr )    {
      typedef std::vector<IRegistry*> Leaves;
      Leaves leaves;
      StatusCode sc = mgr->objectLeaves(pObj, leaves);
      if ( sc.isSuccess() )  {
        for ( Leaves::const_iterator iLeaf=leaves.begin(); iLeaf != leaves.end(); iLeaf++ )   {
	  // it is important to redefine leafRoot->registry() way back from the identifier 
	  std::string leafId = (*iLeaf)->identifier();
	  SmartDataPtr<DataObject> leafRoot(m_fileRecordSvc, leafId);
	  explore(leafRoot->registry(), tag, addresses);
        }
      }
    }
  }
}
