// $Id: HltFilterFittedParticles.h,v 1.1 2010-08-19 09:34:40 gligorov Exp $
#ifndef HLTFILTERFITTEDPARTICLES_H 
#define HLTFILTERFITTEDPARTICLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"
#include <Kernel/IDistanceCalculator.h>

/** @class HltFilterFittedParticles HltFilterFittedParticles.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2010-05-14
 */

class HltFilterFittedParticles : public HltAlgorithm {
public: 
  /// Standard constructor
  HltFilterFittedParticles( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltFilterFittedParticles(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  double m_minIPChi2;

  const IDistanceCalculator* m_dist;

  Hlt::SelectionContainer3<LHCb::Particle,LHCb::Particle,LHCb::RecVertex> m_selections;

  AIDA::IHistogram1D* histochi2;
  AIDA::IHistogram1D* histoip;
  AIDA::IHistogram1D* histochi2best;
  AIDA::IHistogram1D* histoipbest;

};

#endif // HLTFILTERFITTEDPARTICLES_H
