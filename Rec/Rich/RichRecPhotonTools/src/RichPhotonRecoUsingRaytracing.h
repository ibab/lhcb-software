
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingRaytracing.h
 *
 *  Header file for tool : Rich::Rec::PhotonRecoUsingRaytracing
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoUsingRaytracing.h,v 1.3 2008-02-18 10:50:06 jonrob Exp $
 *
 *  @author Claus P Buszello
 *  @date   2008-11-01
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPhotonRecoUsingRaytracing_H
#define RICHRECTOOLS_RichPhotonRecoUsingRaytracing_H 1

// STL
#include <iostream>
#include <sstream>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Base class and interfaces
#include "RichRecBase/RichRecToolBase.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichParticleProperties.h"
#include <RichKernel/IRichRayTracing.h>

// Kernel
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/RichTrackSegment.h"
#include "RichKernel/RichGeomPhoton.h"
#include "RichKernel/FastMaths.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonRecoUsingRaytracing RichPhotonRecoUsingRaytracing.h
     *
     *  Rich detector tool which reconstructs photons from track segments
     *  and smartIDs or global hit positions.
     *
     *  Attempts to do this in a very fast way, by estimating the CK photon angle
     *  using the track and photon hit positions on the detector plane.
     *
     *  @author Claus Buszello Claus.Buszello@cern.ch
     *  @date   2008-11-01
     */
    //-----------------------------------------------------------------------------

    class PhotonRecoUsingRaytracing : public Rich::Rec::ToolBase,
                                      virtual public IPhotonReconstruction
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonRecoUsingRaytracing( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

      virtual ~PhotonRecoUsingRaytracing( ); ///< Destructor

      // Initialization of the tool after creation
      virtual StatusCode initialize();
      virtual StatusCode finalize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                     const LHCb::RichRecPixel * pixel,
                                     LHCb::RichGeomPhoton& gPhoton ) const;

    private: // methods

      /// Check if the local coordinate hits a and b are on the same detector side
      inline bool sameSide( const Rich::RadiatorType rad,
                            const Gaudi::XYZPoint & a,
                            const Gaudi::XYZPoint & b ) const
      {
        return ( Rich::Rich2Gas == rad ? a.x() * b.x() > 0 : a.y() * b.y() > 0 );
      }

    private: // data

      /// RichSmartID tool
      const ISmartIDTool* m_idTool;

      const ICherenkovAngle * m_ckAngle; ///< Cherenkov angle tool

      const Rich::IRayTracing * m_raytrace; ///< raytracing tool

      float m_damping;
      std::vector<double> m_ERLSet;
      std::vector<float> m_maxdiff;
      std::vector<int> m_maxiter;
      std::vector<double> m_fudge;

      float m_ERL;

      bool failiter;
      bool discard;

      /// Cached working photon for ray-tracing
      mutable LHCb::RichGeomPhoton m_photon;

      //mutable int m_itersA[100];
      //mutable int m_iters1[100];
      //mutable int m_iters2[100];

    };

  }
}

#endif // RICHRECTOOLS_RichPhotonRecoUsingRaytracing_H
