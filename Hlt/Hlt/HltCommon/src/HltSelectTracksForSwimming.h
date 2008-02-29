// $Id: HltSelectTracksForSwimming.h,v 1.1 2008-02-29 19:38:12 gligorov Exp $
#ifndef HLTSELECTTRACKSFORSWIMMING_H 
#define HLTSELECTTRACKSFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"

/** @class HltSelectTracksForSwimming HltSelectTracksForSwimming.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2008-02-12
 */

class HltSelectTracksForSwimming : public HltAlgorithm {
public: 
  /// Standard constructor
  HltSelectTracksForSwimming( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectTracksForSwimming(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  template <class CONT>
        void copy(const CONT& c1, CONT& c2)
                {std::copy(c1.begin(),c1.end(),std::back_inserter(c2));}

protected:

  StatusCode filter_Tracks();
  bool tracksMatchInVelo(LHCb::Track*,LHCb::Track*);

  //The location of the B
  std::string m_particlesName;

  //Some iterators for the above
  std::vector<LHCb::Track*>::const_iterator iT;
  std::vector<LHCb::Track*>::const_iterator iTT;
  LHCb::Particles::const_iterator iP;

  Gaudi::XYZVector m_bDirection; //The direction in which to swim
  Gaudi::XYZPoint m_bVertexPosition; //The starting point for the swimming
  double m_swimmingDistance; //How far to move the primary vertex. The sign is determined as positive if moving in the direction of the B momentum

};

#endif // HLTSELECTTRACKSFORSWIMMING_H
