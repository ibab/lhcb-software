#ifndef NEWVELOSPACETOOL_H
#define NEWVELOSPACETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface
#include "CircularRangeUtils.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloTrackTool.h"
#include "PatVeloSpaceTool.h"
#include "NewSpaceTrack.h"

#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "PatKernel/IPatDebugTool.h"
#include "PatKernel/IPatDebugTrackTool.h"

#include "GaudiAlg/ISequencerTimerTool.h"

/** @class NewVeloSpaceTool NewVeloSpaceTool.h
 *
 *
 *  @author Olivier Callot
 *  @date   2009-03-20
 */
class NewVeloSpaceTool : public extends<GaudiTool, ITracksFromTrack> {
public:
  /// Standard constructor
  NewVeloSpaceTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  ~NewVeloSpaceTool( ) override; ///< Destructor

  StatusCode initialize() override; ///< Initialize use base class

  /// take a Velo only Track without phi clusters and add phi clusters
  StatusCode tracksFromTrack(const LHCb::Track& seed,std::vector<LHCb::Track*>& out) const override;

private:
  void printRCoords( Tf::PatVeloRHitManager::Station* station, int zone, std::string title ) const {
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber()
           << " zone " << zone << " ..." << endmsg;
    for ( const auto& i : station->hits(zone)) printCoord( i, " ");
  }

  void printPhiCoords( Tf::PatVeloPhiHitManager::Station* station, int zone, std::string title ) const {
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber()
           << " zone " << zone << " ..." << endmsg;
    for (const auto& i : station->hits(zone)) printCoord( i, " ");
  }

  template<class TYPE> void printCoord( const TYPE* hit, std::string title ) const {
    info() << "  " << title
           << format( " sensor %3d z%7.1f strip%5d coord%10.5f phi%10.5f size%2d inter%5.2f high%2d used%2d ",
                      hit->sensorNumber(), hit->z(), hit->hit()->strip(),
                      hit->hit()->coordHalfBox(),
                      hit->referencePhi(),
                      hit->hit()->cluster().pseudoSize(),
                      hit->hit()->cluster().interStripFraction(),
                      hit->hit()->cluster().highThreshold(),
                      hit->hit()->isUsed() );
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    if ( m_debugTool ) m_debugTool->printKey( info(), myId );
    if ( matchKey( hit ) ) info() << " ***";
    info() << endmsg;
  }

  template<class TYPE> bool matchKey( TYPE* coord ) const {
    if ( !m_debugTool ) return false;
    LHCb::LHCbID id = coord->hit()->lhcbID();
    return m_debugTool->matchKey( id, m_wantedKey );
  }
  

  void getPhiSensorAndStep ( Tf::PatVeloSpaceTrack* track,
                             int& firstPhiSensor, int& lastPhiSensor, int& step ) const;
  
  void mergeClones( std::vector<NewSpaceTrack>& tracks ) const;

  bool addClustersToTrack ( std::vector<Tf::PatVeloPhiHit*>& hitList,
                            NewSpaceTrack& candidate ) const;
  
private:
  std::string m_debugToolName;
  std::string m_debugTrackToolName;
  mutable int m_wantedKey;
  bool        m_printTracks;
  std::string m_rHitManagerName;      /// Name of the R hit manager instance
  std::string m_phiHitManagerName;    /// Name of the Phi hit manager instance
  std::string m_trackToolName;        /// Name of the Track Tool used here

  double      m_phiAngularTol;        /// Angular tolerance to define the phi range
  double      m_phiMatchZone;         /// Size of the zone in phi for candidate search
  double      m_phiBigZone;           /// Size of the zone in phi for recovery of unused
  double      m_fractionFound;        /// Minimal fraction of Phi hits in tested sensors
  double      m_stepError;            /// Multiple scattering weight
  double      m_forwardStepError;     /// Multiple scattering weight in forward fit
  int         m_fullErrorPoints;      /// Number of points after wich MS is used
  double      m_maxChi2PerHit;        /// Maximum chi2 per hit in the fit
  double      m_maxChi2ToAdd;         /// Maximum chi2 per hit to add extra coordinates
  double      m_fractionForMerge;     /// Fraction of shared hits to detect clones.
  double      m_deltaQuality;         /// Allowed quality difference for final track selection
  double      m_fractionForDouble;    /// Fraction of high threshold hits to detect a double charged track
  double      m_maxQFactor;           /// Maximal value of teh quality of a track.

  DeVelo*                        m_velo; ///< Ponter to DeVelo object to get sensor numbers
  Tf::PatVeloPhiHitManager*      m_phiHitManager;
  Tf::PatVeloRHitManager*        m_rHitManager;
  Tf::PatVeloTrackTool*          m_trackTool;
  Tf::CircularRangeUtils<double> m_angleUtils = { -Gaudi::Units::pi,Gaudi::Units::pi };
  IPatDebugTool*                 m_debugTool;
  IPatDebugTrackTool*            m_debugTrackTool;
  mutable bool                   m_isDebug;
  mutable bool                   m_isVerbose;

  bool                           m_measureTime;
  ISequencerTimerTool*           m_timer;
  int                            m_totalTime;
  int                            m_convertTime;
  int                            m_prepareTime;
  int                            m_mainTime;
  int                            m_unusedTime;
  int                            m_storeTime;

};
#endif // NEWVELOSPACETOOL_H
