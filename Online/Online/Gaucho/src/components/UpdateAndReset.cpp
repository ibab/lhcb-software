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
  : GaudiAlgorithm(name, ploc)
{
  declareProperty("desiredDeltaTCycle", m_desiredDeltaTCycle = 10);
  declareProperty("disableReadOdin", m_disableReadOdin = 0);
  declareProperty("disableChekInTimer", m_disableChekInTimer = 0);
  declareProperty("disableChekInExecute", m_disableChekInExecute = 0);
  declareProperty("disableUpdateData", m_disableUpdateData = 0);
  declareProperty("disableResetHistos", m_disableResetHistos = 0);

  m_firstExecute = true;
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::initialize() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isSuccess() ) msg << MSG::DEBUG << "GaudiAlgorithm Initialized" << endreq;
  else {
    msg << MSG::FATAL << "GaudiAlgorithm not Initialized" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = serviceLocator()->service("MonitorSvc", m_pGauchoMonitorSvc, false);
  if( sc.isSuccess() ) msg << MSG::DEBUG << "Found the IGauchoMonitorSvc interface" << endreq;
  else {
    msg << MSG::FATAL << "Unable to locate the IGauchoMonitorSvc interface." << endreq;
    return StatusCode::FAILURE;
  }

  if (0 == m_desiredDeltaTCycle){
    msg << MSG::WARNING << "Your algorithm is using the UpdateAndReset algrithm which update and reset data every desiredDeltaTCycle seconds. You didn't fill the desiredDeltaTCycle option in your options file, then we will consider 10 seconds as default." << endreq;
    m_desiredDeltaTCycle = 10;
  }
    
  // In the begining the delayed values are the same as the current values.
  msg << MSG::INFO << "This program will update data every " << m_desiredDeltaTCycle << " seconds !!!" << endreq;

  // The below part is for test
  m_countExecutes = 0; 
  m_deltaTRunTest = 8 * m_desiredDeltaTCycle;

  m_timeStart = GauchoTimer::currentTime();

  m_runNumber = currentRunNumber().first.first;
  m_cycleNumber = currentCycleNumber(m_timeStart).first;
  m_timeFirstEvInRun = m_timeStart;
  m_offsetTimeFirstEvInRun = offsetToBoundary(m_cycleNumber, m_timeFirstEvInRun, true);
  m_timeLastEvInCycle = m_timeStart;
  m_gpsTimeLastEvInCycle = gpsTime();
  m_runStatus = s_statusNoUpdated;
  m_cycleStatus = s_statusNoUpdated;

  if (0==m_disableMonRate) m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_cycleNumber, m_deltaTCycle, m_offsetTimeFirstEvInRun, m_offsetTimeLastEvInCycle, m_offsetGpsTimeLastEvInCycle);

  //DimTimer::start(20); // we wait a few seconds 

  return StatusCode::SUCCESS;
}

void UpdateAndReset::timerHandler()
{
  MsgStream msg(msgSvc(), name());
  if (0 != m_disableChekInTimer) {
    msg << MSG::DEBUG << "===============> Checking changes inside timer handler is disable." << endreq;
    return;
  }
//  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << "********************Inside timerHandler*******************************" << endreq;

  verifyAndProcessRunChange("timer");
  verifyAndProcessCycleChange("timer");

  DimTimer::start(9); // we verify the cycle status every 9 seconds
//  msg << MSG::DEBUG << "***********************End timerHandler*******************************" << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}


//------------------------------------------------------------------------------
StatusCode UpdateAndReset::execute() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

  m_countExecutes++;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  msg << MSG::DEBUG << "Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "********************************************************************" << endreq;

  if (0 == m_disableChekInExecute){
    verifyAndProcessRunChange("execute");
    verifyAndProcessCycleChange("execute");
  }
  else msg << MSG::DEBUG << "===============> Checking changes inside execute method is disable." << endreq;

  // Because the plot method we start the timer after the first execute...
  // This is because of plot method declareinfo in the execute method...
  if (m_firstExecute){ 
   m_firstExecute = false;
   DimTimer::start(9); 
  }
  msg << MSG::DEBUG << "End of Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  return StatusCode::SUCCESS;
}

