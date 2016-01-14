
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

#ifndef RICHRECPHOTONTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H
#define RICHRECPHOTONTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H 1

// STL
#include <sstream>
#include <limits>

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
     *  @date   2003-11-14
     */
    //-----------------------------------------------------------------------------

    class PhotonRecoUsingCKEstiFromRadius : public PhotonRecoBase,
                                            virtual public IPhotonReconstruction
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonRecoUsingCKEstiFromRadius( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

      /// Destructor
      virtual ~PhotonRecoUsingCKEstiFromRadius( ); 

      /// Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Reconstructs the geometrical photon candidate for a given RichTrackSegment
      // and RichSmartID channel identifier
      StatusCode reconstructPhoton ( const LHCb::RichRecSegment * segment,
                                     const LHCb::RichRecPixel * pixel,
                                     LHCb::RichGeomPhoton& gPhoton ) const;

    private: // methods
      
            /// Access the nominal saturated CK theta for the given radiator
      inline double nomSatCKTheta( const Rich::RadiatorType rad ) const
      {
        if ( m_nomSatCKTheta[rad] < 0 )
        {
          m_nomSatCKTheta[rad] = m_ckAngle->nominalSaturatedCherenkovTheta(rad);
        }
        return m_nomSatCKTheta[rad]; 
      }

      inline double minCalibRingRadius( const Rich::RadiatorType rad ) const
      {
        return nomSatCKTheta(rad) * m_minFracCKtheta[rad];
      }

    private: // data

      /// RichSmartID tool
      const ISmartIDTool * m_idTool = nullptr;

      /// Pointer to ring creator
      const IMassHypothesisRingCreator * m_massHypoRings = nullptr;

      /// Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle = nullptr; 

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

      /// Option to only use lightest hypothesis for calibration points (default false)
      bool m_useLightestHypoOnly = false;

      /// Cache (for speed) the nominal saturated CK theta values for each radiator
      mutable std::vector<double> m_nomSatCKTheta;

      /// Min fraction of saturated CK theta angle for a ring to be used as a calibration source
      std::vector<double> m_minFracCKtheta;

    };

  }
}

#endif // RICHRECPHOTONTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H
