// $Id: LumiIntegrateFSR.cpp,v 1.8 2010-05-13 11:57:05 rlambert Exp $
// Include files 
 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/StatEntity.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"
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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_incSvc(0)
{
  // need to get the registry
  declareProperty( "RawEventLocation"   , m_rawEventLocation = LHCb::RawEventLocation::Default );
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"  );
  declareProperty( "FSRName"            , m_FSRName           = "/LumiFSR"     );
  declareProperty( "EventCountFSRName"  , m_EventCountFSRName = "/EventCountFSR");
  declareProperty( "TimeSpanFSRName"    , m_TimeSpanFSRName   = "/TimeSpanFSR");
  declareProperty( "PrimaryBXType"      , m_PrimaryBXType     = "BeamCrossing" );
  declareProperty( "AddBXTypes"         , m_addBXTypes ) ;
  declareProperty( "SubtractBXTypes"    , m_subtractBXTypes ) ;
  declareProperty( "IntegratorToolName" , m_ToolName          = "LumiIntegrator" );
  
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

  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  // incident service
  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" , true );
  
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  m_incSvc->addListener( this, IncidentType::BeginInputFile);
  if ( msgLevel(MSG::DEBUG) ) debug() << "registered with incSvc" << endmsg;
  //if not then the counting is not reliable
#else
  warn() << "cannot register with incSvc" << endmsg;
#endif //GAUDI_FILE_INCIDENTS

  // counting 
  m_current_fname = "";
  m_count_events = 0;
  m_events_in_file = 0;

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

  // use tool to count events for this file
  m_integratorTool->events( );

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

  // integrate all FSRs in one go
  info() << "integrating normalization: " << endmsg;
  add_file();

  // use tool to get summary for this file
  info() << "integrated normalization: " << m_integratorTool->integral( ) << endmsg;

  // declare statEntities for xml output
  add_to_xml();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

// ==========================================================================
// IIncindentListener interface
// ==========================================================================
void LumiIntegrateFSR::handle( const Incident& incident )
{
  //check extended file incidents are defined
#ifdef GAUDI_FILE_INCIDENTS
  if(incident.type()==IncidentType::BeginInputFile)
  {
    m_current_fname = incident.source();
    if ( msgLevel(MSG::DEBUG) ) debug() << "==>from handle " << m_current_fname << endmsg;
    m_count_files++;
    m_events_in_file = 0;

  }
#endif

}


//=============================================================================
void LumiIntegrateFSR::add_to_xml() {

  // declare statEntities for xml output
  LHCb::LumiIntegral::ValuePair defValue ( -1, 0 );
  for ( int key = 0; key < LHCb::LumiCounters::LastGlobal; key++ ) {
    if ( m_integratorTool->integral().hasInfo(key) ) {
      LHCb::LumiIntegral::ValuePair value = m_integratorTool->integral().info( key, defValue );
      std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
      if ( value.first != -1 ) {
	info() << "counter " << counterName << ": " << key << " " << value.first << " " << value.second << endmsg;
	StatEntity statEntity( value.first, value.second, value.second, 0, 0 );
	if ( m_counterSummarySvc != NULL ) {
	  m_counterSummarySvc->addCounter(name(), counterName, statEntity, 
					  Gaudi::CounterSummary::SaveAlwaysStatEntity);
	  debug() << "declared counter: " << key << endmsg;
	}
      }
    }
  }
}

//=============================================================================
void LumiIntegrateFSR::add_file() {
  // add the FSRs of all input files at the same time

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName; 
  std::vector< std::string > addresses = navigate(fileRecordRoot, m_FSRName);
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
      // initialize with the primary BX
      LHCb::LumiIntegral* result = new LHCb::LumiIntegral();
      add_fsr(result, primaryFileRecordAddress, 0, timeSpanRecordAddress);
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
        if ( factor != 0) add_fsr(result, fileRecordAddress, factor, timeSpanRecordAddress);
      }
      

      // use tool to integrate the background subtracted result for the whole job
      if ( m_integratorTool->integrate( (*result) ) == StatusCode::FAILURE ) {
      	Warning("fail to integrate fsr using tool").ignore(); 
      }
      info() << "Result for this file: " << *result << endmsg;
      delete result;
    }
  }
  
  //touch all EventCountFSRs
  std::vector< std::string > evAddresses = navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; 
  	  iAddr != evAddresses.end() ; ++iAddr ){
  	if ( msgLevel(MSG::VERBOSE) ) verbose() << "address: " << (*iAddr) << endmsg;
  }  
  //in the future I'll need to calculate/check something here...

  
  //touch all TimeSpanFSRs (independently of the LumiFSRs)
  std::vector< std::string > tsAddresses = navigate(fileRecordRoot, m_TimeSpanFSRName);
  for(std::vector< std::string >::iterator iAddr = tsAddresses.begin() ; 
  	  iAddr != tsAddresses.end() ; ++iAddr ){
  	if ( msgLevel(MSG::VERBOSE) ) verbose() << "ts address: " << (*iAddr) << endmsg;
  }  
}


//=============================================================================
void LumiIntegrateFSR::add_fsr(LHCb::LumiIntegral* result, 
			       std::string fileRecordAddress, 
			       float factor,
			       std::string timeSpanRecordAddress
			       ) {
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

  // read TimeSpanFSR to prepare DB access 
  if ( !exist<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress) ) {
    Warning("A timeSpan FSR was not found").ignore();
    if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " not found" << endmsg ;
  } else {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
    LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
    // look at all TimeSpanFSRs (normally only one)
    LHCb::TimeSpanFSRs::iterator tsfsr;
    for ( tsfsr = timeSpanFSRs->begin(); tsfsr != timeSpanFSRs->end(); tsfsr++ ) {
      if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << "READ TimeSpanFSR: " << *(*tsfsr) << endmsg;
      ulonglong t0 = (*tsfsr)->earliest();
      ulonglong t1 = (*tsfsr)->latest();
      if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << "interval: " << t0 << "-" << t1 << endmsg;
    }
    // the TimeSpanFSRs have now been read - something sensible should be done to access DB now and apply calibration
  }

  // read LumiFSR 
  if ( !exist<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress) ) {
    Warning("A fileRecord FSR was not found").ignore();
    if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << " not found" << endmsg ;
  } else {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << fileRecordAddress << " found" << endmsg ;
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
  if ( root ) {
    explore(root->registry(), tag, addresses);
  }
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
