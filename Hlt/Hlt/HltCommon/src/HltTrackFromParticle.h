// $Id: HltTrackFromParticle.h,v 1.6 2009-12-23 16:14:31 graven Exp $
#ifndef HLTTRACKFROMPARTICLE_H 
#define HLTTRACKFROMPARTICLE_H 1

// Include files
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"

/** @class HltTrackFromParticle HltTrackFromParticle.h
 *  
 *
 *  @author Jose A. Hernando
 *  @date   2007-06-04
 */

class HltTrackFromParticle : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackFromParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackFromParticle(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void loadParticle(const LHCb::Particle& par);

  std::string m_particlesName;
  Hlt::SelectionContainer1<LHCb::Track> m_selections;

};

#endif // HLTMCCHECKER_H
