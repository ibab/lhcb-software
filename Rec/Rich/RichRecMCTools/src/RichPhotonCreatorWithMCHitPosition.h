
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCHitPosition.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonCreatorWithMCHitPosition
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   06/11/2007
 */
//-----------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPhotonCreatorWithMCHitPosition_H
#define RICHRECMCTOOLS_RichPhotonCreatorWithMCHitPosition_H 1

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
      /** @class PhotonCreatorWithMCHitPosition RichPhotonCreatorWithMCHitPosition.h
       *
       *  Tool which performs the association between RichRecTracks and
       *  RichRecPixels to form RichRecPhotons. 
       *
       *  This implementation performs some MC cheating
       *  by using the MC hit position on the HPD entrance window.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   06/11/2007
       */
      //-----------------------------------------------------------------------------------------------

      class PhotonCreatorWithMCHitPosition : public PhotonCreatorBase
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonCreatorWithMCHitPosition( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

        /// Destructor
        virtual ~PhotonCreatorWithMCHitPosition();

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
        const IPhotonCreator * m_recoPhotCr;  ///< Delegated photon creator

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RichPhotonCreatorWithMCHitPosition_H
