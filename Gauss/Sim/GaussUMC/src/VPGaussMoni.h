#ifndef VPGAUSSMONI_H 
#define VPGAUSSMONI_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// LHCb
#include "Event/MCHit.h"

/** @class VPGaussMoni VPGaussMoni.h
 *  
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
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  void monitor();

  std::string m_hitsLocation;
  LHCb::MCHits* m_hits;
  bool m_printInfo;
  bool m_detailedMonitor;
  double m_nMCH;
  double m_nMCH2;
  unsigned int m_nEvent;

};
#endif // VPGAUSSMONI_H
