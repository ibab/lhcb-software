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

  if (0 == m_desiredDeltaTCycle){
    msg << MSG::WARNING << "Your algorithm is using the UpdateAndReset algrithm which update and reset data every desiredDeltaTCycle seconds. You didn't fill the desiredDeltaTCycle option in your options file, then we will consider 10 seconds as default." << endreq;
    m_desiredDeltaTCycle = 5;
  }
    
  // In the begining the delayed values are the same as the current values.
  msg << MSG::INFO << "This program will TRY to update data every " << m_desiredDeltaTCycle << " seconds !!!" << endreq;

  // The below part is for test
  m_countExecutes = 0; 
  m_deltaTRunTest = 8 * m_desiredDeltaTCycle;

  m_timeStart = GauchoTimer::currentTime();

  m_runNumber = currentRunNumber().first;
  m_cycleNumber = currentCycleNumber(m_timeStart).first;
  m_timeFirstEvInRun = m_timeStart;
  m_timeLastEvInCycle = m_timeStart;
  m_gpsTimeLastEvInCycle = gpsTime();
  m_runStatus = s_statusNoUpdated;
  m_cycleStatus = s_statusNoUpdated;

  if (0==m_disableMonRate) m_pGauchoMonitorSvc->declareMonRateComplement(m_runNumber, m_cycleNumber, m_deltaTCycle, m_timeFirstEvInRun, m_timeLastEvInCycle, m_gpsTimeLastEvInCycle);

  //DimTimer::start(20); // we wait a few seconds 

  return StatusCode::SUCCESS;
}

void UpdateAndReset::timerHandler()
{
  MsgStream msg(msgSvc(), name());
  if (0 != m_disableChekInTimer) {
    msg << MSG::INFO << "===============> Checking changes inside timer handler is disable." << endreq;
    return;
  }
//  msg << MSG::INFO << "**********************************************************************" << endreq;
//  msg << MSG::INFO << "********************Inside timerHandler*******************************" << endreq;

  verifyAndProcessRunChange("timer");
  verifyAndProcessCycleChange("timer");

  DimTimer::start(1); // we verify the cycle status every 1 second
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

  if (0 == m_disableChekInExecute){
    verifyAndProcessRunChange("execute");
    verifyAndProcessCycleChange("execute");
  }
  //else msg << MSG::INFO << "===============> Checking changes inside execute method is disable." << endreq;

  // Because the plot method we start the timer after the first execute...
  // This is because of plot method declareinfo in the execute method...
  if (m_firstExecute){ 
   m_firstExecute = false;
   DimTimer::start(1); 
  }
//  msg << MSG::DEBUG << "End of Execute method # " << m_countExecutes << endreq;
//  msg << MSG::DEBUG << "*****************************************************************************" << endreq;
  return StatusCode::SUCCESS;
}

void UpdateAndReset::verifyAndProcessCycleChange(std::string method) {
  ulonglong currentTime = GauchoTimer::currentTime();
  std::pair<int, bool> cycleNumber = currentCycleNumber(currentTime);
  if (!cycleNumber.second) return; // false means that the cycleNumber wasn't change
  if (s_statusNoUpdated != m_cycleStatus) return; // We update if the cycleStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << " Process Triggered by the "<< method <<"() method." << endreq;
  m_cycleStatus = s_statusProcessingUpdate;
  updateData(false); //false means that the update wasn't called when runNumber changed
  m_cycleStatus = s_statusUpdated;
  retrieveCycleNumber(cycleNumber.first);
}

void UpdateAndReset::verifyAndProcessRunChange(std::string method) {
  std::pair<int, bool> runNumber = currentRunNumber(); // if this process is too late, then we can not avoid two process calling this method.
  if (!runNumber.second) return;// false means that the runNumber wasn't change
  if (s_statusNoUpdated != m_runStatus) return; // We update if the runStatus is different of ProcessingUpdate and Updated
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << " The Run Number has changed then we UpdateAll and reset Histograms" << endreq;
  msg << MSG::INFO << " Process Triggered by the "<< method <<"() method." << endreq;
  m_runStatus = s_statusProcessingUpdate;
  updateData(true); //true means that the update was called because the runNumber changed
  m_runStatus = s_statusUpdated;
  retrieveRunNumber(runNumber.first);
}

//------------------------------------------------------------------------------
StatusCode UpdateAndReset::finalize() {
//------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void UpdateAndReset::retrieveRunNumber(int runNumber) {
//------------------------------------------------------------------------------
  m_runNumber = runNumber;
  m_runStatus = s_statusNoUpdated;
  m_timeFirstEvInRun = GauchoTimer::currentTime();
}

