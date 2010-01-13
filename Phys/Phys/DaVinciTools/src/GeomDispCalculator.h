// $Id: GeomDispCalculator.h,v 1.15 2010-01-13 16:02:17 pkoppenb Exp $
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
                            double& impactParameterError ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::VertexBase& vertex, 
                            double& impactParameter, 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const LHCb::VertexBase& vertex, 
                            double& impactParameter, 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::XYZVector& errVector ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , 
                            double& impactParameterError ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle& particle,
                            const Gaudi::XYZPoint& point, 
                            double& ip , 
                            double& impactParameterError,
                            Gaudi::XYZVector& ipVector, 
                            Gaudi::SymMatrix9x9& errMatrix ) const { return printFailure() ; };

  StatusCode calcImpactPar(const LHCb::Particle& transParticle,
                           const Gaudi::XYZPoint& pos,
                           const Gaudi::SymMatrix3x3& posError,
                           double& impactParameter, 
                           double& impactParameterError,
                           Gaudi::XYZVector& ipVector, 
                           Gaudi::SymMatrix9x9& errMatrix ) const { return printFailure() ; };
  
  StatusCode calcCloseAppr( const LHCb::Particle& particle0,       
                            const LHCb::Particle& particle1, 
                            double& distance, 
                            double& distanceError ) const { return printFailure() ; };

  StatusCode calcVertexDis( const LHCb::VertexBase& vertex0,
                            const LHCb::VertexBase& vertex1, 
                            double& distance, 
                            double& distanceError ) const { return printFailure() ; };

  StatusCode calcSignedFlightDistance( const LHCb::VertexBase& vertex,
                                       const LHCb::Particle& particle, 
                                       double& distance, 
                                       double& distanceError) const { return printFailure() ; };
  
  StatusCode calcProjectedFlightDistance( const LHCb::VertexBase& vertex,
                                          const LHCb::Particle& particle, 
                                          double& distance, 
                                          double& distanceError) const { return printFailure() ; };
  
private:

  StatusCode  printFailure(){ 
    fatal() << "The GeomDispCalculator is obsolete. Please change your code to use IDistanceCalculator.h" 
            << endmsg ;
    return StatusCode::FAILURE ;
  }  
  

private:

};

#endif // GEOMDISPCALCULATOR_H
