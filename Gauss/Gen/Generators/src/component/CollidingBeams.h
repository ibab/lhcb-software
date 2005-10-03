// $Id: CollidingBeams.h,v 1.1 2005-10-03 10:19:32 robbep Exp $
#ifndef GENERATORS_COLLIDINGBEAMS_H 
#define GENERATORS_COLLIDINGBEAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IBeamTool.h"

class IRndmGenSvc ;

/** @class CollidingBeams CollidingBeams.h 
 *  
 *  Utility tool to compute beam values
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class CollidingBeams : public GaudiTool, virtual public IBeamTool {
public:
  /// Standard constructor
  CollidingBeams( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~CollidingBeams( ); ///< Destructor
  
  /// Initialize method
  virtual StatusCode initialize( ) ;  

  virtual void getMeanBeams( Hep3Vector & pBeam1, Hep3Vector & pBeam2 ) const ;
  
  virtual void getBeams( Hep3Vector & pBeam1 , Hep3Vector & pBeam2 ) ;

protected:

private:
  double m_angleSmear ;

  double m_beamMomentum ;  ///< Incident proton energy (GeV)
  double m_verticalXAngle ;
  double m_horizontalXAngle ;
  double m_emittance ;
  double m_betaStar ;

  Rndm::Numbers m_gaussianDist ;
};
#endif // GENERATORS_COLLIDINGBEAMS_H
