// Include files 
 
// from Gaudi
#include "GaudiKernel/StatEntity.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"

// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"

// local
#include "LumiIntegrateFSR.h"
#include "LumiIntegrator.h"
#include "FSRAlgs/IFSRNavigator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiIntegrateFSR
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiIntegrateFSR )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiIntegrateFSR::LumiIntegrateFSR( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_fileRecordSvc(0),
    m_integratorTool(0),
    m_rawIntegratorTool(0),
    m_databaseTool(0),
    m_acceptTool(0)
{
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_FileRecordName    = "/FileRecords"  );
  declareProperty( "FSRName"            , m_FSRName           = "/LumiFSR"     );
  declareProperty( "EventCountFSRName"  , m_EventCountFSRName = "/EventCountFSR");
  declareProperty( "TimeSpanFSRName"    , m_TimeSpanFSRName   = "/TimeSpanFSR");
  declareProperty( "PrimaryBXType"      , m_PrimaryBXType     = "BeamCrossing" );
  declareProperty( "AddBXTypes"         , m_addBXTypes ) ;
  declareProperty( "SubtractBXTypes"    , m_subtractBXTypes ) ;
  declareProperty( "IntegratorToolName" , m_ToolName          = "LumiIntegrator" );
  declareProperty( "AcceptTool"         , m_acceptToolName    = "DQAcceptTool" );  
  declareProperty( "IgnoreDQFlags"      , m_ignoreDQFlags     = false);
  declareProperty( "AccumulateMu"       , m_accumulateMu      = false);
  declareProperty( "MuKeyName"          , m_muKeyName         = "PoissonRZVelo" );
  declareProperty( "RawIntegratorToolName" , m_RawToolName    = "RawLumiIntegrator" );
  
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
  
  // prepare integrator tool
  m_integratorTool = tool<ILumiIntegrator>( "LumiIntegrator" , m_ToolName );
  // prepare raw integrator tool
  m_rawIntegratorTool = tool<ILumiIntegrator>( "LumiIntegrator" , m_RawToolName );

  // Instantiate the public data quality tool
  m_acceptTool = tool<IAccept>(m_acceptToolName);
  m_DQaccepted = false;

  // initialize calibration factors
  m_statusScale = 1.0;
  m_calibScale = 0;
  m_calibScaleError = 0;
  for ( int key = 0; key <= LHCb::LumiCounters::Random; key++ ) {
    m_calibRelative.push_back(0);
    m_calibCoefficients.push_back(0);
    m_calibRelativeLog.push_back(0);
    m_calibCoefficientsLog.push_back(0);
  }
  m_calibRevolutionFrequency = 1.;
  m_calibCollidingBunches = 1;
  m_calibRandomFrequencyBB = 1.;

  // initialize the mu calculation
  if ( m_accumulateMu ) {
    m_MuKey = LHCb::LumiCounters::counterKeyToType(m_muKeyName);
    info() << "Mu calculation: " 
           << " name: " << m_muKeyName << " KeyIntValue: " <<  m_MuKey
           << endmsg;
  }

  // warn if m_ignoreDQFlags is set
  if ( m_ignoreDQFlags ) {
    warning() << "Disable DQFlags checking" << endmsg;
  }

  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == NULL) {
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // avoid times beyond DB validity at start-up
  m_dds->setEventTime(Gaudi::Time( 1000 ));

  // prepare database tool
  m_databaseTool = tool<IGetLumiParameters>( "GetLumiParameters" , "lumiDatabaseTool" );  // public

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiIntegrateFSR::execute() {

  // if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // use tool to count events for this file
  m_integratorTool->countEvents( );
  // use raw tool to count events for this file
  m_rawIntegratorTool->countEvents( );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Stop
//=============================================================================
StatusCode LumiIntegrateFSR::stop() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Stop" << endmsg;
  info() << "========== Integrating luminosity normalization: START ==========" << endmsg;

  // use tool to count events for this file
  if (msgLevel(MSG::DEBUG)) debug() << "number of events seen: " << m_integratorTool->events( ) << endmsg;

  // integrate all FSRs in one go
  add_file();

  // use tool to get summary for this file
  if (msgLevel(MSG::DEBUG)) debug() << "integrated normalization: " << m_integratorTool->integral( ) << endmsg;

  // declare statEntities for xml output and print list of counters
  add_to_xml();

  // final results
  info() << "Integrated luminosity: " 
         << m_integratorTool->lumiValue() << " +/- "
         << m_integratorTool->lumiError() << " [pb-1]" << endmsg;

  if ( m_accumulateMu ) {
    std::vector<ILumiIntegrator::muTuple> mT = m_integratorTool->muValues( );
    info() << "Mu Tuple created with size: " << mT.size() << endmsg; 
  }

  if ( m_integratorTool->duplicates().size() > 0 ) {
    warning() << "Duplicate Files: " << m_integratorTool->duplicates() << endmsg;
  }
  info() << "========== Integrating luminosity normalization: END ==========" << endmsg;
  return GaudiAlgorithm::stop();  // must be called after all other actions
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiIntegrateFSR::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
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
        if (msgLevel(MSG::DEBUG)) debug() << "counter " << counterName << ": " 
                                          << key << " " << value.first << " " << value.second << endmsg;
        StatEntity statEntity( value.first, value.second, value.second, 0, 0 );
        if ( m_counterSummarySvc != NULL ) {
          m_counterSummarySvc->addCounter(name(), counterName, statEntity, 
                                          Gaudi::CounterSummary::SaveAlwaysStatEntity);
          if (msgLevel(MSG::DEBUG)) debug() << "declared counter: " << key << endmsg;
        }
      }
    }
  }
  // final results
  double lumi = m_integratorTool->lumiValue();
  double lumi_error = m_integratorTool->lumiError();

  StatEntity statEntityLumi( 1, lumi, lumi_error, 0, 0 );
  if ( m_counterSummarySvc != NULL ) {
    m_counterSummarySvc->addCounter(name(), "Luminosity", statEntityLumi, 
                                    Gaudi::CounterSummary::SaveAlwaysStatEntity);
  }
}

