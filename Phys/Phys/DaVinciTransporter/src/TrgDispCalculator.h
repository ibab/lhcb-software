// $Id: TrgDispCalculator.h,v 1.4 2006-05-08 17:57:10 jpalac Exp $
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

  /// Calculates the Impact Parameter and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, double& ipErr ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::XYZVector& errVector ) const;

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , double& ipErr ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;
  /// Calculates the distance of closest approach between two particles
  /// and its error.
  StatusCode calcCloseAppr( const LHCb::Particle& particle1,
                            const LHCb::Particle& particle2, 
                            double& dist, 
                            double& distErr ) const;

  /// Calculates the distance between two vertices and its error.
  StatusCode calcVertexDis( const LHCb::Vertex& vertex1,
                            const LHCb::Vertex& vertex2, 
                            double& dist, 
                            double& distErr ) const;

private:
  /// Calculates the Impact Parameter
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle, 
                            Gaudi::XYZPoint& point) const;

};

#endif // TRGDISPCALCULATOR_H
