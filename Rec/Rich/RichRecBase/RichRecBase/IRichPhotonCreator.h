
//-----------------------------------------------------------------------------
/** @file IRichPhotonCreator.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IPhotonCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHPHOTONCREATOR_H
#define RICHRECTOOLS_IRICHPHOTONCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichRecSegment.h"

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonCreator( "Rich::Rec::IPhotonCreator" , 1 , 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IPhotonCreator IRichPhotonCreator.h
     *
     *  Interface for tool which performs the association between RichRecSegments
     *  and RichRecPixels to form RichRecPhotons.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IPhotonCreator : public virtual IAlgTool 
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichPhotonCreator; }

      /** Access to all RichRecPhotons currently created using this tool
       *  Tool should never return a null pointer.
       *
       *  @return Pointer to the container of RichRecPhotons
       */
      virtual LHCb::RichRecPhotons * richPhotons() const = 0;

      /** Checks whether a photon candidate exists for the given segment and pixel pair
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param pixel   Pointer to the RichRecPixel
       *
       *  @return Pointer to the reconstructed photon candidate
       *  @retval !NULL A valid reconstructed photon exists for this pixel/segment pair
       *  @retval NULL  No photon exists for this pixel/segment pair.
       *                Note this DOES NOT mean it isn't possible to reconstruct a candidate
       *                ( see reconstructPhoton methods ).
       */
      virtual
      LHCb::RichRecPhoton * checkForExistingPhoton( LHCb::RichRecSegment * segment,
                                                    LHCb::RichRecPixel * pixel ) const = 0;

      /** Form a RichRecPhoton candidate from a given segment and a pixel.
       *  Whether or not this method succeeds in creating a photon object
       *  depends on the particular implementation being used, and its runtime
       *  configuration.
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param pixel   Pointer to the RichRecPixel
       *
       *  @return Pointer to the reconstructed photon candidate
       *  @retval !NULL A valid reconstructed photon was possible
       *  @retval NULL  It was not possible to produce a valid photon candidate
       */
      virtual
      LHCb::RichRecPhoton * reconstructPhoton( LHCb::RichRecSegment * segment,
                                               LHCb::RichRecPixel * pixel ) const = 0;

      /** Form all valid photon candidates for a given track and pixel.
       *  Whether or not this method succeeds in creating photon objects
       *  depends on the particular implementation being used, and its runtime
       *  configuration.
       *
       *  @param track Pointer to the RichRecTrack
       *  @param pixel Pointer to the RichRecPixel
       *
       *  @return Container of photon candidates
       */
      virtual
      LHCb::RichRecTrack::Photons reconstructPhotons( LHCb::RichRecTrack * track,
                                                      LHCb::RichRecPixel * pixel ) const = 0;

      /** Form all photon candidates for a given track, with all possible pixels.
       *  Whether or not this method succeeds in creating photon objects
       *  depends on the particular implementation being used, and its runtime
       *  configuration.
       *
       *  @param track Pointer to the RichRecTrack
       *
       *  @return Container of photon candidates
       */
      virtual const LHCb::RichRecTrack::Photons &
      reconstructPhotons( LHCb::RichRecTrack * track ) const = 0;

      /** Form all photon candidates for a given pixel, with all possible tracks.
       *  Whether or not this method succeeds in creating photon objects
       *  depends on the particular implementation being used, and its runtime
       *  configuration.
       *
       *  @param pixel Pointer to the RichRecPixel
       *
       *  @return Container of photon candidates
       */
      virtual const LHCb::RichRecPixel::Photons &
      reconstructPhotons( LHCb::RichRecPixel * pixel ) const = 0;

      /** Form all photon candidates for a given segment, with all possible pixels.
       *  Whether or not this method succeeds in creating photon objects
       *  depends on the particular implementation being used, and its runtime
       *  configuration.
       *
       *  @param segment Pointer to the RichRecSegment
       *
       *  @return Container of photon candidates
       */
      virtual const LHCb::RichRecSegment::Photons &
      reconstructPhotons( LHCb::RichRecSegment * segment ) const = 0;

      /** Method to perform the reconstruction of all tracks and pixels.
       *  The most efficient methods to create all possible photon candidates.
       *  @return StatusCode indicating if request was successful or not
       *  @retval StatusCode::SUCCESS Photon reconstruction was successful
       *  @retval StatusCode::FAILURE Photon reconstruction failed
       */
      virtual StatusCode reconstructPhotons() const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHPHOTONCREATOR_H