//=========================================================================
//  add the FSR data
//=============================================================================
StatusCode LumiIntegrateFSR::add_file() {
  // add the FSRs of all input files at the same time
  double rel_scale = 1.0;
  long n_runs = 0;
  
  // prepare integrator tool
  IFSRNavigator *navigatorTool = tool<IFSRNavigator>( "FSRNavigator" , "FSRNavigator" );

  // make an inventory of the FileRecord store
  std::string fileRecordRoot = m_FileRecordName; 
  std::vector< std::string > addresses = navigatorTool->navigate(fileRecordRoot, m_FSRName);
  for(std::vector< std::string >::iterator iAddr = addresses.begin() ; iAddr != addresses.end() ; ++iAddr ){
    if ( msgLevel(MSG::DEBUG) ) debug() << "address: " << (*iAddr) << endmsg;
  }  

  // a file can contain multiple sets of LumiFSRs - typically after reprocessing multiple input files
  // look first for a primary BX in the list and then look for the corresponding background types
  std::string primaryFileRecordAddress("undefined");
  for(std::vector< std::string >::iterator a = addresses.begin() ; a!= addresses.end() ; ++a ){  
    if ( a->find(m_FSRName + m_PrimaryBXType) != std::string::npos ) {
      // a primary BX is found
      primaryFileRecordAddress = (*a);   
      if ( !exist<LHCb::LumiFSRs>(m_fileRecordSvc, primaryFileRecordAddress) ) {
        Warning("Primary fileRecord FSR was not found").ignore();
        if ( msgLevel(MSG::DEBUG) ) debug() << primaryFileRecordAddress << " not found" << endmsg ;
      } else {
        LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, primaryFileRecordAddress);
        if ( msgLevel(MSG::DEBUG) ) debug() << primaryFileRecordAddress << " found" << endmsg ;
        long fkey_ts = 0;
        LHCb::LumiFSRs::iterator checkfsr = lumiFSRs->begin();
	// length of container to check against the other FSRs
	unsigned long fsr_len = lumiFSRs->size();
  	// search for the TimeSpanFSR 
  	std::string timeSpanRecordAddress(primaryFileRecordAddress);
  	timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), "" );
  	timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_FSRName), m_FSRName.size(), m_TimeSpanFSRName );
  	// read TimeSpanFSR to check length of container
	unsigned long tsfsr_len = 0;
  	if ( exist<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress) ) {
  	  if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
  	  LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);
  	  // check index bounds
  	  tsfsr_len = timeSpanFSRs->size();
	  if ( msgLevel(MSG::DEBUG) ) debug() << "number of timeSpanFSRs: " << tsfsr_len << " number of FSRs " << fsr_len << endmsg;
	  if ( tsfsr_len < fsr_len ) {
	    warning() << "number of timeSpanFSRs: " << tsfsr_len << " smaller than number of FSRs " << fsr_len << endmsg;
	  }
	}

	// run over the FSRs in this container and sum
        for ( unsigned int fkey = 0; fkey != lumiFSRs->size(); fkey++ ) {
          // trigger database update using the timeSpan FSR
          // if no run number defined, there is no timespan FSR, so skip it
          LHCb::TimeSpanFSR* timeSpanFSR = NULL;
          if ( checkfsr[fkey]->runNumbers().size() ) {
            timeSpanFSR = trigger_event(primaryFileRecordAddress, fkey_ts);
	    fkey_ts++;
          } else {
            warning() << "missing run number at keycount: " << fkey << " / " << fkey_ts 
                      << " skip db update for this FSR (" << primaryFileRecordAddress << ")" << endmsg;
	    if ( tsfsr_len >= fsr_len ) fkey_ts++;
          }

          // integrate only if DQ-accepted - flag is updated in trigger_event 
          if ( m_DQaccepted || m_ignoreDQFlags ) {
	    // initialize integral with the primary BX
	    LHCb::LumiIntegral* result = new LHCb::LumiIntegral();
	    add_fsr(result, primaryFileRecordAddress, 0, fkey);
	    // initialize non-background subtracted integral with the primary BX
	    LHCb::LumiIntegral* raw_result = new LHCb::LumiIntegral();
	    add_fsr(raw_result, primaryFileRecordAddress, 0, fkey);
	    // get the background to be subtracted/added
	    std::string fileRecordAddress("undefined");
	    for ( std::vector< std::string >::iterator bx = m_BXTypes.begin() ; bx!= m_BXTypes.end() ; ++bx ){  
	      // construct the right name of the containers
	      std::string fileRecordAddress(primaryFileRecordAddress);
	      fileRecordAddress.replace( fileRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), (*bx) );
	      if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed address" << fileRecordAddress << endmsg; 
	      float factor = 0;     // indicates the primary BX - already used
	      if ( m_addBXTypes.end() != find( m_addBXTypes.begin(), m_addBXTypes.end(), (*bx) ) ) 
		factor = 1.;
	      if ( m_calibCoefficients[LHCb::LumiMethods::CorrectionFlag] != 0) 
		factor = 0;      // no subtraction of EE
	      if ( m_subtractBXTypes.end() != find( m_subtractBXTypes.begin(), m_subtractBXTypes.end(), (*bx) ) ) {
		factor = -1.;
	      }
	      if ( factor != 0) {
                StatusCode sc =  add_fsr(result, fileRecordAddress, factor, fkey);
                if (sc.isFailure()) {
                  m_statusScale = 0;    // invalid luminosity
                  error() << "ERROR summing bunch crossing types for luminosity " << endmsg;
                }
	      }
	    }

            // apply calibration
            if (msgLevel(MSG::DEBUG)) debug() << "Result for this file (before calibration): " << *result << endmsg;
            result->scale(one_vector(m_calibRelative, m_calibRelativeLog, LHCb::LumiMethods::PoissonOffset));
            if ( msgLevel(MSG::VERBOSE) ) verbose() << "Result for this file (after calibration): " << *result << endmsg;
            // simple summing per counter
            if ( m_integratorTool->integrate( *result ) == StatusCode::FAILURE ) {
              m_statusScale = 0;        // invalid luminosity
              error() << "ERROR summing result using tool " << endmsg;
            }
            // raw counter sums
            m_rawIntegratorTool->integrate( *raw_result );
	    
            // summing of integral
            long old_n_runs = n_runs;
            if ( result->runNumbers().size() ) n_runs = result->runNumbers()[0];
            else n_runs = 0;
            double old_scale = rel_scale;
            rel_scale = m_calibRevolutionFrequency * m_calibCollidingBunches / m_calibRandomFrequencyBB;
            if ( m_integratorTool->integrate( *result, one_vector(m_calibCoefficients, 
                                                                  m_calibCoefficientsLog, LHCb::LumiMethods::PoissonOffset), 
                                              rel_scale ) == StatusCode::FAILURE ) {
              m_statusScale = 0;        // invalid luminosity
              error() << "ERROR integrating luminosity result" << endmsg;
            }
            if ( old_scale != rel_scale || old_n_runs != n_runs ) {
              info() << "run: " << result->runNumbers()
                     << " RandomFrequencyBB " << m_calibRandomFrequencyBB 
                     << " CollidingBunches " << m_calibCollidingBunches << endmsg;
            }
            // accumulate mu 
            if ( m_accumulateMu ) {
              m_integratorTool->accumulate_mu( *result, timeSpanFSR, m_MuKey, 
                                               one_vector(m_calibCoefficients, m_calibCoefficientsLog, 
                                                          LHCb::LumiMethods::PoissonOffset), rel_scale );
              m_rawIntegratorTool->accumulate_mu( *raw_result, timeSpanFSR, m_MuKey, 
						  one_vector(m_calibCoefficients, m_calibCoefficientsLog, 
							     LHCb::LumiMethods::PoissonOffset), rel_scale );
            }
            delete result;
            delete raw_result;
          }
        }
      }
      
      // set absolute scales for tool
      double abs_scale = m_statusScale * m_calibScale;
      m_integratorTool->setAbsolute(abs_scale, m_calibScaleError);
      if (msgLevel(MSG::DEBUG)) debug() << "Intermediate Integrated luminosity: " 
                                        << m_integratorTool->lumiValue() << endmsg;
    }
  }
  // set absolute scales for tool
  if ( !m_statusScale ) m_statusScale = -999999.;
  double abs_scale = m_statusScale * m_calibScale;
  m_integratorTool->setAbsolute(abs_scale, m_calibScaleError);
  info() << "Luminosity scale used: " << m_calibScale << " relative uncertainty " << m_calibScaleError << endmsg;
  
  ///////////////////  the following is not needed - but keep to see if we can use the FSR
  //touch all EventCountFSRs
  std::vector< std::string > evAddresses = navigatorTool->navigate(fileRecordRoot, m_EventCountFSRName);
  for(std::vector< std::string >::iterator iAddr = evAddresses.begin() ; 
  	  iAddr != evAddresses.end() ; ++iAddr ){
  	if ( msgLevel(MSG::VERBOSE) ) verbose() << "address: " << (*iAddr) << endmsg;
  }  
  //in the future I'll need to calculate/check something here...

  return StatusCode::SUCCESS;

}

