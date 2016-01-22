#ifndef PRSEEDINGXLAYERS_H 
#define PRSEEDINGYLAYERS_H 1

// Include files
// from Gaudi

//#define DEBUG_HISTO
//uncomment this line if you want to print monitorin histograms

#ifdef DEBUG_HISTO
#include "GaudiAlg/GaudiTupleAlg.h"
#else
#include "GaudiAlg/GaudiAlgorithm.h"
#endif

#include "GaudiAlg/ISequencerTimerTool.h"
#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrSeedTrack.h"
#include "PrGeometryTool.h"
#include "TfKernel/RecoFuncs.h"

//#include "PrKernel/IPrDebugTool.h"
//#include "PrMCTools/PrTStationDebugTool.h"

/** @class PrSeedingXLayers PrSeedingXLayers.h
 *  Stand alone seeding for the FT T stations
 *  This code is a hack which represents the code used for the upgrade tracker TDR
 *  It needs to be superseded by a proper implementation!
 *
 * - InputName: Name of the input container for the forward tracks. Set to '""' to not reuse the FT part of the forward tracks.
 * - OutputName: Name of the output container
 * - HitManagerName: Name of the hit manager
 * - DecodeData: Decode the data (default: false, as normally done in the Forward Tracking)
 * - XOnly: Only reconstruct tracks with the x-layers?
 * - MaxChi2InTrack: Max Chi2 value in internal track fit.
 * - TolXInf: Lower bound for search "road"
 * - TolXSup: Upper bound for search "road"
 * - MinXPlanes: Minimum number of x-planes a track needs to have
 * - MaxChi2PerDoF: Maximum Chi2/nDoF a track can have.
 * - MaxParabolaSeedHits: Maximum number of hits which are use to construct a parabolic search window.
 * - TolTyOffset: Tolerance for the offset in y for adding stereo hits.
 * - TolTySlope: Tolerance for the slope in y for adding stereo hits.
 * - MaxIpAtZero: Maximum impact parameter of the track when doing a straight extrapolation to zero. Acts as a momentum cut.
 * - To Add New Params
 * - DebugToolName: Name of the debug tool
 * - WantedKey: Key of the particle which should be studied (for debugging).
 * - TimingMeasurement: Do timing measurement and print table at the end (?).
  * - PrintSettings: Print all values of the properties at the beginning?
 *
 *  @author Olivier Callot
 *  @date   2013-02-14
 *  2013-03-21 : Yasmine Amhis Modification
 *  2014-02-12 : Michel De Cian (TDR version) 
 *  2014-24-11 : Renato Quagliani
 */
#ifdef DEBUG_HISTO 
class PrSeedingXLayers : public GaudiHistoAlg {
#else
class PrSeedingXLayers : public GaudiAlgorithm {
#endif
public:
    /// Standard constructor
    PrSeedingXLayers( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~PrSeedingXLayers( ); ///< Destructor
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
  protected:
  int matchXCandidate( PrSeedTrack& track );

  /** @brief Collect hits in the x-layers. Obsolete, just kept for documentation
   *  @param part lower (1) or upper (0) half
   */
  void findXProjections( unsigned int part );
  
  /** @brief Collect hits in the stereo-layers. Obsolete, just kept for documentation
   *  @param part lower (1) or upper (0) half
   */
  void addStereo( unsigned int part );
  
  /** @brief Fit the track with a parabola
   *  @param track The track to fit
   *  @return bool Success of the fit
   */
  bool fitTrack( PrSeedTrack& track );

  /** @brief Fit the track combining the XZ and YZ projections                                                                                             
   *  @param track The track to fit                                                                                                                        
   *  @param Refit Iteration in the Refitting after removal worst hit                                                                                      
   *  @return bool Success of the XY+XZ Fit                                                                                                                
   **/
  bool fitSimultaneouslyXY( PrSeedTrack& track, int refit=0 );

  /** @brief Fit the track combining the only in the XZ plane                                                                                              
   *  @param track The track to fit                                                                                                                        
   *  @param Refit Iteration in the Refitting after removal worst hit                                                                                      
   *  @return bool Success of the XZ Fit                                                                                                                   
   **/
  bool fitXProjection( PrSeedTrack & track );

  /** @brief Fit the track combining the only in the YZ plane                                                                                              
   *  @param xtrack candidate
   *
   *  @param itBeg iterator to first UV hit                                                                                                                       
   *  @param itEnd iterator to last UV hit                                                                                                                       
   *  @return bool Success of the YZ Fit                                                                                                                   
   **/
  bool fitYLine(const PrSeedTrack& track, PrHits::const_iterator itBeg, PrHits::const_iterator itEnd, float& ay, float& by);


  /** @brief Remove the hit which gives the largest contribution to the chi2 and refit
   *  @param track The track to fit
   *  @return bool Success of the fit
   */


  bool removeWorstAndRefit( PrSeedTrack& track, bool xonlyFit=false );
  
  /** @brief Set the chi2 of the track
   *  @param track The track to set the chi2 of 
   */
  void setChi2( PrSeedTrack& track );

  /** @brief Transform the tracks from the internal representation into LHCb::Tracks
   *  @param tracks The tracks to transform
   */
  void makeLHCbTracks( LHCb::Tracks* result );

  /** @brief Print some information of the hit in question
   *  @param hit The hit whose information should be printed
   *  @param title Some additional information to be printed
   */
  void printHit( const PrHit* hit, std::string title="" );

  /** @brief Print some information of the track in question
   *  @param hit The track whose information should be printed
   */
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
  
  void setKey( int key ) {
    m_wantedKey = key;
    return;
  };

  
  /** @brief Collect hits in the x-layers using a parabolic search window.
   *  @param part lower (1) or upper (0) half
   */
  void findXProjections2( unsigned int part );
  
  /** @brief Collect hits in the stereo-layers.
   *  @param part lower (1) or upper (0) half
   */
  void addStereo2( unsigned int part );

  /** @brief Internal method to construct parabolic parametrisation out of three hits, using Cramer's rule.
   *  @param hit1 First hit
   *  @param hit2 Second hit
   *  @param hit3 Third hit
   *  @param a quadratic coefficient
   *  @param b linear coefficient
   *  @param c offset
   */
  void solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, float& a, float& b, float& c);

