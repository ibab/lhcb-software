// Include files 

// event model
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"

// local
#include "GetLumiParameters.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetLumiParameters
//
// 2010-11-20 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GetLumiParameters )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetLumiParameters::GetLumiParameters( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_condRelative(0),
    m_condRelativeLog(0),
    m_condAbsolute(0),
    m_condCoefficients(0),
    m_condCoefficientsLog(0),
    m_condSampling(0),       
    m_condTrigger(0),
    m_condFilling(0),
    m_tckReader(0)
{
  declareInterface<IGetLumiParameters>(this);

  declareProperty( "InstanceName"              , m_instanceName = "Hlt1LumiODINFilter");
  declareProperty( "UseOnline"                 , m_useOnline    = true);
}
//=============================================================================
// Destructor
//=============================================================================
GetLumiParameters::~GetLumiParameters() {} 

//=============================================================================
// Integrate Lumi FSR data
//=============================================================================
StatusCode GetLumiParameters::initialize(){

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( !sc ) return sc ; 

  m_doneInit = false;
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

  // trigger conditions
  m_knownTCK = 0;
  m_triggerTCK = 0;
  m_odinTotalRate = 0;
  m_odinFraction = 0;
  m_rateHLT = 0;
  m_rateBB = 0;
  for ( int key = 0; key < 4; key++ ) {
    m_lumiPars.push_back(0);
  }

  // filling scheme conditions
  m_B1NBunches = -1;
  m_B2NBunches = -1;
  m_NCollidingBunches = -1;
  m_B1WrongBucketFlag = 1;
  m_B2WrongBucketFlag = 1;
  m_onlineCollidingBunches = -1;
  
  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == 0) {
    m_statusScale = 0;              // invalid luminosity
    return StatusCode::SUCCESS;
  }

  if (m_useOnline){
    m_tckReader = tool<IRateFromTCK>("RateFromTCK",this); // make it private    
  }
  // register conditions for database acces
  sc = registerDB();                // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already

  runUpdate();               // initial update of DB

  m_doneInit = true;
  return StatusCode::SUCCESS; 
}
//=============================================================================
// 
//=============================================================================
// retrieve number of colliding bunches
long GetLumiParameters::CollidingBunches( ) {
  if ( m_useOnline ) {
    processDB(); 
    // from filling scheme
    return m_onlineCollidingBunches;
  } else {
    return m_calibCollidingBunches;
  }
}
// retrieve random bunch-bunch rate in HLT
double GetLumiParameters::RandomRateBB( ) {
  if ( m_useOnline ) {
    processDB(); 
    // rate from HLT and ODIN unless undefined
    if ( m_rateBB != 0 ) return m_rateBB;
    else return m_calibRandomFrequencyBB; 
  } else {
    return m_calibRandomFrequencyBB; 
  }
}

