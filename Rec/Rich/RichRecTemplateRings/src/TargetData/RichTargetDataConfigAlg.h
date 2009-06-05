// $Id: RichTargetDataConfigAlg.h,v 1.3 2009-06-05 17:21:33 jonrob Exp $
#ifndef RICHTARGETDATACONFIGALG_H
#define RICHTARGETDATACONFIGALG_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecAlgBase.h"

// Event
#include "Event/RichRecTrack.h"

//Interfaces
#include "RichRecBase/IRichTrackSelector.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      /** @class RichTargetDataConfigAlg RichTargetDataConfigAlg.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-05-14
       */
      class RichTargetDataConfigAlg : public RichRingRecAlgBase {
      public:
        /// Standard constructor
        RichTargetDataConfigAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RichTargetDataConfigAlg( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalization

        StatusCode AcquireTargetRichHitInfo();
        StatusCode AcquireTargetTrackInfo();

      protected:

      private:

        const ITrackSelector * m_trSelector; ///< Track selector
        bool m_selectTracksAboveMomentumThreshold; // select tracks above the
        // minimum Cherenkov threshold.

      };

    }
  }
}

#endif // RICHTARGETDATACONFIGALG_H
