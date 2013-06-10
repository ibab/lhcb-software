#ifndef NEWVELORTRACKING_H
#define NEWVELORTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "PatVeloRHitManager.h"
#include "PatRZTrack.h"
#include "PatKernel/IPatDebugTool.h"
#include "PatKernel/IPatDebugTrackTool.h"

#include "GaudiAlg/ISequencerTimerTool.h"

/** @class NewVeloRTracking NewVeloRTracking.h
 *
 *
 *  @author Olivier Callot
 *  @date   2009-08-28
 */
class NewVeloRTracking : public GaudiAlgorithm {
public:
  /// Standard constructor
  NewVeloRTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~NewVeloRTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  typedef Tf::PatVeloRHit::range_type RHitRange;                                 ///< shortcut to range of r hits
  typedef Tf::PatVeloRHitManager::StationIterator        StationIterator;        ///< shortcut for station iterator
  typedef Tf::PatVeloRHitManager::StationReverseIterator StationReverseIterator; ///< shortcut for reverse station iterator

  /** Take three sectors and set up the combinations to pass to
      seedInSectorTriplet.
  */
  void makeForwardSectorTriplets(  StationReverseIterator station0, unsigned int zone0,
                                   StationReverseIterator station1, unsigned int zone1,
                                   StationReverseIterator station2, unsigned int zone2,
                                   StationReverseIterator rStationsEnd,
                                   unsigned int sector,
                                   std::vector<Tf::PatRZTrack> &rzTracks,
                                   unsigned int iter );

  /** find using 3 sector pointers seeds tracks into rzTracks
      also extends tracks from initial triplet, into opposite side
      for forward tracks
  */
  void seedInSectorTriplet( bool forward,
                            std::vector<Tf::PatRZTrack> &rzTracks,
                            const int &iCase, unsigned int sector,
                            StationIterator station0, unsigned int zone0,
                            StationIterator station1, unsigned int zone1,
                            StationIterator station2, unsigned int zone2,
                            unsigned int iter = 0);

  /** extend a tracks into more sectors, starting with sectPt
      if extraCoord==0 add hits into track, otherwise return new hits
      in extraCoord
      returns number of missed stations before track exits acceptance at low r
  */
  int extendTrack( Tf::PatRZTrack &newTrack,
                   StationIterator station, unsigned int zone,
                   unsigned int &nbUsed,
                   double tol,
                   bool forward,
                   Tf::PatVeloRHits *extraCoord);

  /// add the hits from the other side of the detector to a track
  void addOppositeSideHits( Tf::PatRZTrack &newTrack, const int &zone,
                            const DeVeloRType* sensor );

  /// Merge tracks sharing hits
  void mergeTracks( std::vector<Tf::PatRZTrack> &rzTracks );

  void printRCoords( Tf::PatVeloRHitManager::Station* station, int zone, std::string title ) {
    std::vector<Tf::PatVeloRHit*>::const_iterator itC;
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber()
           << " zone " << zone << " ..." << endmsg;
    for ( itC = station->hits(zone).begin(); station->hits(zone).end() != itC; ++itC ) {
      printCoord( *itC, " ");
    }
  }

  template<class TYPE> void printCoord( const TYPE* hit, std::string title ) {
    info() << "  " << title
           << format( " sensor %3d z%7.1f strip%5d coord%8.3f size%2d inter%5.2f high%2d used%2d ",
                      hit->sensorNumber(), hit->z(), hit->hit()->strip(),
                      hit->hit()->coordHalfBox(),
                      hit->hit()->cluster().pseudoSize(),
                      hit->hit()->cluster().interStripFraction(),
                      hit->hit()->cluster().highThreshold(),
                      hit->hit()->isUsed() );
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    if ( 0 != m_debugTool ) m_debugTool->printKey( info(), myId );
    if ( matchKey( hit ) ) info() << " ***";
    info() << endmsg;
  }

  template<class TYPE> bool matchKey( TYPE* coord ) {
    if ( 0 == m_debugTool ) return false;
    LHCb::LHCbID id = coord->hit()->lhcbID();
    return m_debugTool->matchKey( id, m_wantedKey );
  }

private:
  std::string m_debugToolName;
  int         m_wantedKey;

  std::string  m_outputLocation;    ///< Location of track container on the TES
  bool m_mergeTracks;               ///< If true, merge tracks sharung hits
  double m_zVertexMin;      ///< Minimal Z of a vertex for forward tracks
  double m_zVertexMax;      ///< Maximal Z of a vertex for backward tracks
  double m_maxRSlope;       ///< Maximum RZ slope considered
  double m_rMatchTol;       ///< R match tolerance in a triplet, in pitch unit
  double m_rExtraTol;       ///< R match tol. extrapolating, in pitch unit
  double m_rOverlapTol;     ///< R match in overlap, in pitch unit
  int m_maxMissed;          ///< Maximum number of consecutive missed sensors
  unsigned int m_minToTag;  ///< Minimum number of hit on track to tag as used
  int  m_nCommonToMerge;            ///< Num of shared hits to merge the tracks
  bool m_adjacentSectors;   

  double m_zSensorSearchMin;
  double m_zSensorSearchMax;
  bool m_onlyForward;
  bool m_onlyBackward;

  std::string m_rHitManagerName;      /// Name of the R hit manager instance
  DeVelo*                        m_velo; ///< Ponter to DeVelo object to get sensor numbers
  Tf::PatVeloRHitManager*        m_hitManager;
  IPatDebugTool*                 m_debugTool;
  bool                           m_isDebug;
  bool                           m_debug;

  bool                           m_measureTime;
  ISequencerTimerTool*           m_timer;
  int                            m_totalTime;

};
#endif // NEWVELORTRACKING_H
