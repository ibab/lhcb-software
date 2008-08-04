#ifndef GAUCHO_UPDATEANDRESET_H
#define GAUCHO_UPDATEANDRESET_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GauchoTimer.h"
#include "dis.hxx"
#include <map>

static const std::string s_statusNoUpdated("NO_UPDATED");
static const std::string s_statusProcessingUpdate("PROCESSINGUPDATE");
static const std::string s_statusUpdated("UPDATED");

class UpdateAndReset : public GaudiAlgorithm, public DimTimer {
public:
   /// Constructor of this form must be provided
   UpdateAndReset(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  void timerHandler();
  
  void retrieveRunNumber(int runNumber);
  void retrieveCycleNumber(int cycleNumber);
  
  std::pair<int, bool> currentRunNumber();
  std::pair<int, bool> currentCycleNumber(ulonglong currentTime);
  
  ulonglong UpdateAndReset::gpsTime();
  
  void updateData(bool isRunNumberChanged);
  
  void verifyAndProcessRunChange(std::string method);
  void verifyAndProcessCycleChange(std::string method);
  
  
private:
  
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  
  int m_desiredDeltaTCycle;// integer because dimTimer can only accept seconds
  
  // MonRate information
  int m_runNumber;  
  int m_cycleNumber;
  ulonglong m_timeStart;
  ulonglong m_timeFirstEvInRun;
  ulonglong m_timeLastEvInCycle;
  ulonglong m_gpsTimeLastEvInCycle;
  double m_deltaTCycle;
  
  // For Testing RunNumber generator
  int m_deltaTRunTest;
  int m_countExecutes;
  int m_runNumberTest;
  
  bool m_firstExecute;
  
  std::string m_cycleStatus;
  std::string m_runStatus;
  
  int m_disableMonRate;
  int m_disableReadOdin;
  int m_disableChekInTimer;
  int m_disableChekInExecute;
  int m_disableUpdateData;
  int m_disableResetHistos;
};

#endif  // GAUCHO_UPDATEANDRESET_H


