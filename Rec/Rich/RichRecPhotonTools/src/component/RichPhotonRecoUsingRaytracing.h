//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingRaytracing.h
 *
 *  Header file for tool : Rich::Rec::PhotonRecoUsingRaytracing
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

// Base class and interfaces
#include "RichPhotonRecoBase.h"
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

// RichDet
#include "RichDet/Rich1DTabFunc.h"

// GSL
#include "gsl/gsl_math.h"

// VDT
#include "vdt/atan2.h"

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

    class PhotonRecoUsingRaytracing : public PhotonRecoBase,
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

      // tool finalization
      virtual StatusCode finalize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                     const LHCb::RichRecPixel * pixel,
                                     LHCb::RichGeomPhoton& gPhoton ) const;

    private: // data

      /// RichSmartID tool
      const ISmartIDTool* m_idTool;

      const ICherenkovAngle * m_ckAngle; ///< Cherenkov angle tool

      const Rich::IRayTracing * m_raytrace; ///< raytracing tool

      double m_damping;
      std::vector<double> m_ERLSet;
      std::vector<double> m_maxdiff;
      std::vector<int> m_maxiter;

      double m_ERL;

      bool m_failiter;

      /// Cached working photon for ray-tracing
      mutable LHCb::RichGeomPhoton m_photon;

      /// cached ray tracing mode
      LHCb::RichTraceMode m_mode;

    };

  }
}

#endif // RICHRECTOOLS_RichPhotonRecoUsingRaytracing_H
