//      ====================================================================
//      UpdateAndReset.cpp
//      --------------------------------------------------------------------
//
//      Package   : Gaucho
//
//      Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)   
//
//      ====================================================================


// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/ServiceLocatorHelper.h"

#include "RTL/rtl.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "Event/RecHeader.h"

#include "UpdateAndReset.h"
#include "MonitorSvc.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(UpdateAndReset)


// Constructor
//------------------------------------------------------------------------------
UpdateAndReset::UpdateAndReset(const std::string& name, ISvcLocator* ploc)
  : GaudiAlgorithm(name, ploc), m_deltaTCycle(5)
{
  declareProperty("deltaTCycle", m_deltaTCycle);
  m_generatedRunNumber = 0; 
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isSuccess() ) msg << MSG::INFO << "GaudiAlgorithm Initialized" << endreq;
  else {
    msg << MSG::FATAL << "GaudiAlgorithm not Initialized" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = serviceLocator()->service("MonitorSvc", m_pGauchoMonitorSvc, false);
  if( sc.isSuccess() ) msg << MSG::INFO << "Found the IGauchoMonitorSvc interface" << endreq;
  else { 
    msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
    return StatusCode::FAILURE;
  }
  
  // In the begining the delayed values are the same as the current values.
  
  // The below part is for test
  m_countExecutes = 0; 
  m_runTestElapsedTime = 0.0; 
  m_deltaTRunTest = 8 * m_deltaTCycle; 
  m_runTestTimer.start(); 
  m_timeFirstEvInRun = GauchoTimer::currentTime(); 
  m_timeStart = m_timeFirstEvInRun;
  // The above part is for test 
  
  retrieveRunNumber();
  m_cycleNumber = 0;
  m_timeTrueLastEvInCycle = m_timeFirstEvInRun;
  
  msg << MSG::INFO << " m_timeTrueLastEvInCycle = " << m_timeTrueLastEvInCycle << endreq;
  m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_cycleNumber, m_timeFirstEvInRun, m_timeTrueLastEvInCycle); 
  msg << MSG::INFO << " m_timeTrueLastEvInCycle = " << m_timeTrueLastEvInCycle << endreq;
  
  longlong currentTime = GauchoTimer::currentTime();
  longlong timeNextUp  = timeNextUpdate(currentTime);
  
  //int timeToWait = ((int)(timeNextUp - currentTime)); // milliseconds
  int timeToWait = ((int)(timeNextUp - currentTime))/1000; // seconds
  msg << MSG::INFO << " timeToWait = " << timeToWait << endreq;
  
  start(timeToWait); //start seconds
  
  return StatusCode::SUCCESS;
}

void UpdateAndReset::timerHandler()
{
  MsgStream msg(msgSvc(), name());
//  msg << MSG::INFO << "**********************************************************************" << endreq;
//  msg << MSG::INFO << "********************Inside timerHandler*******************************" << endreq;
  start(m_deltaTCycle); // seconds
  retrieveCycleNumber();
  updateData(false);
//  msg << MSG::INFO << "***********************End timerHandler*******************************" << endreq;
//  msg << MSG::INFO << "**********************************************************************" << endreq;

}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  m_countExecutes++;
  
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
//  msg << MSG::DEBUG << "Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "********************************************************************" << endreq;
  
  if (0 != retrieveRunNumber()) {
    msg << MSG::INFO << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq; 
    retrieveCycleNumber();
    updateData(true);
  }
  
//  msg << MSG::DEBUG << "End of Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
int UpdateAndReset::retrieveRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  int currentRunNumber=0;
  
