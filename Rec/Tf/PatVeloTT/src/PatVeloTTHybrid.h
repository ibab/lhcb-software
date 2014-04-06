// $Id: PatVeloTT.h,v 1.5 2007-10-10 18:42:24 smenzeme Exp $
#ifndef PATVELOTTHYBRID_H
#define PATVELOTTHYBRID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// from TrackInterfaces
#include "TrackInterfaces/ITracksFromTrack.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "TfKernel/TTStationHitManager.h"
#include "Event/Track.h"

// local
//#include "PatVeloTTHybridTool.h"
#include "PatKernel/PatTTHit.h"
#include "PatTTMagnetTool.h"

  /** @class PatVeloTTHybrid PatVeloTTHybrid.h
   *
   *  Pat VeloTTHybrid algorithm
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PatVeloTTHybrid : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PatVeloTTHybrid( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatVeloTTHybrid( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:
    void removeUsedTracks( std::vector<LHCb::Track*>& veloTracks);
    bool matchingTracks( LHCb::Track* vttcand, LHCb::Track* trackused);

  private:
    class compChi2  {
    public:
      bool operator() (LHCb::Track* first, LHCb::Track* second ) {
        return fabs(first->chi2PerDoF()) < fabs(second->chi2PerDoF()) ;
      }
    };
    class compVeloTx  {
    public:
      bool operator() (const LHCb::Track* first, const LHCb::Track* second ) {
        return first->closestState(LHCb::State::EndVelo).tx() < second->closestState(LHCb::State::EndVelo).tx();
      }
    };

    Tf::TTStationHitManager<PatTTHit> *      m_ttHitManager;

    std::string m_inputTracksName;    ///< input container name
    std::string m_outputTracksName;   ///< output container name
    bool m_removeUsedTracks;
    /// The fitter tool
    std::string m_fitterName;
    ITrackFitter* m_tracksFitter;
    ITracksFromTrack* m_veloTTTool;
    std::vector< std::string > m_inputUsedTracksNames;
    float m_maxChi2;
    bool m_fitTracks; 
    ISequencerTimerTool* m_timerTool;
    int  m_veloTTTime;
    bool m_doTiming;
    
  };

#endif // PATVELOTT_H
