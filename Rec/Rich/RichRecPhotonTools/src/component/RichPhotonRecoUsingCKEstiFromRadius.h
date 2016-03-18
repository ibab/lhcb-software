
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
#include <vector>

// Base class and interfaces
#include "RichPhotonRecoBase.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichParticleProperties.h"

// Kernel
#include "Kernel/RichSide.h"
#include "Kernel/RichSmartID.h"

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
      
      inline double minCalibRingRadius( const Rich::RadiatorType rad ) const
      {
        if ( m_minCalibRingRadius[rad] < 0 )
        {
          m_minCalibRingRadius[rad] = ( m_ckAngle->nominalSaturatedCherenkovTheta(rad) *
                                        m_minFracCKtheta[rad] );
        }
        return m_minCalibRingRadius[rad];
      }

    private: // data

      /// Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle = nullptr; 

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

      /// Min fraction of saturated CK theta angle for a ring to be used as a calibration source
      std::vector<double> m_minFracCKtheta;

      /// Flag to turn on the rejection of 'ambiguous' photons
      bool m_rejAmbigPhots;

      /// Flag to turn on interpolation between two nearest rings, by radiator
      std::vector<bool> m_useRingInterp;

      /// Cache (for speed) the minimum CK ring radius for use as a calibration source
      mutable std::vector<double> m_minCalibRingRadius;

    };

  }
}

#endif // RICHRECPHOTONTOOLS_RichPhotonRecoUsingCKEstiFromRadius_H
