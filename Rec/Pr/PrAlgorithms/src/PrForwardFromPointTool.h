#ifndef PRFORWARDFROMPOINTTOOL_H 
#define PRFORWARDFROMPOINTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrKernel/PrHit.h"
#include "Event/Track.h"

#include "IPrForwardTool.h"
#include "PrGeometryTool.h"

/** @class PrForwardFromPointTool PrForwardFromPointTool.h
 *  Extend a Velo track to the T stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 */
class PrForwardFromPointTool : public IPrForwardTool {
public:

  /// Standard constructor
  PrForwardFromPointTool( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PrForwardFromPointTool( ); ///< Destructor

  virtual StatusCode initialize();

  virtual void extendTrack( LHCb::Track* velo, LHCb::Tracks* result );

  void findSeedsInStation( PrForwardTrack& track, unsigned int zone, int station );

  void extendTracks( int seedStation, unsigned int side );
  
  bool fitXProjection( PrForwardTrack& track );

  void fitStereoHits( PrForwardTrack& track );

  void setChi2( PrForwardTrack& track );

  void makeLHCbTracks ( LHCb::Tracks* result );

  void fastLinearFit( PrForwardTrack& track );

protected:

private:
  std::string     m_hitManagerName;
  unsigned int    m_minXHits;
  float           m_maxChi2XProjection;
  float           m_maxChi2ToAddExtraXHits;
  unsigned int    m_minStereoHits;
  float           m_deltaMeanDy;
  float           m_maxChi2Stereo;

  float m_tolXSeed;
  float m_tolXSeedSlope;
  float m_tolUMatch;
  float m_tolUMatchSlopeX;
  float m_tolUMatchSlopeY;
  float m_tolVMatch;
  float m_maxChi2Seed;
  float m_maxDistanceToAdd;
  float m_maxChi2Extra;
  
  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;
  bool            m_debug;

  // Working values
  PrForwardTracks m_trackCandidates;
};
#endif // PRFORWARDFROMPOINTTOOL_H
