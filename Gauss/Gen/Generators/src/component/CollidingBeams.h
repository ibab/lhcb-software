// $Id: CollidingBeams.h,v 1.3 2005-12-31 17:32:01 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

// Forward declarations
class IRndmGenSvc ;

/** @class CollidingBeams CollidingBeams.h "CollidingBeams.h"
 *  
 *  Tool to compute colliding beams values. Concrete implementation
 *  of a beam tool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class CollidingBeams : public GaudiTool, virtual public IBeamTool {
 public:
  /// Standard constructor
  CollidingBeams( const std::string& type, const std::string& name,
                  const IInterface* parent ) ;
  
  virtual ~CollidingBeams( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  
  
  /** Implements IBeamTool::getMeanBeams
   */
  virtual void getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                             Gaudi::XYZVector & pBeam2 ) const ;
  
  /** Implements IBeamTool::getBeams
   *  Compute beam 3-momentum taking into account the horizontal and vertical
   *  beam angles (given by job options). These angles are Gaussian-smeared
   *  with an angular smearing equal to (emittance/beta*)^1/2.
   */
  virtual void getBeams( Gaudi::XYZVector & pBeam1 , 
                         Gaudi::XYZVector & pBeam2 ) ;

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

  double m_hBeamLineAngle ; ///< Beam Line horizontal angle in LHCb coord system 
  double m_vBeamLineAngle ; ///< Beam Line vertical angle in LHCb coord system

  Rndm::Numbers m_gaussianDist ; ///< Gaussian random number generator
};
#endif // GENERATORS_COLLIDINGBEAMS_H
