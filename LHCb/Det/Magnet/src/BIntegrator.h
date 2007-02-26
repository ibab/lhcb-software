// $Id: BIntegrator.h,v 1.3 2007-02-26 14:50:47 cattanem Exp $
#ifndef MAGNET_BINTEGRATOR_H 
#define MAGNET_BINTEGRATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/IBIntegrator.h"            // Interface

// Math Definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Forward declarations
class IMagneticFieldSvc;

/** @class BIntegrator BIntegrator.h Magnet/BIntegrator.h
 *
 *  @author Rutger Hierck
 *  @date   2002-05-28
 */
class BIntegrator : public GaudiTool,
                    virtual public IBIntegrator {
public: 
  /// Standard constructor
  BIntegrator( const std::string& type, 
               const std::string& name,
               const IInterface* parent );

  virtual ~BIntegrator( ); ///< Destructor

  StatusCode initialize(); ///< Initialization

 /// Get the z of center and the total Bdl
 StatusCode calculateBdlAndCenter( const Gaudi::XYZPoint& beginPoint,
                                   const Gaudi::XYZPoint& endPoint,
                                   const double tX,
                                   const double tY,
                                   double& zCenter,
                                   Gaudi::XYZVector& Bdl ) const;

protected:

private:
  StatusCode calculateBdlCenter();

  IMagneticFieldSvc* m_pIMF;   // Pointer to the magnetic field service

  int             m_nSteps;
  Gaudi::XYZPoint m_centerZ;
  double          m_firstZ;
  double          m_lastZ;

};
#endif // MAGNET_BINTEGRATOR_H
