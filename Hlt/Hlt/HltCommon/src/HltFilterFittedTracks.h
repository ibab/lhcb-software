// $Id: HltFilterFittedTracks.h,v 1.1 2010-05-19 04:05:36 gligorov Exp $
#ifndef HLTFILTERFITTEDTRACKS_H 
#define HLTFILTERFITTEDTRACKS_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IParticle2State.h"

/** @class HltFilterFittedTracks HltFilterFittedTracks.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2010-05-14
 */

class HltFilterFittedTracks : public HltAlgorithm {
public: 
  /// Standard constructor
  HltFilterFittedTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltFilterFittedTracks(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  double m_minIPChi2;

  const IDistanceCalculator* m_dist;
  const IParticle2State* m_p2s ;

  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::RecVertex> m_selections;

  AIDA::IHistogram1D* histochi2;
  AIDA::IHistogram1D* histoip;
  AIDA::IHistogram1D* histochi2best;
  AIDA::IHistogram1D* histoipbest;

};

#endif // HLTFILTERFITTEDTRACKS_H
