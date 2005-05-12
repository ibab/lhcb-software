// $Id: BIntegrator.h,v 1.1 2005-05-12 15:02:14 erodrigu Exp $
#ifndef TRACKTOOLS_BINTEGRATOR_H 
#define TRACKTOOLS_BINTEGRATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// local
#include "TrackTools/IBIntegrator.h"            // Interface

/** @class BIntegrator BIntegrator.h TrackTools/BIntegrator.h
 *  
 *
 *  @author Rutger Hierck
 *  @date   2002-05-28
 */
class BIntegrator : public GaudiTool, virtual public IBIntegrator {
public: 
  /// Standard constructor
  BIntegrator( const std::string& type, 
               const std::string& name,
               const IInterface* parent );

  virtual ~BIntegrator( ); ///< Destructor

  StatusCode initialize(); ///< Initialization
  StatusCode finalize();   ///< Finalization

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
#endif // TRACKTOOLS_BINTEGRATOR_H
