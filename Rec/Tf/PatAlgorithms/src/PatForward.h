// $Id: PatForward.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATFORWARD_H
#define PATFORWARD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/IPatForwardTool.h"
#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"


  /** @class PatForward PatForward.h
   *  Forward pattern recognition. Connect a Velo track to the T stations.
   *
   *  @author Olivier Callot
   *  @date   2005-04-01 Initial version
   *  @date   2007-08-20 Update for A-Team framework 
   */

  class PatForward : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PatForward( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatForward( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    
  protected:
    
    bool acceptTrack(const LHCb::Track& track);
    
  private:
    std::string      m_inputTracksName;
    std::string      m_outputTracksName;
    std::string      m_trackSelectorName;
    int m_deltaNumberInTT;
    int m_deltaNumberInT;
    bool m_doClean;
    
    Tf::TStationHitManager <PatForwardHit> *  m_tHitManager;
    
    ITrackSelector*      m_trackSelector;
    IPatForwardTool*     m_forwardTool;
    ISequencerTimerTool* m_timerTool;
    int              m_fwdTime;
  };
#endif // PATFORWARD_H
