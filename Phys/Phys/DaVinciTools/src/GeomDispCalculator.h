// $Id: GeomDispCalculator.h,v 1.14 2008-07-10 15:03:24 pkoppenb Exp $
#ifndef GEOMDISPCALCULATOR_H
#define GEOMDISPCALCULATOR_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "LHCbMath/LineTypes.h"
// from DaVinciKernel
#include "Kernel/IGeomDispCalculator.h"

// Forward declarations
class IParticleTransporter;

/** @class GeomDispCalculator GeomDispCalculator.h 
 *  Geometrical displacements calculator. 
 *
 * This tool is now obsolete. Please use IDistanceCalculator
 *
 * \sa IDistanceCalculator
 *
 * @author Miriam Gandelman
 * @date 20/03/2002 
 */

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
                            double& ip , 
                            double& impactParameterError ) const;

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const;

  StatusCode calcImpactPar(const LHCb::Particle& transParticle,
                           const Gaudi::XYZPoint& pos,
                           const Gaudi::SymMatrix3x3& posError,
                           double& impactParameter, 
                           double& impactParameterError,
                           Gaudi::XYZVector& ipVector, 
                           Gaudi::SymMatrix9x9& errMatrix ) const;
  
  StatusCode calcCloseAppr( const LHCb::Particle& particle0,       
                            const LHCb::Particle& particle1, 
                            double& distance, 
                            double& distanceError ) const;

  StatusCode calcVertexDis( const LHCb::VertexBase& vertex0,
                            const LHCb::VertexBase& vertex1, 
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

  double calcErrComponent(const Gaudi::XYZVector& vProj,
                          const LHCb::Particle& particle,
                          const LHCb::VertexBase& vertex,
                          const Gaudi::SymMatrix9x9 errMtx) const;
  
  Gaudi::Vector9 totalDeriv(const LHCb::Particle& particle,
                            const Gaudi::XYZVector& displ) const;

  void calcErrorMatrix(const LHCb::Particle& particle,
                       const Gaudi::SymMatrix3x3& vtxErr, 
                       Gaudi::SymMatrix9x9& errMatrix) const;


  void calcDerivVectors(const Gaudi::Math::XYZLine& part0,
                        const Gaudi::Math::XYZLine& part1,
                        Gaudi::XYZVector& u,
                        Gaudi::XYZVector& u1,
                        Gaudi::XYZVector& u2) const;
  

private:
  IParticleTransporter* m_pTransporter;  ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

};

#endif // GEOMDISPCALCULATOR_H
