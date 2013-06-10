#ifndef PATVELOOPENTRACKING_H
#define PATVELOOPENTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "PatKernel/IPatDebugTool.h"

#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "PatVeloTrackTool.h"

// Local


/** @class PatVeloOpenTracking PatVeloOpenTracking.h
 *
 *
 *  @author Olivier Callot
 *  @date   2007-02-27
 */
class PatVeloOpenTracking : public GaudiAlgorithm {
public:
  /// Standard constructor
  PatVeloOpenTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatVeloOpenTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:
  
  void printRCoords( Tf::PatVeloRHitManager::Station* station, int zone, std::string title ) {
    std::vector<Tf::PatVeloRHit*>::const_iterator itC;
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber() 
           << " zone " << zone << " ..." << endmsg;
    for ( itC = station->hits(zone).begin(); station->hits(zone).end() != itC; ++itC ) {
      printCoord( *itC, " ");
    }
  }
  
  void printPhiCoords( Tf::PatVeloPhiHitManager::Station* station, int zone, std::string title ) {
    std::vector<Tf::PatVeloPhiHit*>::const_iterator itC;
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber() 
            << " zone " << zone << " ..." << endmsg;
    for ( itC = station->hits(zone).begin(); station->hits(zone).end() != itC; ++itC ) {
      printCoord( *itC, " ");
    }
  }
  
  template<class TYPE> void printCoord( const TYPE* hit, std::string title ) {
    info() << "  " << title 
           << format( " sensor %3d z%7.1f strip%5d coord%10.5f used%2d ", 
                      hit->sensorNumber(), hit->z(), hit->hit()->strip(), 
                      hit->hit()->coordHalfBox(), hit->hit()->isUsed() );
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    if ( 0 != m_debugTool ) m_debugTool->printKey( info(), myId );
    info() << endmsg;
  }
  
  template<class TYPE> bool matchKey( TYPE* coord ) {
    if ( 0 == m_debugTool ) return false;
    LHCb::LHCbID id = coord->hit()->lhcbID();
    return m_debugTool->matchKey( id, m_wantedKey );
  }
  
  void matchShared ( PatVeloOpenTrack& track, 
                     std::vector<PatVeloOpenTrack>& tracks);

  void tagManyUsed( PatVeloOpenTrack& track );

private:
  Tf::PatVeloRHitManager*   m_rHitManager;
  Tf::PatVeloPhiHitManager* m_phiHitManager;
  Tf::PatVeloTrackTool*     m_trackTool;
  IPatDebugTool*            m_debugTool;

  DeVelo* m_velo;            ///< Detector element

  std::string m_debugToolName;
  int    m_wantedKey;

  double m_rMatchTol;
  double m_rExtrapTol;
  double m_phiMatchTol;
  double m_phiExtrapTol;
  double m_phiAngularTol;
  double m_maxSlope;
  double m_maxChi2;
  int    m_maxMissedR;

  std::string m_rHitManagerName; ///< name of the R hit manager instance
  std::string m_phiHitManagerName; ///< name of the Phi hit manager instance
  std::string m_trackToolName; /// name of the track tool instance used here
};
#endif // PATVELOOPENTRACKING_H