  //classes to find number of inner modules a track passes 
  int innerMod(const PrSeedTrack& track);
  int innerMod(PrHits thesehits);
  bool addEmptyLayers(  unsigned int part,  PrSeedTrack& track);

  
  /// Class to find lower bound of x of PrHit
  class lowerBoundX {
  public:
    bool operator() (const PrHit* lhs, const float testval ) const { return lhs->x() < testval; }
  };

  /// Class to find upper bound of x of PrHits
  class upperBoundX {
  public:
    bool operator() (const double testval, const PrHit* rhs) const { return testval < rhs->x(); }
  };

  /// Class to compare x positions of PrHits
  class compX {
  public:
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->x() < rhs->x(); }
  };

  /// Class to find lower bound of LHCbIDs of PrHits
  class lowerBoundLHCbID {
  public:
    bool operator() (const PrHit* lhs, const LHCb::LHCbID id ) const { return lhs->id() < id; }
  };
  
  /// Class to compare LHCbIDs of PrHits
  class compLHCbID {
  public:
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->id() < rhs->id(); }
  };

  
  
private:
  std::string     m_inputName;
  std::string     m_outputName;
  std::string     m_hitManagerName;
  unsigned int    m_minXPlanes;
  unsigned int    m_minSPlanes;
  unsigned int    m_minTPlanes;
  unsigned int    m_maxParabolaSeedHits;

  bool            m_useFix; //added  
  bool            m_decodeData;
  bool            m_printSettings;
  bool            m_xOnly;
  
  bool            m_useCubic;
  bool            m_useCorrPos;
  bool            m_SlopeCorr;           
  bool            m_removeHole;

  float           m_radiusHole;
  float           m_Chi2LowLine;
  float           m_Chi2HighLine;
  float           m_maxChi2InTrack;
  float           m_maxIpAtZero;
  float           m_maxIpAtZeroIN;
  float           m_tolXInf;
  float           m_tolXSup;
  float           m_maxChi2PerDoF;
  float           m_bestDist;
  float           m_tolXStereo; //added
  float           m_tolXStereoIN; //added
  float           m_tolXStereoTriangle; //added
  float           m_coord; //added
  float           m_tolTyOffset;
  float           m_tolTySlope;
  float           m_tolYOffset;//added(debug)
  float           m_tolTriangle; //added(debug)


  float          m_zReference;
  float          m_dRatio;
  float          m_invDeltaz[3];
  float          m_zRatio[3];
  //--------_Fit X parametrisation                                                                                                                         
  float          m_maxChi2HitsX;
  float          m_maxChi2HitsY;
  //--------_Full Fit parametrisation                                                                                                                      
  float          m_maxChi2HitFullFitHigh;
  float          m_maxChi2HitFullFitLow;
  float          m_maxY0Low;
  float          m_maxYZrefLow;


  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;

  //== Debugging controls
  std::string     m_debugToolName;
  int             m_wantedKey;
  IPrDebugTool*   m_debugTool;
  //PrTStationDebugTool* m_debugTool;
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
