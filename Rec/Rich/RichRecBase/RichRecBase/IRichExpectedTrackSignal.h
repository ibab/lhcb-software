
//-----------------------------------------------------------------------------
/** @file IRichExpectedTrackSignal.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IExpectedTrackSignal
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
  class RichRecTrack;
  class RichPID;
}
//#include "Event/RichPID.h"

// Kernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichExpectedTrackSignal( "Rich::Rec::IExpectedTrackSignal", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    /** @class IExpectedTrackSignal IRichExpectedTrackSignal.h
     *
     *  Interface for tool to calculate the expected signals for given
     *  RichRecSegments and RichRecTracks.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */

    class IExpectedTrackSignal : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() {return IID_IRichExpectedTrackSignal;}

      /// Expected number of signal photons for given track and hypothesis
      virtual double nSignalPhotons ( LHCb::RichRecTrack * track,
                                      const Rich::ParticleIDType id ) const = 0;

      /// Expected number of signal photons for given segment and hypothesis
      virtual double nSignalPhotons ( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable signal photons for given track and hypothesis
      virtual double nObservableSignalPhotons ( LHCb::RichRecTrack * track,
                                                const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable signal photons for given segment and hypothesis
      virtual double nObservableSignalPhotons ( LHCb::RichRecSegment * segment,
                                                const Rich::ParticleIDType id ) const = 0;

      /// Expected number of scattered photons for given track and hypothesis
      virtual double nScatteredPhotons ( LHCb::RichRecTrack * track,
                                         const Rich::ParticleIDType id ) const = 0;

      /// Expected number of scattered photons for given segment and hypothesis
      virtual double nScatteredPhotons ( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable scattered photons for given track and hypothesis
      virtual double nObservableScatteredPhotons ( LHCb::RichRecTrack * track,
                                                   const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable scattered photons for given segment and hypothesis
      virtual double nObservableScatteredPhotons ( LHCb::RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable signal+scattered photons for given track and hypothesis
      virtual double nTotalObservablePhotons ( LHCb::RichRecTrack * track,
                                               const Rich::ParticleIDType id ) const = 0;

      /// Expected number of observable signal+scattered photons for given segment and hypothesis
      virtual double nTotalObservablePhotons ( LHCb::RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const = 0;

      /// Expected emitted photons for given track and hypothesis
      virtual double nEmittedPhotons ( LHCb::RichRecTrack * track,
                                       const Rich::ParticleIDType id ) const = 0;

      /// Expected number of emitted photons for given segment and hypothesis
      virtual double nEmittedPhotons ( LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const = 0;

      /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
      virtual double nDetectablePhotons ( LHCb::RichRecTrack * track,
                                          const Rich::ParticleIDType id ) const = 0;

      /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
      virtual double nDetectablePhotons ( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) const = 0;

      /// The average energy of signal Cherenkov photons for the given ID
      virtual double avgSignalPhotEnergy( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType id ) const = 0;

      /// The average energy of emitted Cherenkov photons for the given ID
      virtual double avgEmitPhotEnergy( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const = 0;

      /// The average energy of signal Cherenkov photons
      virtual double avgSignalPhotEnergy( LHCb::RichRecSegment * segment ) const = 0;
      
      /// The average energy of emitted Cherenkov photons
      virtual double avgEmitPhotEnergy( LHCb::RichRecSegment * segment ) const = 0;
      
      /// Is it possible for this track to give Rich information
      virtual bool hasRichInfo( LHCb::RichRecTrack * track ) const = 0;

      /// Is it geometrically possible for this segment to give Rich information
      virtual bool hasRichInfo( LHCb::RichRecSegment * segment ) const = 0;

      /// Is this track above threshold for a given particle type in any radiator
      virtual bool aboveThreshold( LHCb::RichRecTrack * track,
                                   const Rich::ParticleIDType type ) const = 0;

      /// Is this track above threshold for a given particle type in a given radiator
      virtual bool aboveThreshold( LHCb::RichRecTrack * track,
                                   const Rich::ParticleIDType type,
                                   const Rich::RadiatorType radiator ) const = 0;

      /// Is this segment above threshold for a given particle hypothesis
      virtual bool aboveThreshold( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType type ) const = 0;

      /// Set the threshold information in a RichPID object for given segment
      virtual void setThresholdInfo( LHCb::RichRecSegment * segment,
                                     LHCb::RichPID * pid ) const = 0;

      /// Set the threshold information in a RichPID object for given track
      virtual void setThresholdInfo( LHCb::RichRecTrack * track,
                                     LHCb::RichPID * pid ) const = 0;

      /// Is this track active in given radiator for given particle id
      virtual bool activeInRadiator( LHCb::RichRecTrack * track,
                                     const Rich::RadiatorType rad,
                                     const Rich::ParticleIDType id = Rich::Electron ) const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
