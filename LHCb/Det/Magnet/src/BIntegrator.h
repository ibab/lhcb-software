// $Id: BIntegrator.h,v 1.1 2005-09-07 12:20:25 cattanem Exp $
#ifndef MAGNET_BINTEGRATOR_H 
#define MAGNET_BINTEGRATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// from LHCbKernel
#include "Kernel/IBIntegrator.h"            // Interface

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
 StatusCode calculateBdlAndCenter( const HepPoint3D& beginPoint,
                                   const HepPoint3D& endPoint,
                                   const double tX,
                                   const double tY,
                                   double& zCenter,
                                   HepVector3D& Bdl ) const;

protected:

private:
  StatusCode calculateBdlCenter();

  IMagneticFieldSvc* m_pIMF;   // Pointer to the magnetic field service

  int        m_nSteps;
  HepPoint3D m_centerZ;
  double     m_firstZ;
  double     m_lastZ;

};
#endif // MAGNET_BINTEGRATOR_H
