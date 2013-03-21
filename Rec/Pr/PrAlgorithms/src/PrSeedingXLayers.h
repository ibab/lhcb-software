#ifndef PRSEEDINGXLAYERS_H 
#define PRSEEDINGYLAYERS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrSeedTrack.h"
#include "PrGeometryTool.h"

/** @class PrSeedingXLayers PrSeedingXLayers.h
 *  Stand alone seeding for the FT T stations
 *
 *  @author Olivier Callot
 *  @date   2013-02-14
 *  2013-03-21 : Yasmine Amhis Modification 
 */
class PrSeedingXLayers : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrSeedingXLayers( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrSeedingXLayers( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void findXProjections( unsigned int zone );

  void addStereo( unsigned int zone );

  bool fitTrack( PrSeedTrack& track );

  bool removeWorstAndRefit( PrSeedTrack& track );

  void setChi2( PrSeedTrack& track );

  void makeLHCbTracks( LHCb::Tracks* result );

  void printHit( const PrHit* hit, std::string title="" );

  void printTrack( PrSeedTrack& track );

  bool matchKey( const PrHit* hit ) {
    if ( m_debugTool ) return m_debugTool->matchKey( hit->id(), m_wantedKey );
    return false;
  };

  bool matchKey( const PrSeedTrack& track ) {
    if ( !m_debugTool ) return false;
    for ( PrHits::const_iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      if ( m_debugTool->matchKey( (*itH)->id(), m_wantedKey ) ) return true;
    }
    return false;
  };

private:
  std::string     m_inputName;
  std::string     m_outputName;
  std::string     m_hitManagerName;
  float           m_maxChi2InTrack;
  float           m_maxIpAtZero;
  float           m_tolXInf;
  float           m_tolXSup;
  unsigned int    m_minXPlanes;
  float           m_maxChi2PerDoF;
  bool            m_xOnly;
  
  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;

  //== Debugging controls
  std::string     m_debugToolName;
  int             m_wantedKey;
  IPrDebugTool*   m_debugTool;
  bool            m_debug;

  std::vector<PrSeedTrack>       m_trackCandidates;
  std::vector<PrSeedTrack>       m_xCandidates;

  bool           m_doTiming;
  ISequencerTimerTool* m_timerTool;
  int            m_timeTotal;
  int            m_timeFromForward;
  int            m_timeXProjection;
  int            m_timeStereo;
  int            m_timeFinal;
};
#endif // PRSEEDINGALGORITHM_H
