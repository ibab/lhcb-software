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

class UpdateAndReset : public GaudiAlgorithm, public DimTimer {
public:
   /// Constructor of this form must be provided
   UpdateAndReset(const std::string& name, ISvcLocator* pSvcLocator); 
   
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  //int getRunNumber();
  
  void timerHandler();
  
  //void changeCycle(bool isRunNumberChanged, int currentRunNumber);
  //void getCycleNumber(std::string deltaTCycle);

  int retrieveRunNumber();
  int retrieveCycleNumber();
  
  int cycleNumber(longlong currentTime);
  
  longlong timeNextUpdate(longlong currentTime);
  void updateData(bool isRunNumberChanged);
  
private:
  
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  
  int m_deltaTCycle;
  
  // MonRate information
  int m_runNumber;  
  int m_cycleNumber;
  longlong m_timeStart;
  longlong m_timeFirstEvInRun;
  longlong m_timeLastEvInCycle;
  longlong m_timeTrueLastEvInCycle;
  
  // For Testing RunNumber generator
  int m_generatedRunNumber;
  int m_countExecutes;
  double m_runTestElapsedTime;
  GauchoTimer m_runTestTimer;
  double m_deltaTRunTest;
  
};  

#endif  // GAUCHO_UPDATEANDRESET_H


