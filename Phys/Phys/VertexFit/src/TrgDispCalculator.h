// $Id: TrgDispCalculator.h,v 1.1 2005-02-08 08:50:22 pkoppenb Exp $
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
  StatusCode calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr );

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const Particle& particle,
                            const Vertex& vertex, double& ip, double& ipErr,
                            Hep3Vector& ipVector, Hep3Vector& errVector );

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr );

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  StatusCode calcImpactPar( const Particle& particle,
                            HepPoint3D& point, double& ip , double& ipErr,
                            Hep3Vector& ipVector, HepSymMatrix& errMatrix );
  /// Calculates the distance of closest approach between two particles
  /// and its error.
  StatusCode calcCloseAppr( const Particle& particle1,
       
                     const Particle& particle2, double& dist, 
                            double& distErr );

  /// Calculates the distance between two vertices and its error.
  StatusCode calcVertexDis( const Vertex& vertex1,
                            const Vertex& vertex2, double& dist, 
                            double& distErr );

private:
  /// Calculates the Impact Parameter
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const Particle& particle, HepPoint3D& point);

};

#endif // TRGDISPCALCULATOR_H