//=============================================================================
// DB access
//=============================================================================
StatusCode GetLumiParameters::registerDB() {
  // register the DB conditions for the update maganer
  if (msgLevel(MSG::DEBUG)) debug() << "==> Register DB" << endmsg;

  // register absolute calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/AbsoluteCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/AbsoluteCalibration",
                      m_condAbsolute, &GetLumiParameters::i_cacheAbsoluteData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/AbsoluteCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // register relative calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibration",
                      m_condRelative, &GetLumiParameters::i_cacheRelativeData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // register relative calibration for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibrationLog")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibrationLog",
                      m_condRelativeLog, &GetLumiParameters::i_cacheRelativeDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibrationLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // register usage coefficients
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Coefficients")) {
    registerCondition("Conditions/Lumi/LHCb/Coefficients",
                      m_condCoefficients, &GetLumiParameters::i_cacheCoefficientData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/Coefficients not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // register usage coefficients for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/CoefficientsLog")) {
    registerCondition("Conditions/Lumi/LHCb/CoefficientsLog",
                      m_condCoefficientsLog, &GetLumiParameters::i_cacheCoefficientDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/CoefficientsLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  // register sampling frequencies of bunches
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Sampling")) {
    registerCondition("Conditions/Lumi/LHCb/Sampling",
                      m_condSampling, &GetLumiParameters::i_cacheSamplingData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/Sampling not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }
  if( m_useOnline ) {
    // register trigger data
    if (this->existDet<Condition>("Conditions/Online/LHCb/RunInfo/Trigger")) {
      registerCondition("Conditions/Online/LHCb/RunInfo/Trigger",
                        m_condTrigger, &GetLumiParameters::i_cacheTriggerData);
      if (msgLevel(MSG::DEBUG)) debug() << "Conditions/Online/LHCb/RunInfo/Trigger found" << endmsg;
    }
    else {
      warning() << "Conditions/Online/LHCb/RunInfo/Trigger not found, fall back to sampling data" << endmsg;
    }
    // register filling scheme data
    if (this->existDet<Condition>("Conditions/Online/LHCb/LHCFillingScheme")) {
      registerCondition("Conditions/Online/LHCb/LHCFillingScheme",
                        m_condFilling, &GetLumiParameters::i_cacheFillingData);
      if (msgLevel(MSG::DEBUG)) debug() << "Conditions/Online/LHCb/LHCFillingScheme found" << endmsg;
    }
    else {
      warning() << "Conditions/Online/LHCb/LHCFillingScheme not found, fall back to sampling data" << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from relativeCalibration
//=========================================================================
StatusCode GetLumiParameters::i_cacheRelativeData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback RelativeCalibration:" << endmsg;
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
StatusCode GetLumiParameters::i_cacheRelativeDataLog() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback RelativeCalibrationLog:" << endmsg;
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
StatusCode GetLumiParameters::i_cacheCoefficientData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback Coefficients:" << endmsg;
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
StatusCode GetLumiParameters::i_cacheCoefficientDataLog() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback CoefficientsLog:" << endmsg;
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
StatusCode GetLumiParameters::i_cacheAbsoluteData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback AbsoluteCalibration:" << endmsg;
  m_calibScale = m_condAbsolute->param<double>("Scale");
  m_calibScaleError = m_condAbsolute->param<double>("RelativeError");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from SamplingData
//=========================================================================
StatusCode GetLumiParameters::i_cacheSamplingData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback Sampling:" << endmsg;
  m_calibRevolutionFrequency = m_condSampling->param<double>("RevolutionFrequency");
  m_calibRandomFrequencyBB = m_condSampling->param<double>("RandomFrequencyBB");
  m_calibCollidingBunches = m_condSampling->param<int>("CollidingBunches");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Trigger
//=========================================================================
StatusCode GetLumiParameters::i_cacheTriggerData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback Trigger:" << endmsg;
  m_triggerTCK = (unsigned int) m_condTrigger->param<int>("TCK");
  m_lumiPars = m_condTrigger->paramVect<double>("LumiPars");

  // derived quantities
  if (msgLevel(MSG::DEBUG)) debug() << "RECALCULATING DERIVED QUANTITIES " << m_triggerTCK << endmsg;
  m_odinTotalRate = 0;
  m_odinFraction = 0;
  for ( unsigned int i = 0; i < m_lumiPars.size(); i++ ) {
    m_odinTotalRate += m_lumiPars[i];
  }
  if ( m_odinTotalRate != 0 ) {
    m_odinFraction = m_lumiPars[0] / m_odinTotalRate;
  }
  // trigger data - enquire TCK
  if ( m_triggerTCK != m_knownTCK ) {
    if ( m_doneInit ) {
      m_knownTCK = m_triggerTCK;
      if (0==m_tckReader) Exception("Trying to call NULL RateFromTCK tool");
      if (m_tckReader->getTCK() != m_knownTCK){
	err() << "TCK from RateFromTCK tool is " << m_tckReader->getTCK() 
	      << " while I expect " << m_knownTCK << endmsg;
	return StatusCode::FAILURE;
      }
      m_rateHLT = m_tckReader->rateFromTCK(m_instanceName);
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Filling scheme
//=========================================================================
StatusCode GetLumiParameters::i_cacheFillingData() {
  if (msgLevel(MSG::DEBUG)) debug() << "callback Filling:" << endmsg;

  m_B1NBunches = (long) m_condFilling->param<int>("B1NBunches");
  m_B2NBunches = (long) m_condFilling->param<int>("B2NBunches");
  m_NCollidingBunches = (long) m_condFilling->param<int>("NCollidingBunches");
  m_B1WrongBucketFlag = (long) m_condFilling->param<int>("B1WrongBucketFlag");
  m_B2WrongBucketFlag = (long) m_condFilling->param<int>("B2WrongBucketFlag");
  return StatusCode::SUCCESS;
}

//=============================================================================
// process DB parameters
//=============================================================================
StatusCode GetLumiParameters::processDB() {
  
  Gaudi::Time xtfound = m_dds->eventTime();
  
  if (msgLevel(MSG::DEBUG)) debug() << "Event time: " << xtfound.format(true, "%Y-%m-%d %H:%M")
                                    << " fLHC "  << m_calibRevolutionFrequency 
                                    << " fLUMI " << m_calibRandomFrequencyBB 
                                    << " nCB "   << m_calibCollidingBunches ;
  for ( long long unsigned int i = 0; i < m_calibRelative.size(); i++ ) {
    if ( msgLevel(MSG::DEBUG)){
      if ( m_calibCoefficientsLog[i] != 0 ) 
        debug() << " LOG# " << i << ":" << m_calibRelativeLog[i];
      if ( m_calibCoefficients[i] != 0 ) {
        if ( i == LHCb::LumiMethods::CorrectionFlag ) debug() << " no EE correction ";
        else debug() << " AVG# " << i << ":" << m_calibRelative[i];
      }
      debug() << endmsg;
    }
  }
  
  // determine random rate from ODIN LumiPars and TCK
  if (msgLevel(MSG::DEBUG)) {
    debug() << "LumiPars: " ;
    for ( unsigned int i = 0; i < m_lumiPars.size(); i++ ) debug() << m_lumiPars[i] << " " ;
    debug() << " odinTotalRate : " << m_odinTotalRate << " "
            << "odinFraction : " << m_odinFraction << " "
            << "TCK : " << m_triggerTCK << " "
            << "HLT rate : " << m_rateHLT << endmsg ;
  }
  
  // HLT rate can never be greater than the ODIN rate (ODIN in kHz), but zero prescale means all
  if ( m_rateHLT > 0 ) {
    m_rateBB = std::min( m_rateHLT, m_odinTotalRate*1000. ) * m_odinFraction;
  }
  else {
    if ( m_calibCollidingBunches > 0 ) {
      m_rateBB = 0;
    }
    else {
      m_rateBB = m_odinTotalRate * 1000. * m_odinFraction;
    }
  }
  
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Random BB rate: " << m_rateBB << endmsg;
    debug() << "B1NBunches : " << m_B1NBunches << " "
            << "B2NBunches : " << m_B2NBunches << " "
            << "NCollidingBunches : " << m_NCollidingBunches << " "
            << "B1WrongBucketFlag : " << m_B1WrongBucketFlag << " "
            << "B2WrongBucketFlag : " << m_B2WrongBucketFlag << endmsg;
  }
  
  // take the colliding bunches from the online unless undefined or bad
  m_onlineCollidingBunches = m_calibCollidingBunches;
  if (m_calibCollidingBunches < 0) {
    if (m_B1WrongBucketFlag == 0 && m_B2WrongBucketFlag == 0) {
      m_onlineCollidingBunches = m_NCollidingBunches;
    } else {
      // period with flagging problems but bunch count OK
      //ulonglong t0 = 1300000000000000;
      //ulonglong t1 = 1303300000000000;
      //if ( xtfound > Gaudi::Time( t0*1000 ) && xtfound < Gaudi::Time( t1*1000 ) ) {
      ulonglong t0sec = 1300000000;
      ulonglong t1sec = 1303300000;
      if ( xtfound > Gaudi::Time( t0sec, 0 ) && xtfound < Gaudi::Time( t1sec, 0 ) ) {
        m_onlineCollidingBunches = m_NCollidingBunches;
      }
    }
  }
  if (msgLevel(MSG::DEBUG)) debug() << "nBunches          : "      
                                    << m_onlineCollidingBunches << endmsg;

  return StatusCode::SUCCESS;
}

