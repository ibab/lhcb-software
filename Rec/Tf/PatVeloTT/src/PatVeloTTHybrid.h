// $Id: PatVeloTT.h,v 1.5 2007-10-10 18:42:24 smenzeme Exp $
#ifndef PATVELOTTHYBRID_H
#define PATVELOTTHYBRID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// from TrackInterfaces
#include "TrackInterfaces/ITracksFromTrack.h"
#include "Event/Track.h"

/** @class PatVeloTTHybrid PatVeloTTHybrid.h
   *
   *  Pat VeloTTHybrid algorithm. This is just a wrapper and contains the option to fit the VeloTT tracks with a Kalman fitter, 
   *  the actual pattern recognition is done in the 'PatVeloTTHybridTool'.
   *
   *  - InputTracksName: Input location for Velo tracks
   *  - OutputTracksName: Output location for VeloTT tracks  
   *  - TimingMeasurement: Do a timing measurement?
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
  
private:
  
  ITracksFromTrack* m_veloTTTool;                          ///< The tool that does the actual pattern recognition
  ISequencerTimerTool* m_timerTool;                        ///< Timing tool
  int  m_veloTTTime;                                       ///< Counter for timing tool
 
  std::string m_inputTracksName;                           ///< input container name
  std::string m_outputTracksName;                          ///< output container name
  
  bool m_doTiming;                                         ///< Measure timing of algorithm?
  
};

#endif // PATVELOTT_H
