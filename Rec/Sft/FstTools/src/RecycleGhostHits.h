#ifndef _RecycleGhostHits_H_
#define _RecycleGhostHits_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "PatKernel/PatTStationHitManager.h"
#include "PatKernel/PatForwardHit.h"
#include "Kernel/LHCbID.h"
#include <algorithm>

class RecycleGhostHits: public GaudiAlgorithm {
 public:
  // Constructors and destructor
  RecycleGhostHits(const std::string& name,
                   ISvcLocator* pSvcLocator);
  virtual ~RecycleGhostHits();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  virtual StatusCode execute();
  
 private:
  std::string m_inputLocation;
  std::string m_outputLocation;
  ToolHandle<ITrackSelector> m_selector;
  Tf::TStationHitManager <PatForwardHit>* m_tHitManager;
};
#endif
