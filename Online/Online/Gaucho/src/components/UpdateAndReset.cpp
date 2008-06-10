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
   m_generatedRunNumber = 0; // to test the run timer generator    
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
  m_deltaTCycle = m_deltaTCycle * 1000.00; // 10 seconds expresed in milliseconds
  m_deltaTRunTest = 4 * m_deltaTCycle;
  // Assign the MonRate information
  // We publish the delayed values because we have the time of the last 
  // event in cycle when a cycle is finished then we have to publish the 
  // information delayed in one cycle. 
  m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumberDelayed, m_cycleNumberDelayed, m_timeFirstEvInRunDelayed, m_timeLastEvInCycleDelayed);

  m_runNumber = getRunNumber();
  
  m_timeFirstEvInRun = GauchoTimer::currentTime();
  m_timeLastEvInCycle = GauchoTimer::currentTime(); // initialization ...
  
  m_cycleNumber = 0;
  m_cycleTimer.start();
  
  m_runTestTimer.start();
  
  // In the begining the delayed values are the same as the current values.
  m_runNumberDelayed = m_runNumber;
  m_cycleNumberDelayed = m_cycleNumber;
  m_timeFirstEvInRunDelayed = m_timeFirstEvInRun;
  m_timeLastEvInCycleDelayed = m_timeLastEvInCycle;
  
  msg << MSG::INFO << " The run number is " << m_runNumber << endreq;
  msg << MSG::INFO << " The m_cycleNumber number is " << m_runNumber << endreq;   
  
  m_countExecutes = 0;
  
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
  msg << MSG::DEBUG << "The values are:" << endreq;
  msg << MSG::DEBUG << "m_runNumber = " << m_runNumber << endreq;
  msg << MSG::DEBUG << "m_cycleNumber = " << m_cycleNumber << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRun = " << m_timeFirstEvInRun << endreq;
  msg << MSG::DEBUG << "m_timeLastEvInCycle (This is not true)= " << m_timeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "********************************************************************" << endreq;
  
    
  int currentRunNumber;
  
  currentRunNumber = getRunNumber();
  
  msg << MSG::INFO << " The current runNumber is " << currentRunNumber  << endreq; 

  //m_cycleNumber = m_countExecutes;
  
  if (currentRunNumber != m_runNumber)
  {
     //fast or other run change update immediately and reset
     msg << MSG::INFO << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq; 
     m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
     m_pGauchoMonitorSvc->resetHistos();
     
     // The runNumber has changed...
     // Then we set the MonRate parameters  
     // (delayed parameters) with the old values 
     m_runNumberDelayed = m_runNumber;
     m_cycleNumberDelayed = m_cycleNumber;
     m_timeFirstEvInRunDelayed = m_timeFirstEvInRun;
     m_timeLastEvInCycleDelayed = GauchoTimer::currentTime();
     
     msg << MSG::DEBUG << "********************************************************************" << endreq;
     msg << MSG::DEBUG << "Because the Run Number changed we publish the old values for MonRate" << endreq;
     msg << MSG::DEBUG << "m_runNumberDelayed = " << m_runNumberDelayed << endreq;
     msg << MSG::DEBUG << "m_cycleNumberDelayed = " << m_cycleNumberDelayed << endreq;
     msg << MSG::DEBUG << "m_timeFirstEvInRunDelayed = " << m_timeFirstEvInRunDelayed << endreq;
     msg << MSG::DEBUG << "m_timeLastEvInCycleDelayed = " << m_timeLastEvInCycleDelayed << endreq;
     msg << MSG::DEBUG << "********************************************************************" << endreq;
     
     // Then we set the current parameters whit the new values
     m_runNumber = currentRunNumber;
     m_cycleNumber = 0;
     m_timeFirstEvInRun = GauchoTimer::currentTime();
     //m_timeLastEvInCycle = ??? // we dont have this value yet
     
     // We reset the cycle timer
     m_cycleTimer.start();
  }
  else 
  {
     //check if update flag is set update if so
     if (m_pGauchoMonitorSvc->getTimerElapsed()) {
       msg << MSG::INFO << " The Timer elapsed is TRUE then we updateAll(false)" << endreq;
       m_pGauchoMonitorSvc->updateAll(false); //the first parameter is the endOfRun flag
     }
     else msg << MSG::INFO << " The Timer elapsed is FALSE" << endreq;
  
     m_cycleElapsedTime = m_cycleTimer.stop();
     msg << MSG::DEBUG << "*****************************************************" << endreq;
     msg << MSG::DEBUG << "Verifying the time elapsed in cycle " << endreq;
     msg << MSG::DEBUG << "m_cycleElapsedTime = " << m_cycleElapsedTime << endreq;
     msg << MSG::DEBUG << "m_deltaTCycle = " << m_deltaTCycle << endreq;
     msg << MSG::DEBUG << "*****************************************************" << endreq;
     
     // if the cycle time elapsed is greater that 
     // the deltaTCycle we change the cycleNumber
     if (m_cycleElapsedTime > m_deltaTCycle ) {
       msg << MSG::DEBUG << "m_cycleElapsedTime > m_deltaTCycle, then we change the cycleNumber" << endreq;
       // The cycleNumber has changed...
       // Then we set the MonRate parameters  
       // (delayed parameters) with the old values 
       m_runNumberDelayed = m_runNumber;
       m_cycleNumberDelayed = m_cycleNumber;
       m_timeFirstEvInRunDelayed = m_timeFirstEvInRun;
       m_timeLastEvInCycleDelayed = GauchoTimer::currentTime();
       
       msg << MSG::DEBUG << "**********************************************************************" << endreq;
       msg << MSG::DEBUG << "Because the Cycle Number changed we publish the old values for MonRate" << endreq;
       msg << MSG::DEBUG << "m_cycleNumberDelayed = " << m_cycleNumberDelayed << endreq;
       msg << MSG::DEBUG << "m_timeLastEvInCycleDelayed = " << m_timeLastEvInCycleDelayed << endreq;
       msg << MSG::DEBUG << "**********************************************************************" << endreq;
     
     // Then we set the current parameters whit the new values
       m_cycleNumber++;
       //m_timeLastEvInCycle = ??? // we dont have this value yet
     
       // We reset the cycle timer
       m_cycleTimer.start();
     }
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
  
//  msg << MSG::WARNING<< "No ODIN Bank found. Can't update DIM services" <<endreq;
  msg << MSG::WARNING<< "For testing we generate a runNumber" <<endreq;
  
  
  m_runTestElapsedTime = m_runTestTimer.stop();
  if (m_runTestElapsedTime > m_deltaTRunTest){
     msg << MSG::WARNING<< "Changing run number" <<endreq;  
     m_generatedRunNumber++; 
     m_runTestElapsedTime = 0;
     m_runTestTimer.start();
  }
  
  return m_generatedRunNumber;
}
