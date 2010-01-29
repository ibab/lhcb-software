// $Id: GeomDispCalculator.h,v 1.18 2010-01-29 11:44:53 jpalac Exp $
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

  StatusCode calcImpactPar( const LHCb::Particle&,
                            const LHCb::VertexBase&, 
                            double& , 
                            double& ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle&,
                            const LHCb::VertexBase&, 
                            double& , 
                            double& ,
                            Gaudi::XYZVector& , 
                            Gaudi::SymMatrix9x9&  ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle&,
                            const LHCb::VertexBase&, 
                            double& , 
                            double& ,
                            Gaudi::XYZVector& , 
                            Gaudi::XYZVector&  ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle&,
                            const Gaudi::XYZPoint& , 
                            double&  , 
                            double&  ) const { return printFailure() ; };

  StatusCode calcImpactPar( const LHCb::Particle&,
                            const Gaudi::XYZPoint& , 
                            double&  , 
                            double& ,
                            Gaudi::XYZVector& , 
                            Gaudi::SymMatrix9x9&  ) const { return printFailure() ; };

  StatusCode calcImpactPar(const LHCb::Particle& ,
                           const Gaudi::XYZPoint&,
                           const Gaudi::SymMatrix3x3& ,
                           double& , 
                           double& ,
                           Gaudi::XYZVector& , 
                           Gaudi::SymMatrix9x9& ) const { return printFailure() ; };
  
  StatusCode calcCloseAppr( const LHCb::Particle& ,       
                            const LHCb::Particle& , 
                            double& , 
                            double& ) const { return printFailure() ; };

  StatusCode calcVertexDis( const LHCb::VertexBase& ,
                            const LHCb::VertexBase& , 
                            double& , 
                            double& ) const { return printFailure() ; };

  StatusCode calcSignedFlightDistance( const LHCb::VertexBase&,
                                       const LHCb::Particle&, 
                                       double&, 
                                       double& ) const { return printFailure() ; };
  
  StatusCode calcProjectedFlightDistance( const LHCb::VertexBase&,
                                          const LHCb::Particle&, 
                                          double& , 
                                          double& ) const { return printFailure() ; };
  
private:

  StatusCode  printFailure() const { 
    fatal() << "The GeomDispCalculator is obsolete. Please change your code to use IDistanceCalculator.h" 
            << endmsg ;
    return StatusCode::FAILURE ;
  }  
  

private:

};

#endif // GEOMDISPCALCULATOR_H
