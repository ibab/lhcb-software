// $Id: IGeomDispCalculator.h,v 1.6 2007-03-02 10:59:57 cattanem Exp $
#ifndef DAVINCIKERNEL_IGEOMDISPCALCULATOR_H 
#define DAVINCIKERNEL_IGEOMDISPCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// Forward declarations
class IParticlePropertySvc;

namespace LHCb {
  class Particle;
  class VertexBase;
}


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
  virtual StatusCode calcImpactPar( const LHCb::Particle& ,
                                    const LHCb::VertexBase&, 
                                    double&, double& ) const = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  virtual StatusCode calcImpactPar( const LHCb::Particle& ,
                                    const LHCb::VertexBase&, 
                                    double&, double&, 
                                    Gaudi::XYZVector&, 
                                    Gaudi::SymMatrix9x9& ) const = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  virtual StatusCode calcImpactPar( const LHCb::Particle& ,
                                    const LHCb::VertexBase&, 
                                    double&, double&, 
                                    Gaudi::XYZVector&, 
                                    Gaudi::XYZVector& ) const = 0; 

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  virtual StatusCode calcImpactPar( const LHCb::Particle& ,
                                    const Gaudi::XYZPoint&, 
                                    double&, double& ) const = 0; 

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  virtual StatusCode calcImpactPar( const LHCb::Particle& ,
                                    const Gaudi::XYZPoint&, double&, double&,
                                    Gaudi::XYZVector&, 
                                    Gaudi::SymMatrix9x9& ) const = 0; 

  /// Calculates the distance of closest approach between two particles
  /// and its error.
  virtual StatusCode calcCloseAppr( const LHCb::Particle& ,
                                    const LHCb::Particle&, 
                                    double&, double& ) const = 0; 

  /// Calculates the distance between two vertices and its error.
  virtual StatusCode calcVertexDis( const LHCb::VertexBase& ,
                                    const LHCb::VertexBase&, 
                                    double&, double& ) const = 0; 

};

#endif // DAVINCIKERNEL_IGEOMDISPCALCULATOR_H
