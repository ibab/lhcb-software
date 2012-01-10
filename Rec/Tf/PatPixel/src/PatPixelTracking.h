// $Id: $
#ifndef PATPIXELTRACKING_H 
#define PATPIXELTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "PatKernel/IPatDebugTool.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "PatPixelHitManager.h"
#include "PatPixelTrack.h"

/** @class PatPixelTracking PatPixelTracking.h
 *  This is the main tracking fo rthe Velo Pixel upgrade
 *
 *  @author Olivier Callot
 *  @date   2011-12-16
 */
class PatPixelTracking : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PatPixelTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatPixelTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void searchTracks ( int length );

  void makeLHCbTracks( LHCb::Tracks* tracks );

  //== Debugging methods
  bool matchKey( const PatPixelHit* hit ) {
    if ( m_debugTool ) {
      LHCb::LHCbID id = hit->id();
      return m_debugTool->matchKey( id, m_wantedKey );
    }
    return false;
  }

  void printHit( const PatPixelHit* hit, std::string title="" );
  void printHits ( PatPixelHits& hits,   std::string title="" );
  
private:
  std::string m_outputLocation;
  PatPixelHitManager* m_hitManager;

  double m_maxXSlope;
  double m_maxYSlope;
  double m_seedTolerance;
  double m_extraTol;
  double m_maxChi2ToAdd;
  double m_maxChi2SameSensor;
  int    m_maxMissed;
  double m_maxChi2Short;

  PatPixelTracks m_tracks;
  
  //== Debug control
  std::string      m_debugToolName;
  int              m_wantedKey;
  IPatDebugTool*   m_debugTool;
  bool             m_isDebug;
  bool             m_debug;

  //== Timing measurement control
  bool             m_doTiming;
  ISequencerTimerTool* m_timerTool;
  int   m_timeTotal;
  int   m_timePrepare;
  int   m_timeDist4;
  int   m_timeDist6;
  int   m_timeDist7;
  int   m_timeDist5;
  int   m_timeDist3;
  int   m_timeFinal;

};
#endif // PATPIXELTRACKING_H
