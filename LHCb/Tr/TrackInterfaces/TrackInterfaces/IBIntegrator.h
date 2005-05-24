#ifndef TRACKINTERFACES_IBINTEGRATOR_H 
#define TRACKINTERFACES_IBINTEGRATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

static const InterfaceID IID_IBIntegrator ( "IBIntegrator", 1, 0 );

/** @class IBIntegrator IBIntegrator.h TrackInterfaces/IBIntegrator.h
 *  
 *
 *  @author Rutger Hierck
 *  @date   2002-05-28
 */
class IBIntegrator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBIntegrator; }

  /// Get the z of center and the total Bdl
  virtual StatusCode calculateBdlAndCenter( const HepPoint3D& beginPoint,
                                            const HepPoint3D& endPoint,
                                            const double tX,
                                            const double tY,
                                            double& zCenter,
                                            HepVector3D& Bdl ) const = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IBINTEGRATOR_H
