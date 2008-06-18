//	====================================================================
//  UpdateAndReset.cpp
//	--------------------------------------------------------------------
//
//	Package   : Gaucho
//
//	Author    : Eric van Herwijnen
//      Should be run as first algorithm in TopAlg
//      Checks:
//      1) If the run number has changed. If so, updateAll(true) and resetHistos
//      2) If timerelapsed flag is true. If so, updateAll(false)   
//
//	====================================================================


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

  m_runTestElapsedTime = 0.0;
  m_deltaTRunTest = 4 * m_deltaTCycle;
  
  // Assign the MonRate information
  // We publish the delayed values because we have the time of the last 
  // event in cycle when a cycle is finished then we have to publish the 
  // information delayed in one cycle. 

  m_runTestTimer.start();
  m_runNumber = getRunNumber();
  m_cycleNumber = 0;

  m_timeFirstEvInRun = GauchoTimer::currentTime();

  m_runTestTimer.start(); // This is timer to generate runNumbers
  
  // In the begining the delayed values are the same as the current values.
  m_runNumberDelayed = m_runNumber;
  m_cycleNumberDelayed = m_cycleNumber;
  m_timeFirstEvInRunDelayed = m_timeFirstEvInRun;
  m_timeLastEvInCycleDelayed = GauchoTimer::currentTime(); // initialization ...
  
  m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumberDelayed, m_cycleNumberDelayed, m_timeFirstEvInRunDelayed, m_timeLastEvInCycleDelayed);
  
  m_countExecutes = 0;
  
  start(m_deltaTCycle);
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  m_countExecutes++;
  
  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  msg << MSG::DEBUG << "Execute method # " << m_countExecutes << endreq;
  msg << MSG::DEBUG << "********************************************************************" << endreq;
  msg << MSG::DEBUG << "The delayed values published in MonRate are:" << endreq;
  msg << MSG::DEBUG << "m_runNumberDelayed = " << m_runNumberDelayed << endreq;
  msg << MSG::DEBUG << "m_cycleNumberDelayed = " << m_cycleNumberDelayed << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRunDelayed = " << m_timeFirstEvInRunDelayed << endreq;
  msg << MSG::DEBUG << "m_timeLastEvInCycleDelayed = " << m_timeLastEvInCycleDelayed << endreq;
  msg << MSG::DEBUG << "********************************************************************" << endreq;
  msg << MSG::DEBUG << "The Current values are:" << endreq;
  msg << MSG::DEBUG << "m_runNumber = " << m_runNumber << endreq;
  msg << MSG::DEBUG << "m_cycleNumber = " << m_cycleNumber << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRun = " << m_timeFirstEvInRun << endreq;
  msg << MSG::DEBUG << "m_timeLastEvInCycle = We don't have yet this value" << endreq;
  msg << MSG::DEBUG << "********************************************************************" << endreq;
  
  int currentRunNumber = getRunNumber();
  
  msg << MSG::INFO << " The current runNumber is " << currentRunNumber  << endreq; 

  if (currentRunNumber != m_runNumber) 
  {
     msg << MSG::INFO << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq; 
     changeCycle(true, currentRunNumber);
  }
  
  msg << MSG::DEBUG << "End of Execute method # " << m_countExecutes << endreq;
  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  
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
int UpdateAndReset::getRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;
  
  if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
    LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
    return odin->runNumber();
  }
  
  msg << MSG::WARNING<< "No ODIN Bank found. We don't get the runNumber" <<endreq;
  msg << MSG::INFO<< "For testing we generate a runNumber" <<endreq;
  
  m_runTestElapsedTime = m_runTestTimer.stop();
  if (m_runTestElapsedTime > 1000*m_deltaTRunTest){
     msg << MSG::INFO<< "Changing run number becase timer elapsed (" << m_runTestElapsedTime << ") is greather than m_deltaTRunTest (" << 1000*m_deltaTRunTest<< ") " <<endreq;  
     m_generatedRunNumber++; 
     m_runTestElapsedTime = 0;
     m_runTestTimer.start();
  }
  
  return m_generatedRunNumber;
}

void UpdateAndReset::timerHandler()
{       	 
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << "********************Inside timerHandler*******************************" << endreq;
  changeCycle(false, 0);
  msg << MSG::DEBUG << "***********************End timerHandler*******************************" << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}

void UpdateAndReset::changeCycle(bool isRunNumberChanged, int currentRunNumber) {
  MsgStream msg( msgSvc(), name() );
  
  // We change the cycle number 
  // We set the MonRate parameters  
  // (delayed parameters) with the old values 
  std::string changed = "Cycle";
  if (isRunNumberChanged) {
    m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    m_pGauchoMonitorSvc->resetHistos();
    changed = "Run";
  }
  else{
    m_pGauchoMonitorSvc->updateAll(false);
  }
  
  m_runNumberDelayed = m_runNumber;
  m_cycleNumberDelayed = m_cycleNumber;
  m_timeFirstEvInRunDelayed = m_timeFirstEvInRun;
  m_timeLastEvInCycleDelayed = GauchoTimer::currentTime();
  
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << changed <<" Number changed, then Publish the old values for MonRate" << endreq;
  msg << MSG::DEBUG << "m_runNumberDelayed = " << m_runNumberDelayed << endreq;
  msg << MSG::DEBUG << "m_cycleNumberDelayed = " << m_cycleNumberDelayed << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRunDelayed = " << m_timeFirstEvInRunDelayed << endreq;
  msg << MSG::DEBUG << "m_timeLastEvInCycleDelayed = " << m_timeLastEvInCycleDelayed << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
     
  m_cycleNumber++;
  
  if  (isRunNumberChanged) {
    m_runNumber = currentRunNumber;
    m_cycleNumber = 0;
    m_timeFirstEvInRun = GauchoTimer::currentTime();
  }

  stop();
  start(m_deltaTCycle);
}

