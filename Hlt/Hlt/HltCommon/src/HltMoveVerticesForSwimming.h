// $Id: HltMoveVerticesForSwimming.h,v 1.3 2008-07-30 13:37:32 graven Exp $
#ifndef HLTMOVEVERTICESFORSWIMMING_H 
#define HLTMOVEVERTICESFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
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

private:

  double get_B_lifetime();
  StatusCode move_PVs();

  Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::RecVertex> m_selections;

  //The location of the B
  std::string m_particlesName;


  Gaudi::XYZVector m_bDirection; //The direction in which to swim
  Gaudi::XYZPoint m_bVertexPosition; //The starting point for the swimming
  double m_swimmingDistance; //How far to move the primary vertex. The sign is positive if moving in the direction of the B momentum
  double m_bLifetime; //the measured B lifetime
  double m_bMass; //the measured B mass, used to convert into proper time
  double m_bMom; //the measured B momentum, used to convert into proper time 

};

#endif 
