
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonCreatorWithMCSignal
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
      /** @class PhotonCreatorWithMCSignal RichPhotonCreatorWithMCSignal.h
       *
       *  Tool which performs the association between RichRecTracks and
       *  RichRecPixels to form RichRecPhotons. MC cheating is performed
       *  by for true Cherenkov photon candidates referring to the associated
       *  MCRichOpticalPhoton to get the true Cherenkov angles etc. For fake photons
       *  the reconstructed quantities are used.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   08/07/2004
       */
      //-----------------------------------------------------------------------------------------------

      class PhotonCreatorWithMCSignal : public PhotonCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonCreatorWithMCSignal( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

        /// Destructor
        virtual ~PhotonCreatorWithMCSignal();

        // Initialize method
        StatusCode initialize();

      private: // private methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const Rich::Rec::PhotonKey key ) const;

      private: // private data

        // Pointers to tool instances
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;  ///< Rich Reconstruction MC Truth tool
        const IPhotonCreator * m_mcPhotCr;    ///< Delegated photon creator for MC photons
        const IPhotonCreator * m_recoPhotCr;  ///< Delegated photon creator for reco photons

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
