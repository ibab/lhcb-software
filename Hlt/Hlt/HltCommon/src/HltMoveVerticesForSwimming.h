// $Id: HltMoveVerticesForSwimming.h,v 1.6 2010-03-25 02:44:26 gligorov Exp $
#ifndef HLTMOVEVERTICESFORSWIMMING_H 
#define HLTMOVEVERTICESFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"
#include <Kernel/ILifetimeFitter.h>
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

  double move_PVs();

  Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::RecVertex> m_selections;

  Gaudi::XYZVector m_bDirection; //The direction in which to swim
  Gaudi::XYZPoint m_bVertexPosition; //The starting point for the swimming
  double m_swimmingDistance; //How far to move the primary vertex. The sign is positive if moving in the direction of the B momentum
  double m_bLifetime; //the measured B lifetime
  double m_bMom_X; //the measured B momentum, used to convert into proper time 
  double m_bMom_Y;
  double m_bMom_Z;
  double m_bVert_X;
  double m_bVert_Y;
  double m_bVert_Z;
  double m_bMass;
  double m_bE; 
  int 	 m_bPID;
  std::vector<double>  m_bCovMatrix;

  ILifetimeFitter* m_fit;

  std::string m_toolName;

};

#endif 
