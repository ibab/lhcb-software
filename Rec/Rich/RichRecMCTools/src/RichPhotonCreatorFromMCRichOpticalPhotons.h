
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonCreatorFromMCRichOpticalPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
#define RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H 1

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------------------------
      /** @class PhotonCreatorFromMCRichOpticalPhotons RichPhotonCreatorFromMCRichOpticalPhotons.h
       *
       *  Tool which performs the association between RichRecTracks and
       *  RichRecPixels to form RichRecPhotons. MC cheating is performed
       *  by only creating true cherenkov photons using MCRichOpticalPhoton
       *  information and also by using the MC geometrical information instead of
       *  the reconstructed values (angles etc.).
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   08/07/2004
       */
      //-----------------------------------------------------------------------------------------------

      class PhotonCreatorFromMCRichOpticalPhotons : public PhotonCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent );

        /// Destructor
        virtual ~PhotonCreatorFromMCRichOpticalPhotons(){}

        // Initialize method
        StatusCode initialize();

      private: // private methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const Rich::Rec::PhotonKey key ) const;

      private: // private data

        /// Rich Reconstruction MC Truth tool
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool; 

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
