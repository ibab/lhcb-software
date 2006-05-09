// $Id: GeomDispCalculator.h,v 1.7 2006-05-09 16:32:19 pkoppenb Exp $
#ifndef GEOMDISPCALCULATOR_H
#define GEOMDISPCALCULATOR_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "Kernel/GenericVectorTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Line.h"
// from DaVinciKernel
#include "Kernel/IGeomDispCalculator.h"

// Forward declarations
class IParticleTransporter;

/** @class GeomDispCalculator GeomDispCalculator.h 
 *  Geometrical displacements calculator. 
 *
 * @author Miriam Gandelman
 * @date 20/03/2002 
*/

/// @todo move Line typedef to LHCbDefinitions once GeomFun implementation
///       stops depending on Kernel/Vector3DTypes.h
namespace Gaudi
{
  typedef Gaudi::Line<Gaudi::XYZPoint, Gaudi::XYZVector> XYZLine;
}


class GeomDispCalculator : public GaudiTool,
                   virtual public IGeomDispCalculator {

public:


  /// Standard Constructor
  GeomDispCalculator( const std::string& type, 
                      const std::string& name, 
                      const IInterface* parent);

  /// Standard Destructor
  virtual ~GeomDispCalculator() { }

  /// Retrieve the necessary tools
  StatusCode initialize();    

  /// Calculates the Impact Parameter and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, 
                            double& ipErr ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, 
                            double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector and its error. 
  /// Inputs: a particle and a vertex.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::Vertex& vertex, 
                            double& ip, 
                            double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::XYZVector& errVector ) const;

  /// Calculates the Impact Parameter and its error.
  /// Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , 
                            double& ipErr ) const;

  /// Calculates the Impact Parameter and its error and outputs also
  /// the impact parameter vector. Inputs: a particle and a space point.
  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , 
                            double& ipErr,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  StatusCode calcImpactPar(const LHCb::Particle& transParticle,
                           const Gaudi::XYZPoint& pos,
                           const Gaudi::SymMatrix3x3& posError,
                           double& ip, 
                           double& ipErr,
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
                            const LHCb::Vertex& vertex2, double& dist, 
                            double& distErr ) const;

private:

  double calcErrComponent(const Gaudi::XYZVector& vProj,
                          const LHCb::Particle& particle,
                          const LHCb::Vertex& vertex,
                          const Gaudi::SymMatrix9x9 errMtx) const;
  
  Gaudi::Vector9 totalDeriv(const LHCb::Particle& particle,
                            const Gaudi::XYZVector& displ) const;

  void calcErrorMatrix(const LHCb::Particle& particle,
                       const Gaudi::SymMatrix3x3& vtxErr, 
                       Gaudi::SymMatrix9x9& errMatrix) const;


  void calcDerivVectors(const Gaudi::XYZLine& part0,
                        const Gaudi::XYZLine& part1,
                        Gaudi::XYZVector& u,
                        Gaudi::XYZVector& u1,
                        Gaudi::XYZVector& u2) const;
  

private:
  IParticleTransporter* m_pTransporter;  ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

};

#endif // GEOMDISPCALCULATOR_H
