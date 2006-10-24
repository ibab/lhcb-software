// $Id: HltRecCheckUtils.h,v 1.1 2006-10-24 09:50:49 hernando Exp $
#ifndef HLTRECCHECKUTILS_H 
#define HLTRECCHECKUTILS_H 1

// Include files
#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"

namespace MCHlt 
{

  // returns the ancestors of the mcparticle
  int ancestors(const LHCb::MCParticle& mcpar,
                std::vector<LHCb::MCParticle*>& ancestors);

  // return the eva mother of this mcparticle
  const LHCb::MCParticle& ancestor(const LHCb::MCParticle& mcpar);

  // return the quark content of the mcparticles
  int iquark(const LHCb::MCParticles& mcpars);
 
  // return the quatk of this mcparticle
  int iquark(const LHCb::MCParticle& par);

  void toTrack(const LHCb::MCParticle& mcpar, LHCb::Track& track);
  
  void to2DTrack(const LHCb::MCParticle& mcpar, LHCb::Track& track);
  
  LHCb::RecVertex toOriginVertex(const LHCb::MCParticle& mcpar);


};
#endif // HLTRECCHECKUTILS_H
