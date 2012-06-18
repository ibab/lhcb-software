#ifndef PRFORWARDTOOL_H 
#define PRFORWARDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrKernel/PrHit.h"
#include "Event/Track.h"

#include "PrGeometryTool.h"
static const InterfaceID IID_PrForwardTool ( "PrForwardTool", 1, 0 );

/** @class PrForwardTool PrForwardTool.h
 *  Extend a Velo track to the T stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 */
class PrForwardTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrForwardTool; }

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
  
  void setChi2( PrForwardTrack& track );

  void makeLHCbTracks ( LHCb::Tracks* result );

  bool matchKey( const PrHit* hit ) {
    if ( m_debugTool ) return m_debugTool->matchKey( hit->id(), m_wantedKey );
    return false;
  };
  
  void printHit( const PrHit* hit, std::string prefix = "" );

  void printTrack( const PrForwardTrack& track );
  
  void printHitsForTrack( const PrHits& hits, const PrForwardTrack& track );
  
  void setDebugParams( IPrDebugTool* tool, int key, int veloKey ) { 
    m_debugTool = tool;
    m_wantedKey = key;
    m_veloKey   = veloKey;
  }

  void fastLinearFit( PrForwardTrack& track );
  
protected:

private:
  std::string     m_hitManagerName;
  float           m_minPt;
  unsigned int    m_minXHits;
  float           m_maxXWindow;
  float           m_maxXWindowSlope;
  float           m_maxXGap;
  float           m_maxChi2XProjection;
  float           m_maxChi2ToAddExtraXHits;
  float           m_maxDxForY;
  unsigned int    m_minStereoHits;
  float           m_deltaMeanDy;
  float           m_maxChi2Stereo;

  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;
  //== Debug controls
  IPrDebugTool*   m_debugTool;
  int             m_wantedKey;
  int             m_veloKey;
  bool            m_debug;

  // Working values
  PrHits          m_allXHits;
  PrHits          m_stereoHits;
  PrForwardTracks m_trackCandidates;
};
#endif // PRFORWARDTOOL_H
