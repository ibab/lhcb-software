// $Id: IGeomDispCalculator.h,v 1.9 2008-01-21 12:36:24 pkoppenb Exp $
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


static const InterfaceID IID_IGeomDispCalculator("IGeomDispCalculator", 2 , 2); 

/** @class IGeomDispCalculator IGeomDispCalculator.h Kernel/IGeomDispCalculator.h
 *  Interface for Geometrical Calculation Tool. 
 *
 *  @author Miriam Gandelman
 *  @author Juan Palacios juancho@nikhef.nl
 *  @date   14/03/2002
 */

class IGeomDispCalculator : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGeomDispCalculator; }

  /**
   * Calculate the Impact Parameter, and associated error,
   * between a vertex and a particle.
   * @param particle
   * @param vertex
   * @param impactParameter
   * @param impactParameterError
   * @return StatusCode to report errors in the calculation
   *
  */
  virtual StatusCode calcImpactPar( const LHCb::Particle& particle,
                                    const LHCb::VertexBase& vertex, 
                                    double& impactParameter, 
                                    double& impactParameterError) const = 0; 

  /**
   * Calculate the Impact Parameter, and associated error,
   * between a vertex and a particle.
   * @param particle
   * @param vertex
   * @param impactParameter
   * @param impactParameterError
   * @param ipVector
   * @param errMatrix
   * @return StatusCode to report errors in the calculation
   *
  */
  virtual StatusCode calcImpactPar( const LHCb::Particle& particle,
                                    const LHCb::VertexBase& vertex, 
                                    double& impactParameter, 
                                    double& impactParameterError, 
                                    Gaudi::XYZVector& ipVector, 
                                    Gaudi::SymMatrix9x9& errMatrix) const = 0; 

  /**
   * Calculate the Impact Parameter, and associated error,
   * between a vertex and a particle.
   * @param particle
   * @param vertex
   * @param impactParameter
   * @param impactParameterError
   * @param ipVector
   * @param errVector
   * @return StatusCode to report errors in the calculation
   *
  */
  virtual StatusCode calcImpactPar( const LHCb::Particle& particle,
                                    const LHCb::VertexBase& vertex, 
                                    double& impactParameter, 
                                    double& impactParameterError, 
                                    Gaudi::XYZVector& ipVector, 
                                    Gaudi::XYZVector& errVector) const = 0; 

  /**
   * Calculate the Impact Parameter, and associated error,
   * between a 3D point and a particle.
   * @param particle
   * @param point
   * @param impactParameter
   * @param impactParameterError
   * @return StatusCode to report errors in the calculation
   *
  */
  virtual StatusCode calcImpactPar( const LHCb::Particle& particle,
                                    const Gaudi::XYZPoint& point, 
                                    double& impactParameter, 
                                    double& impactParameterError) const = 0; 

  /**
   * Calculate the Impact Parameter, and associated error,
   * between a 3D point and a particle.
   * @param particle
   * @param point
   * @param impactParameter
   * @param impactParameterError
   * @param ipVector
   * @param errMatrix
   * @return StatusCode to report errors in the calculation
   *
  */
  virtual StatusCode calcImpactPar( const LHCb::Particle& particle,
                                    const Gaudi::XYZPoint& point, 
                                    double& impactParameter, 
                                    double& impactParameterError,
                                    Gaudi::XYZVector& ipVector, 
                                    Gaudi::SymMatrix9x9& errMatrix) const = 0; 

  /**
   * Calculates the absolute distance of closest approach, 
   * and corresponding error, between two particles. 
   * The order of the particles is irrelevant.
   *
   * @param particle0 The first particle
   * @param particle1 The second particle
   * @param distance: set to the calculated distance
   * @param distanceError: set to the calculated error on the distance
   * @return StatusCode to report errors in the calculation
   */
  virtual StatusCode calcCloseAppr( const LHCb::Particle& particle0,
                                    const LHCb::Particle& particle1, 
                                    double& distance, 
                                    double& distanceError) const = 0; 

  /**
   * Calculates the absolute distance and corresponding error 
   * between two vertices. The order of the vertices is irrelevant.
   *
   * @param vertex0 The first vertex
   * @param vertex1 The second vertex
   * @param distance: set to the calculated distance
   * @param distanceError: set to the calculated error on the distance
   * @return StatusCode to report errors in the calculation
   */
  virtual StatusCode calcVertexDis( const LHCb::VertexBase& vertex0,
                                    const LHCb::VertexBase& vertex1, 
                                    double& distance, 
                                    double& distanceError) const = 0; 

  /**
   * Calculates the signed distance and corresponding error 
   * between a particle's decay vertex and another vertex. 
   * The sign is according to the sign of the particle's 
   * Z momentum in the LHCb frame.
   *
   * @param vertex The vertex
   * @param particle the particle
   * @param distance: set to the calculated distance
   * @param distanceError: set to the calculated error on the distance
   * @return StatusCode to report errors in the calculation
   */
  virtual StatusCode calcSignedFlightDistance( const LHCb::VertexBase& vertex,
                                               const LHCb::Particle& particle, 
                                               double& distance, 
                                               double& distanceError) const = 0; 

  /**
   * Calculates the projected distance and corresponding error 
   * between a particle's decay vertex and another vertex. 
   * Definition: l = r dot phat
   * where l=projected distance 
   *       r=vector from vertex to particle end-vertex 
   *       phat = p/|p| = flight direction of the particle
   *
   * This variable has a sign. 
   * A positive sign corresponds with a particle vertex displaced in the flight direction (signal-like)
   * A negative sign corresponds with a particle vertex opposite to the flight direction (background-like)
   * @param vertex The vertex
   * @param particle the particle
   * @param distance: set to the calculated distance
   * @param distanceError: set to the calculated error on the distance
   * @return StatusCode to report errors in the calculation
   */
  virtual StatusCode calcProjectedFlightDistance( const LHCb::VertexBase& vertex,
                                                  const LHCb::Particle& particle, 
                                                  double& distance, 
                                                  double& distanceError) const = 0; 

};

#endif // DAVINCIKERNEL_IGEOMDISPCALCULATOR_H