//=============================================================================
LHCb::TimeSpanFSR* LumiIntegrateFSR::trigger_event( std::string primaryFileRecordAddress, unsigned int fkey ) {
  StatusCode sc = StatusCode::SUCCESS;

  // get run number
  LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, primaryFileRecordAddress);
  LHCb::LumiFSRs::iterator fsr = lumiFSRs->begin();
  LHCb::LumiFSR* lumiFSR = fsr[fkey];
  unsigned long run = 0;
  if ( lumiFSR->runNumbers().size() ) {
    run = lumiFSR->runNumbers()[0];
  } else {
    warning() << "missing run number at keycount: " << fkey << endmsg;
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << " run number " << run << endmsg;

  // return object
  LHCb::TimeSpanFSR* timeSpanFSR = NULL;
  // search for the TimeSpanFSR 
  std::string timeSpanRecordAddress(primaryFileRecordAddress);
  timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), "" );
  timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_FSRName), m_FSRName.size(), m_TimeSpanFSRName );
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed time span address" << timeSpanRecordAddress << endmsg; 

  // read TimeSpanFSR to prepare DB access 
  if ( !exist<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress) ) {
    if ( msgLevel(MSG::ERROR) ) error() << timeSpanRecordAddress << " timeSpanFSR not found - use previous DB conditions" << endmsg ;
    return timeSpanFSR;
  } else {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
    LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);

    // pick the TimeSpanFSR
    LHCb::TimeSpanFSRs::iterator tsfsr = timeSpanFSRs->begin();
    // check index bounds
    unsigned long tsfsr_len = timeSpanFSRs->size();
    if ( (unsigned long)fkey > tsfsr_len-1 ) { 
      warning() << "missing timeSpanFSR - use previous DB conditions" << endmsg;
      return timeSpanFSR;
    } 
    // define the time interval
    timeSpanFSR = tsfsr[fkey];
    ulonglong t0 = timeSpanFSR->earliest();
    ulonglong t1 = timeSpanFSR->latest();
    if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " READ TimeSpanFSR: " << *timeSpanFSR << " interval: " << t0 << "-" << t1 << endmsg;
    // no time found - try first to repair
    if ( t0 == 0 || t1 == 0 ) {
      if ( msgLevel(MSG::DEBUG) ) debug() << "no time span defined - try harder by using a nearby interval" << endmsg;
      for ( unsigned long new_fkey = 0; new_fkey < tsfsr_len; ++new_fkey ) {
	timeSpanFSR = tsfsr[new_fkey];
	t0 = timeSpanFSR->earliest();
	t1 = timeSpanFSR->latest();
	if ( msgLevel(MSG::DEBUG) ) debug() << timeSpanRecordAddress << " READ TimeSpanFSR: " << *timeSpanFSR << " interval: " << t0 << "-" << t1 << endmsg;
	if ( t0 != 0 && t1 != 0 ) {
	  LHCb::LumiFSR* new_lumiFSR = fsr[new_fkey];
	  unsigned long new_run = 0;
	  if ( new_lumiFSR->runNumbers().size() ) new_run = new_lumiFSR->runNumbers()[0];
	  warning() << timeSpanRecordAddress << " replacement interval: " << t0 << "-" << t1 << " run number: " << new_run << endmsg;
	  break;
	}
      }
    }
    // check if OK now
    if ( t0 == 0 || t1 == 0 ) {
      m_statusScale = 0;        // invalid luminosity: no time span
      error() << "ERROR: no time span defined at all (usually due to empty input file) - this invalidates the luminosity calculation!" << endmsg;
      return timeSpanFSR;
    }
    // the TimeSpanFSRs have now been read -  fake event loop to get update of calibration constants
    m_dds->setEventTime(Gaudi::Time( (t1/2+t0/2)*1000 ));
    if (msgLevel(MSG::DEBUG)) debug() << " creating new event " << endmsg;
    sc = updMgrSvc()->newEvent();
    if (sc.isFailure()) {
      m_statusScale = 0;        // invalid luminosity
      error() << "ERROR updating luminosity constants from DB " << endmsg;
    }
  }    

  // look at DQ flag
  m_DQaccepted = m_acceptTool->accept();
  if (msgLevel(MSG::DEBUG) ) {
    debug() << "Filter DQ flags: " << ((m_DQaccepted) ? "good" : "bad") << " period" << endmsg;
  }
  
  // look at the new DB parameters
  if ( msgLevel(MSG::DEBUG) ) {
    unsigned int toolTCK =  m_databaseTool->getTCK();
    double toolOdinFraction = m_databaseTool->OdinFraction();
    double toolRandomRate = m_databaseTool->HLTRandomRate();
    debug() << "TCK:            " << toolTCK << endmsg;
    debug() << "OdinFraction:   " << toolOdinFraction << endmsg;
    debug() << "HLT Randomrate: " << toolRandomRate << endmsg;
  }

  // get the database parameters
  m_calibCollidingBunches = m_databaseTool->CollidingBunches();
  m_calibRevolutionFrequency = m_databaseTool->LHCFrequency();
  m_calibRandomFrequencyBB = m_databaseTool->RandomRateBB();
  
  m_calibRelative = m_databaseTool->CalibRelative();         
  m_calibCoefficients = m_databaseTool->CalibCoefficients();     
  m_calibRelativeLog = m_databaseTool->CalibRelativeLog();      
  m_calibCoefficientsLog = m_databaseTool->CalibCoefficientsLog();  
  m_calibScale = m_databaseTool->CalibScale();
  m_calibScaleError = m_databaseTool->CalibScaleError();

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "bunches:        " << m_calibCollidingBunches << endmsg;
    debug() << "LHCfrequency:   " << m_calibRevolutionFrequency << endmsg;
    debug() << "BB RandomRate:  " << m_calibRandomFrequencyBB << endmsg;
    debug() << "absolutescale:  " << m_calibScale << endmsg;
    debug() << "scale error:    " << m_calibScaleError << endmsg;
  }

  // flag correct data
  m_statusScale *= m_databaseTool->StatusScale();

  return timeSpanFSR;
}

