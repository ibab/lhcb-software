#ifndef PRCHEATEDVP_H 
#define PRCHEATEDVP_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/DigiEvent
#include "Event/VPCluster.h"

/** @class PrCheatedVP PrCheatedVP.h
 *  Cheated pattern recognition for the upgraded VELO
 *
 *  @author Olivier Callot
 *  @date   2012-07-26
 */

class PrCheatedVP : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrCheatedVP(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrCheatedVP();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  bool m_useMCHits;
};

#endif 
