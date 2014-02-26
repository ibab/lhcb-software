// $Id: PrVeloUT.h,v 1.5 2007-10-10 18:42:24 smenzeme Exp $
#ifndef PRVELOUT_H
#define PRVELOUT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "TfKernel/UTStationHitManager.h"

// local
#include "IPrVeloUTTool.h"
#include "PrKernel/PrUTHit.h"
#include "PrUTMagnetTool.h"

  /** @class PrVeloUT PrVeloUT.h
   *
   *  Pr VeloUT algorithm
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PrVeloUT : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PrVeloUT( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PrVeloUT( ); ///< Destructor

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

    Tf::UTStationHitManager<PrUTHit> *      m_utHitManager;

    std::string m_inputTracksName;    ///< input container name
    std::string m_outputTracksName;   ///< output container name
    bool m_removeUsedTracks;
    /// The fitter tool
    std::string m_fitterName;
    ITrackFitter* m_tracksFitter;
    IPrVeloUTTool* m_veloUTTool;
    std::vector< std::string > m_inputUsedTracksNames;
    float m_maxChi2;
    bool m_fitTracks; 
    ISequencerTimerTool* m_timerTool;
    int  m_veloUTTime;
    bool m_doTiming;
    
  };

#endif // PRVELOUT_H
