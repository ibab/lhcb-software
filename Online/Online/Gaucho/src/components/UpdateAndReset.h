#ifndef GAUCHO_UPDATEANDRESET_H
#define GAUCHO_UPDATEANDRESET_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "Gaucho/IGauchoMonitorSvc.h"
//#include "GauchoTimer.h"
#include "dis.hxx"
#include <map>
#include <sstream>
#include "Event/RawEvent.h"
#include "Gaucho/EoEIncidentListener.h"
#include "Gaucho/Utilities.h"

static const std::string s_statusNoUpdated("NO_UPDATED");
static const std::string s_statusProcessingUpdate("PROCESSINGUPDATE");
static const std::string s_statusUpdated("UPDATED");

class IRegistry;
//class DimService;

class UpdateAndReset : public GaudiAlgorithm{
public:
   /// Constructor of this form must be provided
  UpdateAndReset(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode testeExecute();
  StatusCode start();
  StatusCode stop();
  StatusCode restart();



  void retrieveRunNumber(int runNumber, ulonglong gpsTime);
  void retrieveCycleNumber(int cycleNumber);

  std::pair<std::pair<int, ulonglong>, bool> currentRunNumber();
  std::pair<int, bool> currentCycleNumber(ulonglong currentTime);
  unsigned int currentTCK();
  ulonglong gpsTime();
  IIncidentSvc *m_incs;
  bool m_stopdone;
  void verifyAndProcessRunChange();
  void verifyAndProcessCycleChange(bool isFromTimerHandler);
  double offsetToBoundary(int cycleNumber, ulonglong time, bool inferior);
private:
  bool isSaveCycle(int m_cycleNumber);
  void manageTESHistos(bool list, bool reset, bool save, bool isFromEndOfRun);
  void histogramIdentifier(IRegistry* object, std::vector<std::string> &idList, bool reset, bool save, int &level,
                           TDirectory* rootdir);
  IRegistry* rootObject();
  void getEventChar(int &runno, unsigned int &tck, ulonglong &gps);
private:
  EoEIncidentListener *EoEInc;
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  IHistogramSvc* m_histogramSvc;
  LHCb::RawEvent* m_rawEvt;
  std::vector<std::string> m_bankNames;
  std::vector<LHCb::RawBank::BankType> m_bankTypes;

  int m_desiredDeltaTCycle;// integer because dimTimer can only accept seconds

  // MonRate information
  int m_one;
  int m_runNumber;
  int m_eorNumber;
  unsigned int m_triggerConfigurationKey;
  unsigned int tck;
  ulonglong gpstime;
  int m_cycleNumber;
  int m_firstCycleNumber;
  ulonglong m_timeStart;
  ulonglong m_timeFirstEvInRun;
  double m_offsetTimeFirstEvInRun;
  ulonglong m_timeLastEvInCycle;
  double m_offsetTimeLastEvInCycle;
  double m_gpsTimeLastEvInCycle;
  double m_offsetGpsTimeLastEvInCycle;
  double m_deltaTCycle;
  bool m_abortRetroEvents;

  // For Testing RunNumber generator
  int m_deltaTRunTest;
  int m_runNumberTest;

  int m_teste;

  bool m_firstExecute;
  bool m_doRunChangeInc;
  bool m_ResetonStart;

  std::string m_utgid;
  dyn_string *serviceParts;
  std::string taskName;
  std::string partName;
  std::string instancenumber;
  std::string infoName;

  std::string m_cycleStatus;
  std::string m_runStatus;
  std::string m_infoFileStatus;
  std::string m_MyName;

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
  bool m_publishSavesetLoc;
  DimService *m_dimSvcSaveSetLoc;

};

#endif  // GAUCHO_UPDATEANDRESET_H


