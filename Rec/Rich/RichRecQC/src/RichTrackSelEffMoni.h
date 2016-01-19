
//---------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RichTrackSelEffMoni_H
#define RICHRECQC_RichTrackSelEffMoni_H 1

// STL
#include <string>
#include <set>

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

      private:

        // Book the histos for the given track class
        void prebookHistograms( const std::string & tkClass );

        // track plots
        void trackPlots( const std::string & tag );
        
        /// Fill track plots for given class tag
        void fillTrackPlots( const LHCb::Track * track,
                             const LHCb::RichRecTrack * rTrack,
                             const std::string & tkClass );

        /// Get RichRecTrack for a given Track
        const LHCb::RichRecTrack * getRichTrack( const LHCb::Track * track ) const;
  
      private: // data

        /// Pointer to RichRecMCTruthTool interface
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr;

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr;

        /// MCParticle association weight
        double m_mcAssocWeight;

      };

    }
  }
}

#endif // RICHRECQC_RichTrackSelEffMoni_H
