// $Id: IGeomDispCalculator.h,v 1.1 2002-03-27 20:27:45 gcorti Exp $
#ifndef DAVINCITOOLS_IGEOMDISPCALCULATOR_H 
#define DAVINCITOOLS_IGEOMDISPCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;

static const InterfaceID IID_IGeomDispCalculator("IGeomDispCalculator", 1 , 0); 

/** @class IGeomDispCalculator IGeomDispCalculator.h DaVinciTools/IGeomDispCalcu *  lator.h
 *  Add four momentum interface.
 *
 *  @author Miriam Gandelman
 *  @date   14/03/2002
 */

class IGeomDispCalculator : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGeomDispCalculator; }
  /// Calculate
  virtual void calcImpactPar( const Particle& ,
                                  const Vertex&, double&, double& ) = 0; 
  virtual void calcImpactPar( const Particle& ,
                                  const Vertex&, double&, double&, 
                                  Hep3Vector&, HepSymMatrix& ) = 0; 
  virtual void calcImpactPar( const Particle& ,
                                  HepPoint3D&, double&, double& ) = 0; 
  virtual void calcImpactPar( const Particle& ,
                                  HepPoint3D&, double&, double&,
                                  Hep3Vector&, HepSymMatrix& ) = 0; 
  virtual void calcCloseAppr( const Particle& ,
                                  const Particle&, double&, double& ) = 0; 
  virtual void calcVertexDis( const Vertex& ,
                                  const Vertex&, double&, double& ) = 0; 
};

#endif // DAVINCITOOLS_IGEOMDISPCALCULATOR_H
