
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonCreatorWithMCSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithMCSignal.h,v 1.6 2007-02-02 10:06:27 jonrob Exp $
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

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
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
        virtual ~PhotonCreatorWithMCSignal(){}

        // Initialize method
        StatusCode initialize();

        // Finalize method
        StatusCode finalize();

      private: // private methods

        /// Form a Photon candidate from a Segment and a pixel.
        virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                   LHCb::RichRecPixel * pixel,
                                                   const RichRecPhotonKey key ) const;

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
