// $Id: PatVeloSpaceTracking.h,v 1.3 2007-09-16 13:52:03 krinnert Exp $
#ifndef TF_PATVELOSPACETRACKING_H 
#define TF_PATVELOSPACETRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// track selection tool
#include "TrackInterfaces/ITrackSelector.h"

// PatVeloSpaceTool interface
#include "TrackInterfaces/ITracksFromTrack.h"

// local
#include "PatVeloSpaceTrack.h"
#include "PatVeloSpaceTool.h"

namespace Tf {

  /** @class PatVeloSpaceTracking PatVeloSpaceTracking.h
   *   
   *  Algorithm wich implements Velo space point tracking
   *  
   *  @author Olivier Callot
   *  @date   2005-06-09
   */
  class PatVeloSpaceTracking : public GaudiAlgorithm {
    public: 
      /// Standard constructor
      PatVeloSpaceTracking( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~PatVeloSpaceTracking( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    private:
      StatusCode storeTracks(std::vector<PatVeloSpaceTrack*> tracks);

      bool accept(const LHCb::Track& track); ///< Use TrackSelector if required

    private:

      std::string m_inputTracksLocation;
      std::string m_outputTracksLocation;
      std::string m_trackSelectorName; /// name of the tool to accept tracks

      ITrackSelector* m_trackSelector; /// pointer to the tool to select tracks

      ITracksFromTrack * m_PatVeloSpaceTool; ///< tool that does the pattern recog.

      LHCb::Tracks* m_inputTracks;   ///< Container for input RZ tracks
      LHCb::Tracks* m_outputTracks;  ///< Container for output Space tracks

      /// sort function for track lengths; then by sensor number in a tie
      /// NOTE using greater [instead of less] to make longer tracks sort first
      struct greater_trackLength{    
        bool operator()(LHCb::Track* const &first, 
            LHCb::Track* const &second) { 
          if ( first->nLHCbIDs() == second->nLHCbIDs() ) {
            return ( first->lhcbIDs()[0].veloID().sensor() > 
                second->lhcbIDs()[0].veloID().sensor() );
          }
          return ( first->nLHCbIDs() > second->nLHCbIDs() ) ;
        }
      };

  };
}
#endif // TF_PATVELOSPACETRACKING_H
