// $Id: HltSelectTracksForSwimming.h,v 1.2 2008-07-30 13:37:32 graven Exp $
#ifndef HLTSELECTTRACKSFORSWIMMING_H 
#define HLTSELECTTRACKSFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
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


protected:

  StatusCode filter_Tracks();
  bool tracksMatchInVelo(LHCb::Track*,LHCb::Track*);

  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::Track> m_selections;
  //The location of the B
  std::string m_particlesName;


  Gaudi::XYZVector m_bDirection; //The direction in which to swim
  Gaudi::XYZPoint m_bVertexPosition; //The starting point for the swimming
  double m_swimmingDistance; //How far to move the primary vertex. The sign is determined as positive if moving in the direction of the B momentum

};

#endif // HLTSELECTTRACKSFORSWIMMING_H