void UpdateAndReset::verifyAndProcessCycleChange(std::string method) {
  ulonglong currentTime = GauchoTimer::currentTime();
  std::pair<int, bool> cycleNumber = currentCycleNumber(currentTime);
  if (!cycleNumber.second) return; // false means that the cycleNumber wasn't change
  if (s_statusNoUpdated != m_cycleStatus) return; // We update if the cycleStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " Process Triggered by the "<< method <<"() method." << endreq;
  m_cycleStatus = s_statusProcessingUpdate;
  updateData(false); //false means that the update wasn't called when runNumber changed
  m_cycleStatus = s_statusUpdated;
  retrieveCycleNumber(cycleNumber.first);
}

void UpdateAndReset::verifyAndProcessRunChange(std::string method) {
  std::pair<std::pair<int, ulonglong>, bool> runNumber = currentRunNumber(); // if this is too late, we can not avoid two process calling it.
  if (!runNumber.second) return;// false means that the runNumber wasn't change
  if (s_statusNoUpdated != m_runStatus) return; // We update if the runStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq;
  msg << MSG::DEBUG << " Process Triggered by the "<< method <<"() method." << endreq;
  m_runStatus = s_statusProcessingUpdate;
  updateData(true); //true means that the update was called because the runNumber changed
  m_runStatus = s_statusUpdated;
  retrieveRunNumber(runNumber.first.first, runNumber.first.second);
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "finalizing...." << endreq;
  DimTimer::stop();
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveRunNumber(int runNumber, ulonglong gpsTime) {
//------------------------------------------------------------------------------
  m_runNumber = runNumber;
  m_runStatus = s_statusNoUpdated;
  m_timeFirstEvInRun = gpsTime;
  m_offsetTimeFirstEvInRun = offsetToBoundary(currentCycleNumber(m_timeFirstEvInRun).first, m_timeFirstEvInRun, true);
}

//------------------------------------------------------------------------------
// std::pair<int, bool> UpdateAndReset::currentRunNumber() {
std::pair<std::pair<int, ulonglong>, bool> UpdateAndReset::currentRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  int runNumber=0;
  ulonglong gpsTime = GauchoTimer::currentTime();
  bool changed = false;

  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::DEBUG<< "ODIN Bank exist. " <<endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      msg << MSG::DEBUG<< "Getting RunNumber. " <<endreq;
      runNumber = odin->runNumber();
      gpsTime = odin->gpsTime();
      msg << MSG::DEBUG<< "runNumber from ODIN is. " <<endreq;
    }
    else
    {
      msg << MSG::DEBUG<< "ODIN Bank doesn't exist. " <<endreq;
      // When Odin doesn't work
      runNumber = gpsTime/(m_deltaTRunTest*1000000);
    }
  }
  else {
    msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. Then runNumber = 0 and gpsTime = currentTime" <<endreq;
  }

  if (m_runNumber != runNumber) changed = true;

  std::pair<int, ulonglong> runNumberGpsTime = std::pair<int, ulonglong>(runNumber, gpsTime);
  
  // return std::pair<int, bool>(runNumber,changed);
  return std::pair<std::pair<int, ulonglong>, bool>(runNumberGpsTime, changed);
}

//------------------------------------------------------------------------------
ulonglong UpdateAndReset::gpsTime() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::DEBUG<< " Congratulations, you can read the ODIN Bank. " << endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      return odin->gpsTime();
    }
  }
  else msg << MSG::DEBUG<< "===============> Reading Odin bank is disabled. " <<endreq;

  // this is only for test when Odin doesn't work
  ulonglong currentTime = GauchoTimer::currentTime();

  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000000);

  ulonglong gpsTime = ((ulonglong)cycleNumber)*((ulonglong) m_desiredDeltaTCycle*1000000);

  return gpsTime;
}

