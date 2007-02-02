
//-----------------------------------------------------------------------------
/** @file RichRayTraceCherenkovCone.h
 *
 *  Header file for tool : Rich::Rec::RayTraceCherenkovCone
 *
 *  CVS Log :-
 *  $Id: RichRayTraceCherenkovCone.h,v 1.11 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H
#define RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecSegment.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class RayTraceCherenkovCone RichRayTraceCherenkovCone.h
     *
     *  Tool to ray trace cherenkov photons in a cone around a given
     *  RichRecSegment direction, at the given angle or the angle correct
     *  for a given mass hypothesis.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo Find a way to merge the functionality of the similar ray tracing methods
     */
    //-----------------------------------------------------------------------------

    class RayTraceCherenkovCone : public Rich::Rec::ToolBase,
                                  virtual public IRayTraceCherenkovCone
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      RayTraceCherenkovCone( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Destructor
      virtual ~RayTraceCherenkovCone(){}

      // Initialize method
      StatusCode initialize();


    public: // methods (and doxygen comments) inherited from public interface

      // Ray trace the Cherenkov cone for the given segment and mass hypothesis to the detector plane
      StatusCode rayTrace ( LHCb::RichRecSegment * segment,
                            const Rich::ParticleIDType id,
                            std::vector<Gaudi::XYZPoint> & points,
                            const unsigned int nPoints,
                            const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const;

      // Ray trace the Cherenkov cone for the given segment and cherenkov angle to the detector plane
      StatusCode rayTrace ( LHCb::RichRecSegment * segment,
                            const double ckTheta,
                            std::vector<Gaudi::XYZPoint> & points,
                            const unsigned int nPoints,
                            const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const;

      // Ray trace the Cherenkov cone using the given emission point, direction and Cherenkov angle
      StatusCode rayTrace ( const Rich::DetectorType rich,
                            const Gaudi::XYZPoint & emissionPoint,
                            const Gaudi::XYZVector & direction,
                            const double ckTheta,
                            std::vector<Gaudi::XYZPoint> & points,
                            const unsigned int nPoints,
                            const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const;

      // Ray trace the Cherenkov cone for the given ring to the detector plane
      StatusCode rayTrace ( LHCb::RichRecRing * ring,
                            const unsigned int nPoints,
                            const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                            const bool forceTracing = false ) const;

      // Ray trace the Cherenkov cone for the given ring to the detector plane
      StatusCode rayTrace ( const Rich::DetectorType rich,
                            const Gaudi::XYZPoint & emissionPoint,
                            const Gaudi::XYZVector & direction,
                            const double ckTheta,
                            LHCb::RichRecRing * ring,
                            const unsigned int nPoints,
                            const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                            const bool forceTracing = false ) const;

    private: // data

      // Pointers to tool instances
      const IRayTracing * m_rayTrace;     ///< Optical ray tracing tool
      const ICherenkovAngle * m_ckAngle;  ///< Cherenkov angle calculator tool
      const ISmartIDTool * m_smartIDTool; ///< RichSmartID manipulation tool

    };

  }
}

#endif // RICHRECBASE_RICHRAYTRACECHERENKOVCONE_H
