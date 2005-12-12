// $Id: FixedTarget.h,v 1.2 2005-12-12 16:06:20 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;

/** @class FixedTarget FixedTarget.h "FixedTarget.h"
 *  
 *  Tool to compute beam values with only one beam colliding to a fixed
 *  target. Concrete implementation of a IBeamTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class FixedTarget : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  FixedTarget( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~FixedTarget( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /// Implements IBeamTool::getMeanBeams. See CollidingBeams::getMeanBeams
  virtual void getMeanBeams( Hep3Vector & pBeam1, Hep3Vector & pBeam2 ) const ;
  
  /// Implements IBeamTool::getBeams. See CollidingBeams::getBeams
  virtual void getBeams( Hep3Vector & pBeam1 , Hep3Vector & pBeam2 ) ;
  
 private:
  /// Value of angular smearing (computed from quantities given in options)
  double m_angleSmear ;

  double m_beamMomentum ; ///< Incident proton energy (set by job options)

  /// Vertical crossing angle (set by job options)
  double m_verticalXAngle ; 

  /// Horizontal crossing angle (set by job options)
  double m_horizontalXAngle ;

  double m_emittance ; ///< Emmittance (set by job options)

  double m_betaStar ; ///< Beta star (set by job options)

  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
};
#endif // GENERATORS_FIXEDTARGET_H
