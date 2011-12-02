
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorCheatedTrackDir.h
 *
 *  Header file for tool : Rich::Rec::PhotonCreatorCheatedTrackDir
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONCREATOR_H
#define RICHRECTOOLS_RICHPHOTONCREATOR_H 1

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class PhotonCreatorCheatedTrackDir RichPhotonCreatorCheatedTrackDir.h
       *
       *  Tool which performs the association between RichRecTracks and
       *  RichRecPixels to form RichRecPhotons. For signal photos, cheats by using the
       *  MC track direction at emission point.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/03/2002
       */
      //-----------------------------------------------------------------------------

      class PhotonCreatorCheatedTrackDir : public PhotonCreatorBase
      {

      public: // Methods for Gaudi Framework

        /// Standard constructor
        PhotonCreatorCheatedTrackDir( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

        /// Destructor
        virtual ~PhotonCreatorCheatedTrackDir(){}

        // Initialize method
        StatusCode initialize();

      protected: // methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const Rich::Rec::PhotonKey key ) const;

      private: // private data

        /// Rich Reconstruction MC Truth tool
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;

        /// Delegated photon creator for reco photons
        const IPhotonCreator * m_recoPhotCr;

        /// Flag to turn on cheating of track directions
        bool m_cheatDir;

        /// Flag to turn on cheating of track positions
        bool m_cheatPos;

      };

    }
  }
}

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
