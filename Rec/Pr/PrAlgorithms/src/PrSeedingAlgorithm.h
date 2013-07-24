#ifndef PRSEEDINGALGORITHM_H 
#define PRSEEDINGALGORITHM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrSeedQuadruplet.h"
#include "PrSeedTrack.h"
#include "PrGeometryTool.h"

/** @class PrSeedingAlgorithm PrSeedingAlgorithm.h
 *  Stand alone seeding fo rthe FT T stations
 *
 *  @author Olivier Callot
 *  @date   2012-06-18
 */
class PrSeedingAlgorithm : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrSeedingAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrSeedingAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void findQuadruplets( int zone );

  void tracksFromTriplets( int zone );

  void tracksFromPairs( int zone );

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

  bool matchKey( const PrSeedQuadruplet& quad ) {
    if ( !m_debugTool ) return false;
    for ( PrHits::const_iterator itH = quad.hits().begin(); quad.hits().end() != itH; ++itH ) {
      if (! m_debugTool->matchKey( (*itH)->id(), m_wantedKey ) ) return false;
    }
    return true;
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
  float           m_tolXQuad;
  float           m_tolVMatch;
  float           m_maxChi2InTrack;
  float           m_deltaSlopeQuadMatch;
  float           m_deltaXQuadMatch;
  float           m_deltaArrowQuadMatch;
  float           m_maxChi2Triplet;

  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;

  //== Debugging controls
  std::string     m_debugToolName;
  int             m_wantedKey;
  IPrDebugTool*   m_debugTool;

  std::vector<PrSeedTrack>       m_trackCandidates;
  std::vector<PrSeedQuadruplets> m_quadruplets;
};
#endif // PRSEEDINGALGORITHM_H
