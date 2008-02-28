// $Id: TrgDispCalculator.h,v 1.5 2008-02-28 13:52:51 pkoppenb Exp $
#ifndef TRGDISPCALCULATOR_H
#define TRGDISPCALCULATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IGeomDispCalculator.h"

/** @class TrgDispCalculator TrgDispCalculator.h 
 *
 *  Fast geometrical displacements calculator. 
 *  Assumes there is no B field and that the errors are cylindrical.
 *
 *  Description: Calculation of impact parameter, distance between 
 *  two vertices and distance of closest approach of two particles
 *
 *  THIS TOOL IS MEANT FOR HLT, WHERE TRACKS ARE ASSUMED TO HAVE A
 *  'CYLINDRICAL' ERROR, THAT IS, THE COVARIANCE MATRIX HAS
 *  COV(1,1)=COV(2,2)!=0 AND ZERO IN ANY OTHER ELEMENT
 *  Could give reasonable approximations in other situations?
 *
 *  The closest thing to a documentation available by now is a talk at
 *  http://agenda.cern.ch/fullAgenda.php?ida=a05940#2005-02-07
 *
 * @author Hugo Ruiz
 * @date 01/02/2005 
 */

class TrgDispCalculator : public GaudiTool,
                          virtual public IGeomDispCalculator {

public:

  /// Standard Constructor
  TrgDispCalculator( const std::string& type, const std::string& name, 
                     const IInterface* parent);

  /// Standard Destructor
  virtual ~TrgDispCalculator() { }

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::VertexBase& vertex, 
                            double& impactParameter, 
                            double& impactParameterError ) const;

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::VertexBase& vertex, 
                            double& impactParameter, 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::VertexBase& vertex, 
                            double& impactParameter, 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::XYZVector& errVector ) const;

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& impactParameter, 
                            double& impactParameterError ) const;

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& impactParameter, 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  StatusCode calcCloseAppr( const LHCb::Particle& particle1,
                            const LHCb::Particle& particle2, 
                            double& distance, 
                            double& distanceError ) const;

  StatusCode calcVertexDis( const LHCb::VertexBase& vertex1,
                            const LHCb::VertexBase& vertex2, 
                            double& distance, 
                            double& distanceError ) const;

  StatusCode calcSignedFlightDistance( const LHCb::VertexBase& vertex,
                                       const LHCb::Particle& particle, 
                                       double& distance, 
                                       double& distanceError) const;

  StatusCode calcProjectedFlightDistance( const LHCb::VertexBase& vertex,
                                          const LHCb::Particle& particle, 
                                          double& distance, 
                                          double& distanceError) const;

private:
  /// Calculates the Impact Parameter
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle, 
                            Gaudi::XYZPoint& point) const;

};

#endif // TRGDISPCALCULATOR_H
