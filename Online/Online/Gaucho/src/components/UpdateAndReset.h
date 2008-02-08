#ifndef GAUCHO_UPDATEANDRESET_H
#define GAUCHO_UPDATEANDRESET_H 1


// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"

class UpdateAndReset : public GaudiAlgorithm {
public:
   /// Constructor of this form must be provided
   UpdateAndReset(const std::string& name, ISvcLocator* pSvcLocator); 
   
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  int m_runNumber;  
  int m_lastRefreshTime; 
  time_t m_currentTime;
  time_t m_timeOfLastRefresh;
  
//  MonTimer* m_dimtimer;
  bool m_timerElapsed;
//  int m_refreshTime;
};  

#endif  // GAUCHO_UPDATEANDRESET_H