//------------------------------------------------------------------------------
std::pair<int, bool> UpdateAndReset::currentRunNumber() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );
  int runNumber=0;
  bool changed = false;

  //msg << MSG::INFO<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::INFO<< "ODIN Bank exist. " <<endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      msg << MSG::INFO<< "Getting RunNumber. " <<endreq;
      runNumber = odin->runNumber();
      msg << MSG::INFO<< "runNumber from ODIN is. " <<endreq;
    }
    else
    {
      //msg << MSG::INFO<< "ODIN Bank doesn't exist. " <<endreq;
      // this is only for test when Odin doesn't work
      ulonglong currentTime = GauchoTimer::currentTime();
      runNumber = currentTime/(m_deltaTRunTest*1000);
    }
  }
  else {
    //msg << MSG::INFO<< "===============> Reading Odin data base process is disable. " <<endreq;
    ulonglong currentTime = GauchoTimer::currentTime();
    runNumber = currentTime/(m_deltaTRunTest*1000);
  }

  if (m_runNumber != runNumber) changed = true;

  return std::pair<int, bool>(runNumber,changed);

}

//------------------------------------------------------------------------------
ulonglong UpdateAndReset::gpsTime() {
//------------------------------------------------------------------------------
  MsgStream msg( msgSvc(), name() );

//  msg << MSG::DEBUG<< "Reading ODIN Bank. " <<endreq;

  if (0 == m_disableReadOdin) {
    if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
      msg << MSG::INFO<< " Congratulations, you can read the ODIN Bank. " << endreq;
      LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
      return odin->gpsTime();
    }
  }
  else msg << MSG::INFO<< "===============> Reading Odin data base process is disable. " <<endreq;

  // this is only for test when Odin doesn't work
  ulonglong currentTime = GauchoTimer::currentTime();

  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000);

  ulonglong gpsTime = ((ulonglong)cycleNumber)*((ulonglong) m_desiredDeltaTCycle*1000);

  return gpsTime;
}

std::pair<int, bool> UpdateAndReset::currentCycleNumber(ulonglong currentTime) {
  bool changed = false;
  int cycleNumber = currentTime/(m_desiredDeltaTCycle*1000);
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


void UpdateAndReset::updateData(bool isRunNumberChanged) {
  MsgStream msg( msgSvc(), name() );
  ulonglong currentTime = GauchoTimer::currentTime();
  msg << MSG::INFO << "**********************************************************************" << endreq;
  msg << MSG::INFO << "************Updating data " << (currentTime - m_timeStart) << " milliseconds after start **********" << endreq;
  msg << MSG::INFO << "m_runNumber        = " << m_runNumber << endreq;
  msg << MSG::INFO << "m_cycleNumber      = " << m_cycleNumber << endreq;
  msg << MSG::INFO << "m_timeFirstEvInRun      = " << m_timeFirstEvInRun << endreq;
  m_deltaTCycle = currentTime - m_timeLastEvInCycle;
  msg << MSG::INFO << "m_deltaTCycle = " << m_deltaTCycle << " milliseconds" << endreq;
  m_timeLastEvInCycle = currentTime;
  msg << MSG::INFO << "m_timeLastEvInCycle     = " << m_timeLastEvInCycle << endreq;
  msg << MSG::INFO << "deltaT error = " << m_deltaTCycle - m_desiredDeltaTCycle*1000 << " milliseconds" << endreq;
  m_gpsTimeLastEvInCycle = gpsTime();
  msg << MSG::INFO << "m_gpsTimeLastEvInCycle  = " << m_gpsTimeLastEvInCycle << endreq;
  msg << MSG::INFO << "TimeLastEvent error = " << (m_timeLastEvInCycle - m_gpsTimeLastEvInCycle) << " milliseconds" << endreq;

  if (isRunNumberChanged) {
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(true); //the first parameter is the endOfRun flag
    else msg << MSG::INFO << "===============> Data was not updated because the UpdateData process is disable." << endreq;
    if (0 == m_disableResetHistos) m_pGauchoMonitorSvc->resetHistos();
    else  msg << MSG::INFO << "===============> Histos were not reseted because the ResetHistos process is disable." << endreq;
  }
  else{
    if (0 == m_disableUpdateData) m_pGauchoMonitorSvc->updateAll(false);
    else msg << MSG::INFO << "===============> Data was not updated because the UpdateData process is disable." << endreq;
  }
  msg << MSG::INFO << "***************************  End updateData  *************************" << endreq;
  msg << MSG::INFO << "**********************************************************************" << endreq;
}

