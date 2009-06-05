// $Id: RichRingMasterAlg.h,v 1.3 2009-06-05 17:21:31 jonrob Exp $
#ifndef RICHRINGMASTERALG_H
#define RICHRINGMASTERALG_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecTupleAlgBase.h"

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichRecRing.h"
//Interfaces
#include "RichRecBase/IRichTrackSelector.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichRingMasterAlg RichRingMasterAlg.h
       *
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-17
       */
      class RichRingMasterAlg : public RichRingRecTupleAlgBase {
      public:
        /// Standard constructor
        RichRingMasterAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RichRingMasterAlg( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalization

        StatusCode FindRingsWithTracks();  // main ring finding loop for radiators and TrackSegments
        StatusCode ReconstructMassForRings(); //  find mass for TrackSegments and tracks
        StatusCode SetRadiatorSpecificParam(int irad);
        StatusCode ResetRichEventRingReconParam();

        StatusCode StoreRingInfoInNtup();
        StatusCode saveRingsInTES();
        inline LHCb::RichRecRings*  getRings(const std::string aLocation) const
        {
          return getOrCreate<LHCb::RichRecRings,LHCb::RichRecRings>(aLocation);

        }


      protected:

      private:
        const ITrackSelector * m_trSelector; ///< Track selector
        bool m_storeNtupRadius;
        std::string m_ringLocation;
        bool m_activateRingMassFinder;
        bool m_activateSavingRingsOnTES;


      };

    }
  }
}

#endif // RICHRINGMASTERALG_H


