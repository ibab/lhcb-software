// $Id: RichRayTraceCherenkovCone.h,v 1.1 2004-05-31 21:30:50 jonrob Exp $
#ifndef RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H
#define RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H 1

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichDetTools/IRichRayTracing.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecSegment.h"

/** @class RichRayTraceCherenkovCone RichRayTraceCherenkovCone.h
 *
 *  Tool to ray trace cherenkov photons to the detectors
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRayTraceCherenkovCone : public RichRecToolBase,
                                  virtual public IRichRayTraceCherenkovCone {

public:

  /// Standard constructor
  RichRayTraceCherenkovCone( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

  /// Destructor
  virtual ~RichRayTraceCherenkovCone(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Ray trace the Cherenkov cone for the given segment and mass hypothesis to the detector plane
  StatusCode rayTrace ( RichRecSegment * segment,
                        const Rich::ParticleIDType id,
                        std::vector<HepPoint3D> & points ) const;

  /// Ray trace the Cherenkov cone for the given segment and cherenkov angle to the detector plane
  StatusCode rayTrace ( RichRecSegment * segment,
                        const double ckTheta,
                        std::vector<HepPoint3D> & points ) const;

  /// Ray trace the Cherenkov cone for the given ring to the detector plane
  std::vector<HepPoint3D> & rayTrace ( RichRecRing * ring ) const;

private: // methods


private: // data

  // Pointers to tool instances
  IRichRayTracing * m_rayTrace;
  IRichCherenkovAngle * m_ckAngle;

  // Number of photons to use in ray tracing
  int m_nRayTrace;

  // Cherenkov phi incrementation
  double m_incPhi;

  typedef std::vector<double> AngleVector;
  // Cache Sin(angle) for geometrical efficiency calculation
  AngleVector m_sinCkPhi;
  // Cache Cos(angle) for geometrical efficiency calculation
  AngleVector m_cosCkPhi;

};

#endif // RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H
