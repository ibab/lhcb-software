//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingCKEstiFromRadius.h
 *
 *  Header file for tool : Rich::Rec::PhotonRecoUsingCKEstiFromRadius
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis
 *  @date   2003-11-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H
#define RICHRECTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H 1

// STL
#include <sstream>

// Base class and interfaces
#include "RichPhotonRecoBase.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichParticleProperties.h"

// Kernel
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/RichTrackSegment.h"
#include "RichKernel/RichGeomPhoton.h"

// GSL
#include "gsl/gsl_math.h"

// VDT
#include "vdt/atan2.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonRecoUsingCKEstiFromRadius RichPhotonRecoUsingCKEstiFromRadius.h
     *
     *  Rich detector tool which reconstructs photons from track segments
     *  and smartIDs or global hit positions.
     *
     *  Attempts to do this in a very fast way, by estimating the CK photon angle
     *  using the track and photon hit positions on the detector plane.
     *
     *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
     */
    //-----------------------------------------------------------------------------

    class PhotonRecoUsingCKEstiFromRadius : public PhotonRecoBase,
                                            virtual public IPhotonReconstruction
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonRecoUsingCKEstiFromRadius( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent);

      virtual ~PhotonRecoUsingCKEstiFromRadius( ); ///< Destructor

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                     const LHCb::RichRecPixel * pixel,
                                     LHCb::RichGeomPhoton& gPhoton ) const;

    private: // data

      /// RichSmartID tool
      const ISmartIDTool* m_idTool;

      /// Pointer to ring creator
      const IMassHypothesisRingCreator * m_massHypoRings;

      const ICherenkovAngle * m_ckAngle; ///< Cherenkov angle tool

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

    };

  }
}

#endif // RICHRECTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H
