// Include files 
// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
// event model
#include "Event/EventCountFSR.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"
#include "FSRAlgs/IFSRNavigator.h"
// local
#include "GetIntegratedLuminosity.h"
#include "LumiIntegrator.h"
// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"
//-----------------------------------------------------------------------------
// Implementation file for class : GetIntegratedLuminosity
//
// Yasmine Amhis 
// 
// 2010-05-26
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetIntegratedLuminosity )
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetIntegratedLuminosity::GetIntegratedLuminosity( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_fileRecordSvc(NULL),
    m_defaultStatusStr("UNCHECKED"),
    m_defaultStatus(LHCb::EventCountFSR::UNCHECKED),
    m_events_in_file(0),
    m_count_input(0),
    m_count_output(0),
    m_integratorTool(NULL),
    m_rawIntegratorTool(NULL)
{
  declareProperty( "FSRName"               , m_FSRName           = "/LumiFSR"     );
  declareProperty( "FileRecordLocation"    , m_FileRecordName    = "/FileRecords"  );
  declareProperty( "WriteCountersDetails"  , m_countersDetails   = false );
  declareProperty( "IntegratorToolName"    , m_ToolName          = "LumiIntegrator" );  
  declareProperty( "RawIntegratorToolName" , m_RawToolName       = "RawLumiIntegrator" );  
  declareProperty( "EventCountFSRName"     , m_EventCountFSRName = "/EventCountFSR");
}
//=============================================================================
// Destructor
//=============================================================================
GetIntegratedLuminosity::~GetIntegratedLuminosity() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetIntegratedLuminosity::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  // prepare tools
  m_integratorTool = tool<ILumiIntegrator>( "LumiIntegrator", m_ToolName );
  m_rawIntegratorTool = tool<ILumiIntegrator>( "LumiIntegrator", m_RawToolName );
  // get the File Records service
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetIntegratedLuminosity::execute() {
  // if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetIntegratedLuminosity::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  std::string fromlumi = "FromLumi_";
  Tuple tuple = nTuple("LumiTuple");
  
  double  IntegratedLuminosity    = -1000000000;
  double  IntegratedLuminosityErr = -1000000000;
  
  if(check() == 1){
    IntegratedLuminosity =  m_integratorTool->lumiValue() ; 
    IntegratedLuminosityErr =  m_integratorTool->lumiError() ; 
    info() << "The FSR were checked : " << check()<< endmsg; 
    info() << "number of events seen: " << m_integratorTool->events( ) << endmsg;
    info() << "Integrated Luminosity  : "  << IntegratedLuminosity  
           <<   " +/-  " << IntegratedLuminosityErr <<  " [pb-1]" << endmsg; 
      
    if ( m_integratorTool->duplicates().size() > 0 ) {
      warning() << "Duplicate Files: " << m_integratorTool->duplicates() << endmsg;
    }
    //store the integrated lumi
    tuple->column("IntegratedLuminosity", IntegratedLuminosity);
    tuple->column("IntegratedLuminosityErr", IntegratedLuminosityErr);
    //loop over the counters
    LHCb::LumiIntegral::ValuePair defValue ( -1, 0 );
    for ( int key = 0; key < LHCb::LumiCounters::LastGlobal; key++ ) {
      if ( m_integratorTool->integral().hasInfo(key) ) {
        LHCb::LumiIntegral::ValuePair value = m_integratorTool->integral().info( key, defValue );
        std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
        if ( value.first != -1 ) {
          if (msgLevel(MSG::DEBUG)) debug() << "From Lumi: The counter " << counterName 
                                            << ":  with the Key " 
                                            << key << "  was used : " 
                                            << value.first 
                                            << " times and the summed value is :  " << value.second << endmsg;
          //write the counters in the LumiTuple
          if(m_countersDetails == true){
            tuple->column(fromlumi+counterName+"_Key" ,key);
            tuple->column(fromlumi+counterName+ "_Counter", value.first);
            tuple->column(fromlumi+counterName+ "_Value", value.second);
          }//debug mode 
        }//check the first value
      }//take the key
      else {
        if (msgLevel(MSG::DEBUG)) debug() << "No counters available" <<  endmsg; 
      }
    }//loop over the keys

    // mu per raw file
    std::string frommu = "FromMu_";
    std::vector<ILumiIntegrator::muTuple> mTuple = m_integratorTool->muValues( );
    if ( mTuple.size() ) {
      Tuple tuple_mu = nTuple("CorrectedMuTuple");
      if (msgLevel(MSG::DEBUG)) debug() << "mu: n_files " << mTuple.size() <<  endmsg; 
      for ( std::vector<ILumiIntegrator::muTuple>::iterator iT = mTuple.begin(); iT < mTuple.end(); iT++ ) { 
	tuple_mu->column(frommu+"run", (unsigned long long)iT->run);
	tuple_mu->column(frommu+"time0", iT->time0);
	tuple_mu->column(frommu+"time1", iT->time1);
	tuple_mu->column(frommu+"dL", iT->deltaLumi);
	tuple_mu->column(frommu+"norm", iT->norm);
	tuple_mu->column(frommu+"mu", iT->mu);
	if (msgLevel(MSG::DEBUG)) debug() << "mu: per file " << iT->run << " " << iT->time0 << " " << iT->time1 << " " 
					<< iT->deltaLumi << " " << iT->norm << " " << iT->mu << " " << iT->keys.size() <<  endmsg; 
	// per raw file a set of vectors
	std::vector<int> keys(0);
	std::vector<double> vnorm(0);
	std::vector<double> value(0);
	// iterate over keys and create vectors
	std::vector<LHCb::LumiIntegral::ValuePair>::iterator iVal;
	std::vector<int>::iterator ikey;
	for ( ikey = iT->keys.begin(), iVal = iT->valuePairs.begin(); ikey < iT->keys.end(); ikey++, iVal++ ) {
	  if ( *ikey >= LHCb::LumiMethods::PoissonOffset ) {
	    keys.push_back(*ikey);
	    vnorm.push_back(iVal->first);
	    value.push_back(iVal->first ? iVal->second/iVal->first : 0);
	    if (msgLevel(MSG::DEBUG)) debug() << "mu: per file per counter " << *ikey << " " << iVal->first << " " 
					    << (iVal->first ? iVal->second/iVal->first : 0) <<  endmsg; 
	  }
	} 
	tuple_mu->farray( frommu+"counterkey", keys, frommu+"counterkey_len", keys.size() );
	tuple_mu->farray( frommu+"counternorm", vnorm, frommu+"counternorm_len", vnorm.size() );
	tuple_mu->farray( frommu+"countervalue", value, frommu+"countervalue_len", value.size() );
	// one "event" per raw file
	tuple_mu->write();
      } // loop over mu per raw files 
    }

    // raw mu per raw file
    std::string fromrawmu = "FromRawMu_";
    std::vector<ILumiIntegrator::muTuple> mRawTuple = m_rawIntegratorTool->muValues( );
    if ( mRawTuple.size() ) {
      Tuple tuple_rawmu = nTuple("RawMuTuple");
      if (msgLevel(MSG::DEBUG)) debug() << "rawmu: n_files " << mRawTuple.size() <<  endmsg; 
      for ( std::vector<ILumiIntegrator::muTuple>::iterator iT = mRawTuple.begin(); iT < mRawTuple.end(); iT++ ) { 
	tuple_rawmu->column(fromrawmu+"run", (unsigned long long)iT->run);
	tuple_rawmu->column(fromrawmu+"time0", iT->time0);
	tuple_rawmu->column(fromrawmu+"time1", iT->time1);
	if (msgLevel(MSG::DEBUG)) debug() << "rawmu: per file " << iT->run << " " << iT->time0 << " " << iT->time1 << " " << iT->keys.size() <<  endmsg; 
	// per raw file a set of vectors
	std::vector<int> keys(0);
	std::vector<double> vnorm(0);
	std::vector<double> value(0);
	// iterate over keys and create vectors
	std::vector<LHCb::LumiIntegral::ValuePair>::iterator iVal;
	std::vector<int>::iterator ikey;
	for ( ikey = iT->keys.begin(), iVal = iT->valuePairs.begin(); ikey < iT->keys.end(); ikey++, iVal++ ) {
	  if ( *ikey >= LHCb::LumiMethods::PoissonOffset ) {
	    keys.push_back(*ikey);
	    vnorm.push_back(iVal->first);
	    value.push_back(iVal->first ? iVal->second/iVal->first : 0);
	    if (msgLevel(MSG::DEBUG)) debug() << "rawmu: per file per counter " << *ikey << " " << iVal->first << " " 
					    << (iVal->first ? iVal->second/iVal->first : 0) <<  endmsg; 
	  }
	} 
	tuple_rawmu->farray( fromrawmu+"counterkey", keys, fromrawmu+"counterkey_len", keys.size() );
	tuple_rawmu->farray( fromrawmu+"counternorm", vnorm, fromrawmu+"counternorm_len", vnorm.size() );
	tuple_rawmu->farray( fromrawmu+"countervalue", value, fromrawmu+"countervalue_len", value.size() );
	// one "event" per raw file
	tuple_rawmu->write();
      } // loop over raw mu per raw files 
      
    }



  }//check that all the events were processed
  else {
    warning() << "The FSR were not  verified : " << check()<< endmsg;
    warning() <<  "Some events were not processed  " << endmsg; 
    warning() <<  "This integrated luminosity will not be computed " << endmsg; 
    tuple->column("IntegratedLuminosity",IntegratedLuminosity);
    tuple->column("IntegratedLuminosityErr", -IntegratedLuminosityErr);
      
  }//write dummy values

  StatusCode sc = tuple->write();
  if( sc.isFailure() ) {
    warning() << "Failure writing the LumiTuple" << endmsg;
  }
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
int GetIntegratedLuminosity::check() {
 
  int checkTheFSR  = 0;

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName;
  
  // prepare integrator tool
  IFSRNavigator *navigatorTool = tool<IFSRNavigator>( "FSRNavigator" , "FSRNavigator" );

  //touch all EventCountFSRs
  std::vector< std::string > evAddresses = navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; 
      iAddr != evAddresses.end() ; ++iAddr ){
    if (msgLevel(MSG::DEBUG)) debug() << "ev address: " << (*iAddr) << endmsg;
    std::string eventCountRecordAddress = *iAddr;
    // read EventCountFSR 
    if ( !exist<LHCb::EventCountFSR>(m_fileRecordSvc, eventCountRecordAddress) ) {
      Warning("An EventCount Record was not found").ignore();
      if ( msgLevel(MSG::DEBUG) ) debug() << eventCountRecordAddress << " not found" << endmsg ;
    } else {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << eventCountRecordAddress << " found" << endmsg ;
      LHCb::EventCountFSR* eventCountFSR = get<LHCb::EventCountFSR>(m_fileRecordSvc, eventCountRecordAddress);
      // look at the EventCountFSR
      if (msgLevel(MSG::DEBUG)) {
        debug() << eventCountRecordAddress << ": EventCountFSR: " << *eventCountFSR << endmsg;
        debug() << " Event Count FSR Flag " << eventCountFSR->statusFlag()  << endmsg; 
        debug() << " Event Count FSR Input : " << eventCountFSR->input() << endmsg; 
        debug()  << "Event Count FSR output : " << eventCountFSR->output()  << endmsg; 
      }
      if ( eventCountFSR->statusFlag()  == LHCb::EventCountFSR::VERIFIED ){
        if (msgLevel(MSG::DEBUG)) debug ()      << "These files are checked : " 
                                                <<   eventCountFSR->statusFlag()  << endmsg;
        checkTheFSR = 1;
      }//check the flag
      
      else {
        if (msgLevel(MSG::DEBUG)) debug() << "These files were not checked yet : "
                                          <<   eventCountFSR->statusFlag() << endmsg; 
        checkTheFSR = -1;
      }//check the flag 
    }//the eventcount was found
  }  
  return (checkTheFSR);
}
