
//---------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  CVS Log :-
 *  $Id: RichTrackSelEffMoni.h,v 1.4 2009-07-27 20:35:28 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RichTrackSelEffMoni_H
#define RICHRECQC_RichTrackSelEffMoni_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichTrackSelector.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class TrackSelEff RichTrackSelEffMoni.h
       *
       *  Monitors the efficiency of the RICH track selection
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   21/05/2009
       */
      //---------------------------------------------------------------------------------

      class TrackSelEff : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        TrackSelEff( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~TrackSelEff( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      protected:
        
        /// Pre-Book all (non-MC) histograms
        virtual StatusCode prebookHistograms();
  
      private: // data

        /// Pointer to RichRecMCTruthTool interface
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Input location of Tracks in TES
        std::string m_trTracksLocation;

        /// MCParticle association weight
        double m_mcAssocWeight;

      };

    }
  }
}

#endif // RICHRECQC_RichTrackSelEffMoni_H
