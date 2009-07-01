// $Id: NewVeloSpaceTool.h,v 1.2 2009-07-01 14:35:52 ocallot Exp $
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

/** @class NewVeloSpaceTool NewVeloSpaceTool.h
 *
 *
 *  @author Olivier Callot
 *  @date   2009-03-20
 */
class NewVeloSpaceTool : public GaudiTool, virtual public ITracksFromTrack {
public:
  /// Standard constructor
  NewVeloSpaceTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual ~NewVeloSpaceTool( ); ///< Destructor

  StatusCode initialize(); ///< Initialize use base class

  /// take a Velo only Track without phi clusters and add phi clusters
  virtual StatusCode tracksFromTrack(const LHCb::Track& seed,
                                     std::vector<LHCb::Track*>& tracks );

protected:
  void printRCoords( Tf::PatVeloRHitManager::Station* station, int zone, std::string title ) {
    std::vector<Tf::PatVeloRHit*>::const_iterator itC;
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber()
           << " zone " << zone << " ..." << endreq;
    for ( itC = station->hits(zone).begin(); station->hits(zone).end() != itC; ++itC ) {
      printCoord( *itC, " ");
    }
  }

  void printPhiCoords( Tf::PatVeloPhiHitManager::Station* station, int zone, std::string title ) {
    std::vector<Tf::PatVeloPhiHit*>::const_iterator itC;
    info() << " ... coordinates in " << title << " sensor " << station->sensor()->sensorNumber()
           << " zone " << zone << " ..." << endreq;
    for ( itC = station->hits(zone).begin(); station->hits(zone).end() != itC; ++itC ) {
      printCoord( *itC, " ");
    }
  }

  template<class TYPE> void printCoord( const TYPE* hit, std::string title ) {
    info() << "  " << title
           << format( " sensor %3d z%7.1f strip%5d coord%10.5f phi%10.5f size%2d inter%5.2f used%2d ",
                      hit->sensorNumber(), hit->z(), hit->hit()->strip(),
                      hit->hit()->coordHalfBox(),
                      hit->referencePhi(),
                      hit->hit()->cluster().pseudoSize(),
                      hit->hit()->cluster().interStripFraction(),
                      hit->hit()->isUsed() );
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    if ( 0 != m_debugTool ) m_debugTool->printKey( info(), myId );
    if ( matchKey( hit ) ) info() << " ***";
    info() << endreq;
  }

  template<class TYPE> bool matchKey( TYPE* coord ) {
    if ( 0 == m_debugTool ) return false;
    LHCb::LHCbID id = coord->hit()->lhcbID();
    return m_debugTool->matchKey( id, m_wantedKey );
  }
  
  class increasingByReferencePhi {
    public:
    bool operator() ( const Tf::PatVeloPhiHit* h1, const Tf::PatVeloPhiHit* h2 ) const {
      return h1->referencePhi() < h2->referencePhi();
    }
  };

  void getPhiSensorAndStep ( Tf::PatVeloSpaceTrack* track,
                             int& firstPhiSensor, int& lastPhiSensor, int& step );
  
  void mergeClones( std::vector<NewSpaceTrack>& tracks );
  
private:
  std::string m_debugToolName;
  std::string m_debugTrackToolName;
  int         m_wantedKey;
  bool        m_printTracks;
  std::string m_rHitManagerName;      /// Name of the R hit manager instance
  std::string m_phiHitManagerName;    /// Name of the Phi hit manager instance
  std::string m_trackToolName;        /// Name of the Track Tool used here

  double      m_phiAngularTol;        /// Angular tolerance to define the phi range
  double      m_phiMatchZone;         /// Size of the zone in phi for candidate search
  double      m_phiMatchZoneSlope;    /// Dependance in dz of the matchzone value.
  double      m_fractionFound;        /// Minimal fraction of Phi hits in tested sensors
  double      m_stepError;            /// Multiple scattering weight
  double      m_forwardStepError;     /// Multiple scattering weight in forward fit
  int         m_fullErrorPoints;      /// Number of points after wich MS is used
  double      m_maxChi2PerHit;        /// Maximum chi2 per hit
  double      m_fractionForMerge;     /// Fraction of shared hits to detect clones.

  DeVelo*                        m_velo; ///< Ponter to DeVelo object to get sensor numbers
  Tf::PatVeloPhiHitManager*      m_phiHitManager;
  Tf::PatVeloRHitManager*        m_rHitManager;
  Tf::PatVeloTrackTool*          m_trackTool;
  Tf::CircularRangeUtils<double> m_angleUtils;
  IPatDebugTool*                 m_debugTool;
  IPatDebugTrackTool*            m_debugTrackTool;
  bool                           m_isDebug;
  bool                           m_isVerbose;
};
#endif // NEWVELOSPACETOOL_H
