#ifndef _TrackerMCParticle2MCHitAlg_H_
#define _TrackerMCParticle2MCHitAlg_H_
/** @class TrackerMCParticle2MCHitAlg TrackerMCParticle2MCHitAlg.h
*
*
*   @author: M. Needham
*   @date 03-07-2002
*/

// STL
#include <string>
#include <vector>

// GaudiKernel
#include "GaudiAlg/GaudiAlgorithm.h"


class TrackerMCParticle2MCHitAlg : public GaudiAlgorithm {
  
public:

  /// Standard constructor
  TrackerMCParticle2MCHitAlg(const std::string& name, 
                             ISvcLocator* pSvcLocator );

  virtual ~TrackerMCParticle2MCHitAlg(); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  std::vector<std::string> m_dets;
  std::string m_outputData;  ///< location of Particles to associate
  
};

#endif // _TrackerMCParticle2MCHitAlg_H_