//=========================================================================
//  create one calibration vector
//=========================================================================
std::vector<double> LumiIntegrateFSR::one_vector(std::vector<double> a,
                                                 std::vector<double> b, int offset) {
  std::vector<double> c;
  for ( int key = 0; key <= offset + LHCb::LumiCounters::Random; key++ ) {
    double value = 0;
    if ( key <= LHCb::LumiCounters::Random ) value = a[key];
    if ( key >= offset ) value = b[key-offset];
    c.push_back(value);
  }
  return c;
}

//=============================================================================
StatusCode LumiIntegrateFSR::add_fsr(LHCb::LumiIntegral* result, 
                                     std::string fileRecordAddress, 
                                     float factor, unsigned int fkey ) {

  // read LumiFSR 
  if ( !exist<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress) ) {
    Warning("A fileRecord FSR was not found").ignore();
    if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << " not found" << endmsg ;
  } else {
    LHCb::LumiFSRs* lumiFSRs = get<LHCb::LumiFSRs>(m_fileRecordSvc, fileRecordAddress);
    if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << " found" << endmsg ;

    // prepare an empty summary for this BXType
    LHCb::LumiFSR bxFSR;
    // pick the right fsr at fkey
    LHCb::LumiFSRs::iterator fsr = lumiFSRs->begin();

    // protection for missing data
    if ( lumiFSRs->empty() ) warning() << "FSR is empty!!!:" << fileRecordAddress << endmsg;
    if ( lumiFSRs->size() <= fkey ) {
      warning() << "FSR is missing elements: [" << fkey << "] missing in " << fileRecordAddress << endmsg;
      return StatusCode::SUCCESS;
    }

    LHCb::LumiFSR* lumiFSR = fsr[fkey];
    bxFSR = (*lumiFSR);
    // debug output
    if ( msgLevel(MSG::VERBOSE) ) {
      // print also the contents using the builtin lookup tables
      LHCb::LumiFSR::ExtraInfo::iterator infoIter;
      LHCb::LumiFSR::ExtraInfo  fsrInfo = lumiFSR->extraInfo();
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
      if ( msgLevel(MSG::DEBUG) ) debug() << fileRecordAddress << " READ FSR: " << *lumiFSR << endmsg; 
    }

    // add or subtract the normalized result
    if ( factor == 0 ) {    
      // primary crossing;
      result->mergeRuns(bxFSR.runNumbers());
      result->mergeFileIDs(bxFSR.fileIDs());
      result->addNormalized(bxFSR, factor, LHCb::LumiMethods::PoissonOffset);
      if ( msgLevel(MSG::DEBUG) ) debug() << "addPrimary   : " << *result << endmsg;
    } else {
      // correction
      result->addNormalized(bxFSR, factor, LHCb::LumiMethods::PoissonOffset);
      if ( msgLevel(MSG::DEBUG) ) debug() << "addNormalized: " << *result << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