//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;
  
  if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
    LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
    currentRunNumber = odin->runNumber();
  }
  else {
  
//    msg << MSG::WARNING<< "No ODIN Bank found. We don't get the runNumber" <<endreq;
//    msg << MSG::INFO<< "For testing we generate a runNumber" <<endreq;
  
    m_runTestElapsedTime = m_runTestTimer.stop();
    if (m_runTestElapsedTime > 1000*m_deltaTRunTest){
//       msg << MSG::INFO<< "Changing run number becase timer elapsed (" << m_runTestElapsedTime << ") is greather than m_deltaTRunTest (" << 1000*m_deltaTRunTest<< ") " <<endreq;  
       m_generatedRunNumber++; 
       m_runTestElapsedTime = 0;
       m_runTestTimer.start();
    }
    
    currentRunNumber = m_generatedRunNumber;
  }
  
  if (m_runNumber != currentRunNumber) {
    m_runNumber = currentRunNumber;
    m_timeFirstEvInRun = GauchoTimer::currentTime();
    return 1;
  }
  
  return 0;
}

int UpdateAndReset::cycleNumber(longlong currentTime) {
  return currentTime/(m_deltaTCycle*1000);
}

//------------------------------------------------------------------------------
int UpdateAndReset::retrieveCycleNumber() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
//  msg << MSG::INFO << "**********************************************************************" << endreq;
//  msg << MSG::INFO << "********************Inside retrieveCycleNumber************************" << endreq;
  longlong currentTime = GauchoTimer::currentTime();
  int currentCycleNumber = cycleNumber(currentTime);
//  msg << MSG::INFO << "m_cycleNumber = " << m_cycleNumber << endreq;
//  msg << MSG::INFO << "currentCycleNumber = " << currentCycleNumber << endreq;
  
  if (m_cycleNumber != currentCycleNumber) {
    m_cycleNumber = currentCycleNumber;
    m_timeLastEvInCycle = (((longlong)m_cycleNumber) + 1)*((longlong) m_deltaTCycle)*1000;
//    msg << MSG::INFO << "We change th cycle number " << endreq;
//    msg << MSG::INFO << "m_timeLastEvInCycle = " << m_timeLastEvInCycle << endreq;
//    msg << MSG::INFO << "***********************End retrieveCycleNumber************************" << endreq;
//    msg << MSG::INFO << "**********************************************************************" << endreq;
    return 1;
  }
  
//  msg << MSG::INFO << "***********************End retrieveCycleNumber************************" << endreq;
//  msg << MSG::INFO << "**********************************************************************" << endreq;
  
  return 0;
}

longlong UpdateAndReset::timeNextUpdate(longlong currentTime) {
  return (((longlong) cycleNumber(currentTime)) + 1)*((longlong) m_deltaTCycle)*1000;
}

void UpdateAndReset::updateData(bool isRunNumberChanged) {
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO << "**********************************************************************" << endreq;
  msg << MSG::INFO << "************Updating data " << (GauchoTimer::currentTime() - m_timeStart) << " milliseconds after start **********" << endreq;
  msg << MSG::INFO << "************Updating data " << (GauchoTimer::currentTime() - m_timeTrueLastEvInCycle) << " milliseconds after last update " << endreq;
  
  m_timeTrueLastEvInCycle = GauchoTimer::currentTime();
 
  // We change the cycle number 
  // We set the MonRate parameters  
  // (delayed parameters) with the old values 
  msg << MSG::INFO << "m_runNumber        = " << m_runNumber << endreq;
  msg << MSG::INFO << "m_cycleNumber      = " << m_cycleNumber << endreq;
  msg << MSG::INFO << "m_timeFirstEvInRun      = " << m_timeFirstEvInRun << endreq;
  msg << MSG::INFO << "m_timeLastEvInCycle     = " << m_timeLastEvInCycle << endreq;
  msg << MSG::INFO << "m_timeTrueLastEvInCycle = " << m_timeTrueLastEvInCycle << endreq;
  msg << MSG::INFO << "Time ERROR = " << m_timeTrueLastEvInCycle - m_timeLastEvInCycle << endreq;
  
  std::string changed = "Cycle";
  if (isRunNumberChanged) {
    m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    m_pGauchoMonitorSvc->resetHistos();
    changed = "Run";
  }
  else{
    m_pGauchoMonitorSvc->updateAll(false);
  }
  msg << MSG::INFO << "***************************  End updateData  *************************" << endreq;
  msg << MSG::INFO << "**********************************************************************" << endreq;
}

