#ifndef PRPIXELMONITOR_H
#define PRPIXELMONITOR_H 1

#include "GaudiAlg/GaudiTupleAlg.h"

#include "PrPixelHitManager.h"
#include "PrPixelTrack.h"

/** @class PrPixelMonitor PrPixelMonitor.h
 *
 *  Monitoring and debugging plots for PrPixelTracking.
 *
 */

class PrPixelMonitor : public GaudiTupleAlg {

 public:
  /// Standard constructor
  PrPixelMonitor(const std::string &name, ISvcLocator *pSvcLocator);
  /// Destructor
  virtual ~PrPixelMonitor();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// Hit manager tool
  PrPixelHitManager *m_hitManager;

};
#endif
