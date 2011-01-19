// $Id: LumiIntegrateFSR.cpp,v 1.8 2010-05-13 11:57:05 rlambert Exp $
// Include files 
 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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
#include "FSRNavigator.h"

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
    m_condRelative(NULL),
    m_condAbsolute(NULL),
    m_condCoefficients(NULL),
    m_condGUIDs(NULL)
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

  // initialize calibration factors
  m_statusScale = 1.0;
  m_calibScale = 0;
  m_calibScaleError = 0;
  for ( int key = 0; key <= LHCb::LumiCounters::Random; key++ ) {
    std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
    m_calibRelative.push_back(0);
    m_calibCoefficients.push_back(0);
    m_calibRelativeLog.push_back(0);
    m_calibCoefficientsLog.push_back(0);
  }
  m_calibRevolutionFrequency = 1.;
  m_calibCollidingBunches = 1;
  m_calibRandomFrequencyBB = 1.;

  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == NULL) {
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register conditions for database acces
  StatusCode sc0 = registerDB(); // must be executed first
  if ( sc0.isFailure() ) return sc0;  // error printed already

  return runUpdate();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiIntegrateFSR::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
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
  info() << "========== Integrating luminosity normalization: START ==========" << endmsg;

  // use tool to count events for this file
  debug() << "number of events seen: " << m_integratorTool->events( ) << endmsg;

  // integrate all FSRs in one go
  add_file();

  // use tool to get summary for this file
  debug() << "integrated normalization: " << m_integratorTool->integral( ) << endmsg;

  // declare statEntities for xml output and print list of counters
  add_to_xml();

  // final results
  info() << "Integrated luminosity: " 
	 << m_integratorTool->lumiValue() << " +/- "
	 << m_integratorTool->lumiError() << " [pb-1]" << endmsg;

  if ( m_integratorTool->duplicates().size() > 0 ) {
    warning() << "Duplicate Files: " << m_integratorTool->duplicates() << endmsg;
  }
  info() << "========== Integrating luminosity normalization: END ==========" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
void LumiIntegrateFSR::add_to_xml() {

  // declare statEntities for xml output
  LHCb::LumiIntegral::ValuePair defValue ( -1, 0. );
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
  // final results
  double lumi = m_integratorTool->lumiValue();
  double lumi_error = m_integratorTool->lumiError();

  StatEntity statEntityLumi( 1, lumi, lumi_error, 0, 0 );
  if ( m_counterSummarySvc != NULL ) {
    m_counterSummarySvc->addCounter(name(), "Luminosity", statEntityLumi, 
				    Gaudi::CounterSummary::SaveAlwaysStatEntity);
  }
}


//=============================================================================
// DB access
//=============================================================================
StatusCode LumiIntegrateFSR::registerDB() {
  // register the DB conditions for the update maganer
  debug() << "==> Register DB" << endmsg;

  // register absolute calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/AbsoluteCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/AbsoluteCalibration",
                      m_condAbsolute, &LumiIntegrateFSR::i_cacheAbsoluteData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/AbsoluteCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register relative calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibration",
                      m_condRelative, &LumiIntegrateFSR::i_cacheRelativeData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register relative calibration for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibrationLog")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibrationLog",
                      m_condRelativeLog, &LumiIntegrateFSR::i_cacheRelativeDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibrationLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register usage coefficients
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Coefficients")) {
    registerCondition("Conditions/Lumi/LHCb/Coefficients",
                      m_condCoefficients, &LumiIntegrateFSR::i_cacheCoefficientData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/Coefficients not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register usage coefficients for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/CoefficientsLog")) {
    registerCondition("Conditions/Lumi/LHCb/CoefficientsLog",
                      m_condCoefficientsLog, &LumiIntegrateFSR::i_cacheCoefficientDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/CoefficientsLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register sampling frequencies of bunches
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Sampling")) {
    registerCondition("Conditions/Lumi/LHCb/Sampling",
                      m_condSampling, &LumiIntegrateFSR::i_cacheSamplingData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/Sampling not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from relativeCalibration
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheRelativeData() {
  debug() << "callback RelativeCalibration:" << endmsg;
  std::vector<double> cal = m_condRelative->paramVect<double>("RelativeFactors");
  if ( cal.size() == m_calibRelative.size() ) {
    m_calibRelative = cal;
    return StatusCode::SUCCESS;
  }
  fatal() << "inconsistent number of parameters in RelativeCalibration:" << cal.size() << endmsg;
  m_statusScale = 0;        // invalid luminosity
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Extract data from relativeCalibrationLog
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheRelativeDataLog() {
  debug() << "callback RelativeCalibrationLog:" << endmsg;
  std::vector<double> cal = m_condRelativeLog->paramVect<double>("RelativeFactorsLog");
  if ( cal.size() == m_calibRelativeLog.size() ) {
    m_calibRelativeLog = cal;
    return StatusCode::SUCCESS;
  }
  fatal() << "inconsistent number of parameters in RelativeCalibrationLog:" << cal.size() << endmsg;
  m_statusScale = 0;        // invalid luminosity
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Extract data from Coefficients
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheCoefficientData() {
  debug() << "callback Coefficients:" << endmsg;
  std::vector<double> cal = m_condCoefficients->paramVect<double>("Coefficients");
  if ( cal.size() == m_calibCoefficients.size() ) {
    m_calibCoefficients = cal;
    return StatusCode::SUCCESS;
  }
  fatal() << "inconsistent number of parameters in Coefficients:" << cal.size() << endmsg;
  m_statusScale = 0;        // invalid luminosity
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Extract data from CoefficientsLog
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheCoefficientDataLog() {
  debug() << "callback CoefficientsLog:" << endmsg;
  std::vector<double> cal = m_condCoefficientsLog->paramVect<double>("CoefficientsLog");
  if ( cal.size() == m_calibCoefficientsLog.size() ) {
    m_calibCoefficientsLog = cal;
    return StatusCode::SUCCESS;
  }
  fatal() << "inconsistent number of parameters in CoefficientsLog:" << cal.size() << endmsg;
  m_statusScale = 0;        // invalid luminosity
  return StatusCode::SUCCESS;

}

//=========================================================================
//  Extract data from AbsoluteCalibration
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheAbsoluteData() {
  debug() << "callback AbsoluteCalibration:" << endmsg;
  m_calibScale = m_condAbsolute->param<double>("Scale");
  m_calibScaleError = m_condAbsolute->param<double>("RelativeError");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from AbsoluteCalibration
//=========================================================================
StatusCode LumiIntegrateFSR::i_cacheSamplingData() {
  debug() << "callback Sampling:" << endmsg;
  m_calibRevolutionFrequency = m_condSampling->param<double>("RevolutionFrequency");
  m_calibRandomFrequencyBB = m_condSampling->param<double>("RandomFrequencyBB");
  m_calibCollidingBunches = m_condSampling->param<int>("CollidingBunches");

  return StatusCode::SUCCESS;
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

//=========================================================================
//  add the FSR data
//=============================================================================
StatusCode LumiIntegrateFSR::add_file() {
  // add the FSRs of all input files at the same time


  // register conditions for database acces
  StatusCode sc0 = registerDB(); // must be executed first
  if ( sc0.isFailure() ) return sc0;  // error printed already
  
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
	for ( unsigned int fkey = 0; fkey != lumiFSRs->size(); fkey++ ) {

	  // trigger database update using the timeSpan FSR
          // if no run number defined, there is no timespan FSR, so skip it
          // todo: might want to check if lengths of containers are different
          //       or always search for the GUID correspondence
          if ( checkfsr[fkey]->runNumbers().size() ) {
            StatusCode sc = trigger_event(primaryFileRecordAddress, fkey_ts);
            fkey_ts++;
          } else {
            warning() << "missing run number at keycount: " << fkey << " " << fkey_ts << " skip db update for this FSR" << endmsg;
          }

      	  // initialize integral with the primary BX
      	  LHCb::LumiIntegral* result = new LHCb::LumiIntegral();
      	  add_fsr(result, primaryFileRecordAddress, 0, fkey);
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
      	    if ( m_subtractBXTypes.end() != find( m_subtractBXTypes.begin(), m_subtractBXTypes.end(), (*bx) ) ) {
      	      factor = -1.;
	      if ( m_calibRelative[LHCb::LumiMethods::CorrectionFlag] != 0) {
		factor = 0;      // no subtraction of EE
	      }
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
	  debug() << "Result for this file (before calibration): " << *result << endmsg;
	  result->scale(one_vector(m_calibRelative, m_calibRelativeLog, LHCb::LumiMethods::PoissonOffset));
	  verbose() << "Result for this file (after calibration): " << *result << endmsg;
	  // simple summing per counter
	  if ( m_integratorTool->integrate( *result ) == StatusCode::FAILURE ) {
	    m_statusScale = 0;        // invalid luminosity
	    error() << "ERROR summing result using tool " << endmsg;
	  }
	  // summing of integral
	  long old_n_runs = n_runs;
	  if ( result->runNumbers().size() ) n_runs = result->runNumbers()[0];
	  else n_runs = 0;
	  double old_scale = rel_scale;
	  rel_scale = m_calibRevolutionFrequency * m_calibCollidingBunches / m_calibRandomFrequencyBB;
	  if ( old_scale != rel_scale || old_n_runs != n_runs ) {
	    info() << "run: " << result->runNumbers()
		   << " Revolution frequency " << m_calibRevolutionFrequency 
		   << " RandomFrequencyBB " << m_calibRandomFrequencyBB 
		   << " CollidingBunches " << m_calibCollidingBunches 
		   << endmsg;
	  }
	  if ( m_integratorTool->integrate( *result, one_vector(m_calibCoefficients, 
								m_calibCoefficientsLog, LHCb::LumiMethods::PoissonOffset), 
					    rel_scale ) == StatusCode::FAILURE ) {
	    m_statusScale = 0;        // invalid luminosity
	    error() << "ERROR integrating luminosity result" << endmsg;
	  }
	  delete result;
	}
      }
      
      // set absolute scales for tool
      double abs_scale = m_statusScale * m_calibScale;
      m_integratorTool->setAbsolute(abs_scale, m_calibScaleError);
      debug() << "Intermediate Integrated luminosity: " << m_integratorTool->lumiValue() << endmsg;
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
StatusCode LumiIntegrateFSR::trigger_event( std::string primaryFileRecordAddress, unsigned int fkey ) {
  StatusCode sc = StatusCode::SUCCESS;

  // search for the TimeSpanFSR 
  std::string timeSpanRecordAddress(primaryFileRecordAddress);
  timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_PrimaryBXType), m_PrimaryBXType.size(), "" );
  timeSpanRecordAddress.replace( timeSpanRecordAddress.find(m_FSRName), m_FSRName.size(), m_TimeSpanFSRName );
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "constructed time span address" << timeSpanRecordAddress << endmsg; 

  // read TimeSpanFSR to prepare DB access 
  if ( !exist<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress) ) {
    if ( msgLevel(MSG::ERROR) ) error() << timeSpanRecordAddress << " not found" << endmsg ;
    return StatusCode::FAILURE;
  } else {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << timeSpanRecordAddress << " found" << endmsg ;
    LHCb::TimeSpanFSRs* timeSpanFSRs = get<LHCb::TimeSpanFSRs>(m_fileRecordSvc, timeSpanRecordAddress);

    // pick the TimeSpanFSR
    LHCb::TimeSpanFSRs::iterator tsfsr = timeSpanFSRs->begin();
    // check index bounds
    unsigned long tsfsr_len = timeSpanFSRs->size();
    if ( (unsigned long)fkey > tsfsr_len-1 ) { 
      warning() << "missing timeSpanFSR - use previous DB conditions" << endmsg;
      return StatusCode::SUCCESS;
    } 
    LHCb::TimeSpanFSR* timeSpanFSR = tsfsr[fkey];
    ulonglong t0 = timeSpanFSR->earliest();
    ulonglong t1 = timeSpanFSR->latest();
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << timeSpanRecordAddress << " READ TimeSpanFSR: " << *timeSpanFSR << endmsg;
      debug() << timeSpanRecordAddress << " interval: " << t0 << "-" << t1 << endmsg;
    }
    if ( t0 == 0 && t1 == 0 ) {
      m_statusScale = 0;        // invalid luminosity: no time span
      error() << "ERROR: no time span defined " << endmsg;
      return StatusCode::FAILURE;
    }
    // the TimeSpanFSRs have now been read -  fake event loop to get update of calibration constants
    m_dds->setEventTime(Gaudi::Time( (t1/2+t0/2)*1000 ));
    debug() << " creating new event " << endmsg;
    sc = updMgrSvc()->newEvent();
    if (sc.isFailure()) {
      m_statusScale = 0;        // invalid luminosity
      error() << "ERROR updating luminosity constants from DB " << endmsg;
    }
  }    
  return runUpdate();
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

