
//-----------------------------------------------------------------------------
/** @file RichRayTraceCherenkovCone.h
 *
 *  Header file for tool : Rich::Rec::RayTraceCherenkovCone
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichRayTraceCherenkovCone_H
#define RICHRECTOOLS_RichRayTraceCherenkovCone_H 1

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichPhotonEmissionPoint.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecSegment.h"

// RichKernel
#include "RichKernel/RichMap.h"

// boost
#include "boost/assign/list_of.hpp"

// VDT
#include "vdt/sincos.h"

namespace Rich
{
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
      virtual ~RayTraceCherenkovCone();

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

    private: // helper classes

      /** @class CosSinPhi RichRayTraceCherenkovCone.h
       *
       *  Utility class to cache cos and sin values
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   17/02/2008
       */
      class CosSinPhi
      {
      public:
        typedef std::vector<CosSinPhi> Vector;
      public:
        explicit CosSinPhi( const float& _phi ) : phi(_phi) 
        {
          // cache sin(phi) and cos(phi) using fast VDT method
          //vdt::fast_sincos( phi, sinPhi, cosPhi );
          vdt::fast_sincosf( phi, sinPhi, cosPhi );
        }
      public:
        float phi    = 0; ///< CK phi
        float cosPhi = 0; ///< Cos(CK phi)
        float sinPhi = 0; ///< Sin(CK phi)
      };

    private: // methods

      // Trace a single photon
      LHCb::RichTraceMode::RayTraceResult traceAphoton ( const CosSinPhi& sinCosPhi,
                                                         const Rich::DetectorType rich,
                                                         LHCb::RichRecRing * ring,
                                                         const Gaudi::XYZPoint & emissionPoint,
                                                         const Gaudi::XYZVector & photDir,
                                                         const LHCb::RichTraceMode mode ) const;

    private:

      /// Returns the cos and sin phi values for the given number of points
      inline const CosSinPhi::Vector & cosSinValues( const unsigned int nPoints ) const
      {
        auto & vect = m_cosSinPhi[nPoints];
        if ( vect.empty() ) { fillCosSinValues( vect, nPoints ); }
        return vect;
      }

      /// Fill the cosna nd sin vector for the given number of points
      void fillCosSinValues( CosSinPhi::Vector & vect, 
                             const unsigned int nPoints ) const;

    private:

      // Pointers to tool instances
      const IRayTracing          * m_rayTrace    = nullptr; ///< Optical ray tracing tool
      const ICherenkovAngle      * m_ckAngle     = nullptr; ///< Cherenkov angle calculator tool
      const ISmartIDTool         * m_smartIDTool = nullptr; ///< RichSmartID manipulation tool
      const IGeomTool            * m_geomTool    = nullptr; ///< Geometry tool
      const IPhotonEmissionPoint * m_emissPoint  = nullptr; ///< Estimated emission point tool

      /** Bailout fraction. If no ray tracings have worked after this fraction have been
       *  perfromed, then give up */
      std::vector<float> m_bailoutFrac;

      /// Temporary working photon object
      mutable LHCb::RichGeomPhoton m_photon;

      /// Cache of sin and cos values for various number of points on the ring
      mutable Rich::Map<unsigned int,CosSinPhi::Vector> m_cosSinPhi;

    };

  }
}

#endif // RICHRECTOOLS_RichRayTraceCherenkovCone_H
