// $Id: PatVeloTT.h,v 1.4 2007-09-06 16:17:16 smenzeme Exp $
#ifndef PATVELOTT_H
#define PATVELOTT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "TfKernel/TTStationHitManager.h"

// local
#include "PatKernel/PatTTHit.h"

namespace Tf {

  /** @class PatVeloTT PatVeloTT.h
   *
   *  Pat VeloTT algorithm
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PatVeloTT : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PatVeloTT( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatVeloTT( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:
    bool acceptTrack(const LHCb::Track& track);
    void removeUsedTracks( std::vector<LHCb::Track*>& veloTracks);
    bool matchingTracks( LHCb::Track* vttcand, LHCb::Track* trackused);

  private:
    class compChi2  {
    public:
      bool operator() (LHCb::Track* first, LHCb::Track* second ) {
        return fabs(first->chi2PerDoF()) < fabs(second->chi2PerDoF()) ;
      }
    };

    ITrackSelector* m_trackSelector; // tool to accept a track

    TTStationHitManager<PatTTHit> *      m_ttHitManager;

    std::string m_inputTracksName;    ///< input container name
    std::string m_outputTracksName;   ///< output container name
    std::string m_trackSelectorName; ///< name of the tool to accept a track
    bool m_removeUsedTracks;
    /// The fitter tool
    std::string m_fitterName;
    ITrackFitter* m_tracksFitter;
    PatVeloTTTool* m_veloTTTool;
    std::vector< std::string > m_inputUsedTracksNames;
    double m_maxChi2;
    bool m_fitTracks;

  };
}
#endif // PATVELOTT_H
