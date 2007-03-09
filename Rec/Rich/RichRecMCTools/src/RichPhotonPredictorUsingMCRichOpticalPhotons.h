
//---------------------------------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCRichOpticalPhotons.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::PhotonPredictorUsingMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonPredictorUsingMCRichOpticalPhotons.h,v 1.8 2007-03-09 22:57:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//---------------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H
#define RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------------------------
      /** @class PhotonPredictorUsingMCRichOpticalPhotons RichPhotonPredictorUsingMCRichOpticalPhotons.h
       *
       *  Tool which performs the association between RichRecSegments and
       *  RichRecPixels to form RichRecPhotons.  This particular instance uses
       *  MCRichOpticalPhoton objects to select only true Cherenkov photon combinations.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   08/07/2004
       */
      //---------------------------------------------------------------------------------------------------

      class PhotonPredictorUsingMCRichOpticalPhotons : public Rich::Rec::ToolBase,
                                                       virtual public IPhotonPredictor
      {

      public: // methods for Gaudi framework

        /// Standard constructor
        PhotonPredictorUsingMCRichOpticalPhotons( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent );

        /// Destructor
        virtual ~PhotonPredictorUsingMCRichOpticalPhotons(){}

        // Initialize method
        StatusCode initialize();

      public: // Public interface methods

        // Is it possible to make a photon candidate using this segment and pixel.
        bool photonPossible( LHCb::RichRecSegment * segment,
                             LHCb::RichRecPixel * pixel ) const;

      private: // private data

        // Pointers to tool instances
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool;  ///< MC Truth tool

      };

    }
  }
}

#endif // RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H
