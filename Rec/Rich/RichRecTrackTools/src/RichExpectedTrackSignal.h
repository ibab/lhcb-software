
//-----------------------------------------------------------------------------
/** @file RichExpectedTrackSignal.h
 *
 *  Header file for tool : Rich::Rec::ExpectedTrackSignal
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/RichRecTrack.h"
#include "Event/RichPID.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichRayleighScatter.h"
#include "RichRecBase/IRichGasQuartzWindowAbs.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class ExpectedTrackSignal RichExpectedTrackSignal.h
     *
     *  Tool to calculate the expected signals for given
     *  RichRecSegments and RichRecTracks.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class ExpectedTrackSignal : public Rich::Rec::ToolBase,
                                virtual public IExpectedTrackSignal
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      ExpectedTrackSignal( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      /// Destructor
      virtual ~ExpectedTrackSignal() {}

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Expected number of observable signal photons for given track and hypothesis
      double nSignalPhotons ( LHCb::RichRecTrack * track,
                              const Rich::ParticleIDType id ) const;

      // Expected number of signal photons for given segment and hypothesis
      double nSignalPhotons ( LHCb::RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

      // Expected number of observable signal photons for given track and hypothesis
      double nObservableSignalPhotons ( LHCb::RichRecTrack * track,
                                        const Rich::ParticleIDType id ) const;

      // Expected number of observable signal photons for given segment and hypothesis
      double nObservableSignalPhotons ( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const;

      // Expected number of scattered photons for given track and hypothesis
      double nScatteredPhotons ( LHCb::RichRecTrack * track,
                                 const Rich::ParticleIDType id ) const;

      // Expected number of scattered photons for given segment and hypothesis
      double nScatteredPhotons ( LHCb::RichRecSegment * segment,
                                 const Rich::ParticleIDType id ) const;

      // Expected number of observable scattered photons for given track and hypothesis
      double nObservableScatteredPhotons ( LHCb::RichRecTrack * track,
                                           const Rich::ParticleIDType id ) const;

      // Expected number of observable scattered photons for given segment and hypothesis
      double nObservableScatteredPhotons ( LHCb::RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const;

      // Expected number of observable signal and scattered photons for
      //  the given track and hypothesis
      double nTotalObservablePhotons ( LHCb::RichRecTrack * track,
                                       const Rich::ParticleIDType id ) const;

      // Expected number of observable signal+scattered photons for given segment and hypothesis
      double nTotalObservablePhotons ( LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const;

      // Expected number of emitted photons for given track and hypothesis
      double nEmittedPhotons ( LHCb::RichRecTrack * track,
                               const Rich::ParticleIDType id ) const;

      // Expected number of emitted photons for given segment and hypothesis
      double nEmittedPhotons ( LHCb::RichRecSegment * segment,
                               const Rich::ParticleIDType id ) const;

      // Expected number of emitted photons for given segment and hypothesis,
      //  scaled by the HPD quantum efficiency
      double nDetectablePhotons ( LHCb::RichRecTrack * track,
                                  const Rich::ParticleIDType id ) const;

      //  Expected number of emitted photons for given segment and hypothesis,
      //  scaled by the HPD quantum efficiency
      double nDetectablePhotons ( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const;

      // The average energy of signal Cherenkov photons for the given ID
      double avgSignalPhotEnergy( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const;

      // The average energy of emitted Cherenkov photons for the given ID
      double avgEmitPhotEnergy( LHCb::RichRecSegment * segment,
                                const Rich::ParticleIDType id ) const;

      // The average energy of signal Cherenkov photons
      double avgSignalPhotEnergy( LHCb::RichRecSegment * segment ) const;

      // The average energy of emitted Cherenkov photons
      double avgEmitPhotEnergy( LHCb::RichRecSegment * segment ) const;

      // Is it possible for this track to give Rich information
      bool hasRichInfo( LHCb::RichRecTrack * track ) const;

      // Is it geometrically possible for this segment to give Rich information
      bool hasRichInfo( LHCb::RichRecSegment * segment ) const;

      // Is this track above threshold for a given particle type
      bool aboveThreshold( LHCb::RichRecTrack * track,
                           const Rich::ParticleIDType type ) const;

      // Is this track above threshold for a given particle type in a given radiator
      bool aboveThreshold( LHCb::RichRecTrack * track,
                           const Rich::ParticleIDType type,
                           const Rich::RadiatorType radiator ) const;

      // Is this segment above threshold for a given particle hypothesis
      bool aboveThreshold( LHCb::RichRecSegment * segment,
                           const Rich::ParticleIDType type ) const;

      // Set the threshold information in a RichPID object for given track
      void setThresholdInfo( LHCb::RichRecTrack * track,
                             LHCb::RichPID * pid ) const;

      // Set the threshold information in a RichPID object for given segment
      void setThresholdInfo( LHCb::RichRecSegment * segment,
                             LHCb::RichPID * pid ) const;

      // Is this track active in this radiator medium for given particle id
      bool activeInRadiator( LHCb::RichRecTrack * track,
                             const Rich::RadiatorType rad,
                             const Rich::ParticleIDType id = Rich::Electron ) const;

    private:  // Private data

      // Pointers to tool instances
      const IGeomEff            * m_geomEff      = nullptr; ///< Geometrical efficiency tool
      const ISellmeirFunc       * m_sellmeir     = nullptr; ///< Sellmeir function
      const ISignalDetectionEff * m_sigDetEff    = nullptr; ///< signal detection efficiency
      const IParticleProperties * m_richPartProp = nullptr; ///< General particle properties
      const IRayleighScatter    * m_rayScat      = nullptr; ///< Rayleigh scattering function
      const IGasQuartzWindowAbs * m_gasQuartzWin = nullptr; ///< Absorption at the quartz window

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

      /** Minimum number of photons in each radiator for a radiator segment 
       *  to be considered as having RICH information */
      std::vector<double> m_minPhotonsPerRad;

      /// Threshold momentum scale factor
      double m_pThresScale;

      // Minimum momentum by radiator for track segments.
      std::vector<double> m_minPbyRad;

      // Maximum momentum by radiator for track segments.
      std::vector<double> m_maxPbyRad;

    };

  }
}

#endif // RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
