// $Id: FixedTarget.h,v 1.1 2005-10-03 10:20:12 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

class IRndmGenSvc ;

/** @class FixedTarget FixedTarget.h 
 *  
 *  Utility tool to compute beam values
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

  virtual void getMeanBeams( Hep3Vector & pBeam1, Hep3Vector & pBeam2 ) const ;
  
  virtual void getBeams( Hep3Vector & pBeam1 , Hep3Vector & pBeam2 ) ;

protected:
private:
  double m_angleSmear ;

  double m_beamMomentum ;
  double m_verticalXAngle ;
  double m_horizontalXAngle ;
  double m_emittance ;
  double m_betaStar ;

  Rndm::Numbers m_gaussianDist ;
};
#endif // GENERATORS_FIXEDTARGET_H
