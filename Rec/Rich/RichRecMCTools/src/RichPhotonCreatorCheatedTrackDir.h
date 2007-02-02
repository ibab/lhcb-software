
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorCheatedTrackDir.h
 *
 *  Header file for tool : Rich::Rec::PhotonCreatorCheatedTrackDir
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorCheatedTrackDir.h,v 1.3 2007-02-02 10:06:27 jonrob Exp $
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
#include "RichRecBase/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @namespace MC
     *
     *  General namespace for RICH MC related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   05/12/2006
     */
    //-----------------------------------------------------------------------------
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

        // Finalize method
        StatusCode finalize();

      protected: // methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const RichRecPhotonKey key ) const;

      private: // private data

        /// Rich Reconstruction MC Truth tool
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;

        /// Delegated photon creator for reco photons
        const IPhotonCreator * m_recoPhotCr;

      };

    }
  }
}

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