std::pair<int, bool> UpdateAndReset::currentCycleNumber(ulonglong currentTime) {
  bool changed = false;
  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000000);
  if (m_cycleNumber != cycleNumber ) changed = true;
  return std::pair<int, bool>(cycleNumber,changed);
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveCycleNumber(int cycleNumber) {
//------------------------------------------------------------------------------
  // In order to avoid conflict, the cycle number is updated here and only here
  m_cycleNumber = cycleNumber;
  m_cycleStatus = s_statusNoUpdated;
}

double UpdateAndReset::offsetToBoundary(int cycleNumber, ulonglong time, bool inferior){
  if (inferior) {
    ulonglong timeIniCycle = ((ulonglong) cycleNumber)*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeIniCycle));
  }
  else {
    ulonglong timeEndCycle = ((ulonglong) (cycleNumber+1))*((ulonglong)(m_desiredDeltaTCycle*1000000));
    return ((double)(time - timeEndCycle));
  }
}

void UpdateAndReset::updateData(bool isRunNumberChanged) {
  MsgStream msg( msgSvc(), name() );
  ulonglong currentTime = GauchoTimer::currentTime();
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
  msg << MSG::DEBUG << "************Updating data " << (currentTime - m_timeStart) << " microseconds after start **********" << endreq;
  msg << MSG::DEBUG << "m_runNumber        = " << m_runNumber << endreq;
  msg << MSG::DEBUG << "m_cycleNumber      = " << m_cycleNumber << endreq;
  msg << MSG::DEBUG << "m_timeFirstEvInRun      = " << m_timeFirstEvInRun << endreq;
  msg << MSG::DEBUG << "m_offsetTimeFirstEvInRun      = " << m_offsetTimeFirstEvInRun << endreq;
  m_deltaTCycle = currentTime - m_timeLastEvInCycle;
  msg << MSG::DEBUG << "m_deltaTCycle = " << m_deltaTCycle << " microseconds" << endreq;
  m_timeLastEvInCycle = currentTime;
  m_offsetTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_timeLastEvInCycle, false);
  msg << MSG::DEBUG << "m_timeLastEvInCycle     = " << m_timeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "m_offsetTimeLastEvInCycle     = " << m_offsetTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "deltaT error = " << m_deltaTCycle - m_desiredDeltaTCycle*1000000 << " microseconds" << endreq;
  m_gpsTimeLastEvInCycle = gpsTime();
  m_offsetGpsTimeLastEvInCycle = offsetToBoundary(m_cycleNumber, m_gpsTimeLastEvInCycle, false);
  msg << MSG::DEBUG << "m_gpsTimeLastEvInCycle  = " << m_gpsTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "m_offsetGpsTimeLastEvInCycle  = " << m_offsetGpsTimeLastEvInCycle << endreq;
  msg << MSG::DEBUG << "TimeLastEvent error = " << (m_timeLastEvInCycle - m_gpsTimeLastEvInCycle) << " microseconds" << endreq;

  if (isRunNumberChanged) {
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    else msg << MSG::DEBUG << "===============> Data was not updated because the UpdateData process is disable." << endreq;
    // if (0 == m_disableResetHistos) m_pGauchoMonitorSvc->resetHistos();
    //else  
    msg << MSG::DEBUG << "===============> Histos were not reset because IT'S NOT IMPLEMENTED." << endreq;
  }
  else{
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(false);
    else msg << MSG::DEBUG << "===============> Data was not updated because the UpdateData process is disable." << endreq;
  }
  msg << MSG::DEBUG << "***************************  End updateData  *************************" << endreq;
  msg << MSG::DEBUG << "**********************************************************************" << endreq;
}

