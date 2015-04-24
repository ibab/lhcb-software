#ifndef PRPIXELSTORECLUSTERS_H
#define PRPIXELSTORECLUSTERS_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include "PrPixelHitManager.h"

/** @class PrPixelStoreClusters
 *
 *  Store clusters created in the hit manager on the TES.
 *
 *  This is meant to run after the pattern recognition. It calls
 *  into the PrPixelHitManager to store the clusters. So not much
 *  is happening here, it's simply a wrapper to put the cluster
 *  creation on the TES in the algorithm sequence.
 *
 *  @author Kurt Rinnert
 *  @date   2014-02-26
 */

class PrPixelStoreClusters : public GaudiAlgorithm {

 public:
  /// Standard constructor
  PrPixelStoreClusters(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~PrPixelStoreClusters();  ///< Destructor
  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// Where to store the clusters.
  std::string m_clusterLocation;

  /// Hit manager tool
  PrPixelHitManager *m_hitManager;

  /// Fast check for debug configuration
  bool m_isDebug;
};

#endif  // PRPIXELSTORECLUSTERS_H
