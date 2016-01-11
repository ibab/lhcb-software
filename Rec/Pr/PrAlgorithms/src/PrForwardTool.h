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

#include "TrackInterfaces/IPrAddUTHitsTool.h"

/** @class PrForwardTool PrForwardTool.h
 *  Extend a Velo track to the T stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 *  @modified Thomas Nikodem
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

  void fastLinearFit( PrForwardTrack& track );

  bool fitXProjection( PrForwardTrack& track );

  bool addHitsOnEmptyXLayers( PrForwardTrack& track, bool refit = true );

  void collectStereoHits( PrForwardTrack& track );

  bool fitStereoHits( PrForwardTrack& track );

  void makeLHCbTracks ( LHCb::Tracks* result );

protected:

private:

  /// Class to find lower bound of x of PrHit in an X layer
  class lowerBoundX {
  public:
    bool operator() (const PrHit* lhs, const float testval ) const { return lhs->x() < testval; }
  };
  
  /// Class to find lower bound of x of PrHit if in a uv layer
  class lowerBoundXYZone {
  public:
    lowerBoundXYZone(const float yZone): m_yZone(yZone){};
    bool operator() (const PrHit* lhs, const float testval ) const { return lhs->x(m_yZone) < testval; }
  private:
    float m_yZone;
  };

  std::string     m_hitManagerName;
  std::string     m_addUTHitsToolName;
  float           m_minPt;
  unsigned int    m_minXHits;
  float           m_tolY;
  float           m_tolYSlope;
  float           m_maxXWindow;
  float           m_maxXWindowSlope;
  float           m_maxXGap;
  float           m_maxChi2LinearFit;
  float           m_maxChi2XProjection;
  float           m_maxChi2PerDoF;
  float           m_minYGap;
  float           m_tolYMag;
  float           m_tolYMagSlope;
  float           m_maxYChi2AtMagnet;
  unsigned int    m_minTotalHits;
  float           m_maxChi2StereoLinear;
  float           m_maxChi2Stereo;
  float           m_maxQuality;
  float           m_deltaQuality;
  float           m_yQualityWeight;
  unsigned int    m_nbXPlanes;
  unsigned int    m_minStereoHits;
  float           m_yTol_UVsearch;
  bool            m_useTFix;    

  IPrAddUTHitsTool*  m_addUTHitsTool;
  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;
  bool            m_debug;

  //Momentum estimate
  bool   m_useMomentumEstimate;
  bool m_Preselection;
  float m_PreselectionPT;

  bool m_useWrongSignWindow;
  float m_wrongSignPT;
  
  // Working values
  float           m_minXSize;   // current value for the minimal number of X hits.
  PrHits          m_allXHits;
  PrHits          m_stereoHits;
  PrForwardTracks m_trackCandidates;
};
#endif // PRFORWARDTOOL_H
