#ifndef PRFORWARDTOOL_H 
#define PRFORWARDTOOL_H 1

// Include files
// from Gaudi
#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrKernel/PrHit.h"
#include "Event/Track.h"
#include "IPrForwardTool.h"
#include "PrGeometryTool.h"

/** @class PrForwardTool PrForwardTool.h
 *  Extend a Velo track to the T stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 */
class PrForwardTool : public IPrForwardTool {
public: 

  /// Standard constructor
  PrForwardTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~PrForwardTool( ); ///< Destructor

  virtual StatusCode initialize();

  void extendTrack( LHCb::Track* velo, LHCb::Tracks* result );

  void collectAllXHits( PrForwardTrack& track, unsigned int zone );

  void selectXCandidates( PrForwardTrack& track );

  bool fitXProjection( PrForwardTrack& track );

  bool addHitsOnEmptyXLayers( PrForwardTrack& track );

  void collectStereoHits( PrForwardTrack& track );

  bool fitStereoHits( PrForwardTrack& track );
  
  void makeLHCbTracks ( LHCb::Tracks* result );

  void fastLinearFit( PrForwardTrack& track );

protected:

private:
  std::string     m_hitManagerName;
  float           m_minPt;
  unsigned int    m_minXHits;
  float           m_tolY;
  float           m_tolYSlope;
  float           m_maxXWindow;
  float           m_maxXWindowSlope;
  float           m_maxXGap;
  float           m_maxChi2XProjection;
  float           m_maxChi2ToAddExtraXHits;
  float           m_maxChi2PerDoF;
  float           m_maxDxForY;
  unsigned int    m_minStereoHits;
  float           m_maxChi2Stereo;
  float           m_maxQuality;
  float           m_deltaQuality;
  float           m_yQualityWeight;
  unsigned int    m_nbXPlanes;

  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;
  bool            m_debug;

  // Working values
  float           m_minXSize;   // current value for the minimal number of X hits.
  PrHits          m_allXHits;
  PrHits          m_stereoHits;
  PrForwardTracks m_trackCandidates;
};
#endif // PRFORWARDTOOL_H
