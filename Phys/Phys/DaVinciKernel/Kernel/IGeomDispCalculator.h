// $Id: IGeomDispCalculator.h,v 1.1 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IGEOMDISPCALCULATOR_H 
#define DAVINCIKERNEL_IGEOMDISPCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;

static const InterfaceID IID_IGeomDispCalculator("IGeomDispCalculator", 2 , 1); 

/** @class IGeomDispCalculator IGeomDispCalculator.h Kernel/IGeomDispCalculator.h
 *  Interface for Geometrical Calculation Tool. 
 *
 *  @author Miriam Gandelman
 *  @date   14/03/2002
 */

class IGeomDispCalculator : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGeomDispCalculator; }

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a vertex.
  virtual StatusCode calcImpactPar( const Particle& ,
                                    const Vertex&, double&, double& ) = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  virtual StatusCode calcImpactPar( const Particle& ,
                                    const Vertex&, double&, double&, 
                                    Hep3Vector&, HepSymMatrix& ) = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  virtual StatusCode calcImpactPar( const Particle& ,
                                    const Vertex&, double&, double&, 
                                    Hep3Vector&, Hep3Vector& ) = 0; 

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  virtual StatusCode calcImpactPar( const Particle& ,
                                    HepPoint3D&, double&, double& ) = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  virtual StatusCode calcImpactPar( const Particle& ,
                                    HepPoint3D&, double&, double&,
                                    Hep3Vector&, HepSymMatrix& ) = 0; 

  /// Calculates the distance of closest approach between two particles
  /// and its error.
  virtual StatusCode calcCloseAppr( const Particle& ,
                                    const Particle&, double&, double& ) = 0; 

  /// Calculates the distance between two vertices and its error.
  virtual StatusCode calcVertexDis( const Vertex& ,
                                    const Vertex&, double&, double& ) = 0; 

};

#endif // DAVINCIKERNEL_IGEOMDISPCALCULATOR_H
