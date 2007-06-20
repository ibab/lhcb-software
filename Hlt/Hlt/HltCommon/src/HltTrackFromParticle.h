// $Id: HltTrackFromParticle.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTTRACKFROMPARTICLE_H 
#define HLTTRACKFROMPARTICLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void loadParticle(const LHCb::Particle& par);

  std::string m_particlesName;

};

#endif // HLTMCCHECKER_H
