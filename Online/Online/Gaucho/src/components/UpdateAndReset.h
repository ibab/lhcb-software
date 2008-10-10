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

class IRegistry;

class UpdateAndReset : public GaudiAlgorithm, public DimTimer {
public:
   /// Constructor of this form must be provided
  UpdateAndReset(const std::string& name, ISvcLocator* pSvcLocator); 
   
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode testeExecute();
  
    
  void timerHandler();
  
  void retrieveRunNumber(int runNumber, ulonglong gpsTime);
  void retrieveCycleNumber(int cycleNumber);
  
  std::pair<std::pair<int, ulonglong>, bool> currentRunNumber();
  std::pair<int, bool> currentCycleNumber(ulonglong currentTime);
  
  ulonglong UpdateAndReset::gpsTime();
  
  void updateData(bool isRunNumberChanged, bool isFromTimerHandler);
  
  void verifyAndProcessRunChange();
  void verifyAndProcessCycleChange(bool isFromTimerHandler);
  double offsetToBoundary(int cycleNumber, ulonglong time, bool inferior);
private:
  bool isSaveCycle(int m_cycleNumber);
  void manageTESHistos(bool list, bool reset, bool save, bool isFromEndOfRun);
  void histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level);
  IRegistry* rootObject();
  
private:
  
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  IHistogramSvc* m_histogramSvc;

    
  int m_desiredDeltaTCycle;// integer because dimTimer can only accept seconds
  
  // MonRate information
  int m_runNumber;  
  int m_cycleNumber;
  int m_firstCycleNumber;
  ulonglong m_timeStart;
  ulonglong m_timeFirstEvInRun;
  double m_offsetTimeFirstEvInRun;
  ulonglong m_timeLastEvInCycle;
  double m_offsetTimeLastEvInCycle;
  ulonglong m_gpsTimeLastEvInCycle;
  double m_offsetGpsTimeLastEvInCycle;
  double m_deltaTCycle;
  
  // For Testing RunNumber generator
  int m_deltaTRunTest;
  int m_countExecutes;
  int m_runNumberTest;
  
  int m_teste;
  
  bool m_firstExecute;
  
  std::string m_utgid;
  std::string m_cycleStatus;
  std::string m_runStatus;
  std::string m_infoFileStatus;
  
  int m_disableMonRate;
  int m_disableReadOdin;
  int m_disableChekInTimer;
  int m_disableChekInExecute;
  int m_disableUpdateData;
  int m_disableResetHistos;
  int m_saveHistograms;
  std::string m_saveSetDir;
  int m_saverCycle;
  int m_timerCycle;
  int m_numCyclesToSave;
  int m_resetHistosAfterSave;
  
};

#endif  // GAUCHO_UPDATEANDRESET_H


