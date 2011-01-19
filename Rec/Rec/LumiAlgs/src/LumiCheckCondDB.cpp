// $Id: LumiCheckCondDB.cpp,v 1.8 2010-05-13 11:57:05 rlambert Exp $
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
#include "LumiCheckCondDB.h"
#include "LumiIntegrator.h"
#include "FSRNavigator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCheckCondDB
// 
// 2009-02-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCheckCondDB );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCheckCondDB::LumiCheckCondDB( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_condRelative(NULL),
    m_condRelativeLog(NULL),
    m_condAbsolute(NULL),
    m_condCoefficients(NULL),
    m_condCoefficientsLog(NULL),
    m_condSampling(NULL),
    m_condGUIDs(NULL),
    m_calibRelative(0),
    m_calibCoefficients(0),
    m_calibRelativeLog(0),
    m_calibCoefficientsLog(0),
    m_statusScale(0.),
    m_calibScale(0.),
    m_calibScaleError(0.),
    m_calibRevolutionFrequency(0.),
    m_calibRandomFrequencyBB(0.),
    m_calibCollidingBunches(0)
{
  // expect the data to be written at LHCb::LumiFSRLocation::Default
  declareProperty( "StartTime"          , m_startTime         = 1269817249  );
  declareProperty( "NumberSteps"        , m_numberSteps       = 80  );
  declareProperty( "StepHours"          , m_stepHours         = 72  );
}
//=============================================================================
// Destructor
//=============================================================================
LumiCheckCondDB::~LumiCheckCondDB() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiCheckCondDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

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
    m_statusScale = 0;              // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register conditions for database acces
  sc = registerDB();                // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already

  return runUpdate();               // initial update of DB
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCheckCondDB::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiCheckCondDB::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << "========== Probe DB: START ==========" << endmsg;

  unsigned long long xt0 = m_startTime ;
  for ( unsigned long long ixt = 0; ixt < m_numberSteps; ixt++ ) {
    unsigned long long xt = ixt*3600*m_stepHours + xt0;
    Gaudi::Time gxt = xt * 1000*1000*1000;
    m_dds->setEventTime( gxt );
    StatusCode sc = updMgrSvc()->newEvent( gxt );
    if (sc.isFailure()) {
      error() << "ERROR newEvent " << endmsg;
    }
    sc = runUpdate();
    if (sc.isFailure()) {
      error() << "ERROR runUpdate " << endmsg;
    }
    info() << " t0 "    << xt 
	   << " " << gxt.format(true, "%Y-%m-%d %H:%M")
	   << " fLHC "  << m_calibRevolutionFrequency 
	   << " fLUMI " << m_calibRandomFrequencyBB 
	   << " nCB "   << m_calibCollidingBunches ;
    for ( long long unsigned int i = 0; i < m_calibRelative.size(); i++ ) {
      if ( m_calibCoefficientsLog[i] != 0 ) info() << " LOG# " << i << ":" << m_calibRelativeLog[i];
      if ( m_calibCoefficients[i] != 0 ) {
	if ( i == LHCb::LumiMethods::CorrectionFlag ) info() << " no EE correction ";
	else info() << " AVG# " << i << ":" << m_calibRelative[i];
      }
    }
    info() << endmsg;
    Gaudi::Time xtfound = m_dds->eventTime();
    if ( xtfound != gxt ) {
      error() << "different time read back: found " << xtfound  << " " << gxt 
	      << endmsg;
    }
  }    

  info() << "========== Probe DB: END   ==========" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// DB access
//=============================================================================
StatusCode LumiCheckCondDB::registerDB() {
  // register the DB conditions for the update maganer
  debug() << "==> Register DB" << endmsg;

  // register absolute calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/AbsoluteCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/AbsoluteCalibration",
                      m_condAbsolute, &LumiCheckCondDB::i_cacheAbsoluteData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/AbsoluteCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register relative calibration
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibration")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibration",
                      m_condRelative, &LumiCheckCondDB::i_cacheRelativeData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibration not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register relative calibration for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/RelativeCalibrationLog")) {
    registerCondition("Conditions/Lumi/LHCb/RelativeCalibrationLog",
                      m_condRelativeLog, &LumiCheckCondDB::i_cacheRelativeDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/RelativeCalibrationLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register usage coefficients
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Coefficients")) {
    registerCondition("Conditions/Lumi/LHCb/Coefficients",
                      m_condCoefficients, &LumiCheckCondDB::i_cacheCoefficientData);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/Coefficients not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register usage coefficients for -log method
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/CoefficientsLog")) {
    registerCondition("Conditions/Lumi/LHCb/CoefficientsLog",
                      m_condCoefficientsLog, &LumiCheckCondDB::i_cacheCoefficientDataLog);
  }
  else {
    fatal() << "Conditions/Lumi/LHCb/CoefficientsLog not found" << endmsg;
    m_statusScale = 0;        // invalid luminosity
    return StatusCode::SUCCESS;
  }

  // register sampling frequencies of bunches
  if (this->existDet<Condition>("Conditions/Lumi/LHCb/Sampling")) {
    registerCondition("Conditions/Lumi/LHCb/Sampling",
                      m_condSampling, &LumiCheckCondDB::i_cacheSamplingData);
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
StatusCode LumiCheckCondDB::i_cacheRelativeData() {
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
StatusCode LumiCheckCondDB::i_cacheRelativeDataLog() {
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
StatusCode LumiCheckCondDB::i_cacheCoefficientData() {
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
StatusCode LumiCheckCondDB::i_cacheCoefficientDataLog() {
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
StatusCode LumiCheckCondDB::i_cacheAbsoluteData() {
  debug() << "callback AbsoluteCalibration:" << endmsg;
  m_calibScale = m_condAbsolute->param<double>("Scale");
  m_calibScaleError = m_condAbsolute->param<double>("RelativeError");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from AbsoluteCalibration
//=========================================================================
StatusCode LumiCheckCondDB::i_cacheSamplingData() {
  debug() << "callback Sampling:" << endmsg;
  m_calibRevolutionFrequency = m_condSampling->param<double>("RevolutionFrequency");
  m_calibRandomFrequencyBB = m_condSampling->param<double>("RandomFrequencyBB");
  m_calibCollidingBunches = m_condSampling->param<int>("CollidingBunches");

  return StatusCode::SUCCESS;
}
