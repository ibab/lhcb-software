// $Id: HltMoveVerticesForSwimming.h,v 1.1 2008-02-29 19:37:42 gligorov Exp $
#ifndef HLTMOVEVERTICESFORSWIMMING_H 
#define HLTMOVEVERTICESFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"

/** @class HltMoveVerticesForSwimming HltMoveVerticesForSwimming.h
 *  
 *
 *  @author v. Gligorov
 *  @date   2007-02-12
 */

class HltMoveVerticesForSwimming : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMoveVerticesForSwimming( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMoveVerticesForSwimming(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  double get_B_lifetime();
  StatusCode move_PVs();

  //The location of the B
  std::string m_particlesName;

  //Some iterators for the above
  std::vector<LHCb::RecVertex*>::const_iterator iV;
  LHCb::Particles::const_iterator iP;

  Gaudi::XYZVector m_bDirection; //The direction in which to swim
  Gaudi::XYZPoint m_bVertexPosition; //The starting point for the swimming
  double m_swimmingDistance; //How far to move the primary vertex. The sign is positive if moving in the direction of the B momentum
  double m_bLifetime; //the measured B lifetime

};

#endif 
