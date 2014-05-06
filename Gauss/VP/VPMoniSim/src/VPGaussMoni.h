#ifndef VPGAUSSMONI_H 
#define VPGAUSSMONI_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// LHCb
#include "Event/MCHit.h"

/** @class VPGaussMoni VPGaussMoni.h
 *  Algorithm run in Gauss for monitoring VP MCHits. 
 *
 *  @author Victor Coco based on VeloGaussMoni
 *  @date   2009-06-05
 */

class VPGaussMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VPGaussMoni(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~VPGaussMoni(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  /// Create monitoring plots
  void monitor();

  /// TES location of VP MCHits container
  std::string m_hitsLocation;
  /// Container of VP MCHits 
  LHCb::MCHits* m_hits;
  /// Flag to activate printout of each hit
  bool m_printInfo;
  /// Flag to activate additional set of histograms
  bool m_detailedMonitor;
  /// Total number of MC hits
  double m_nMCH;
  /// Total number of MC hits squared
  double m_nMCH2;
  /// Number of events
  unsigned int m_nEvent;

};
#endif // VPGAUSSMONI_H
