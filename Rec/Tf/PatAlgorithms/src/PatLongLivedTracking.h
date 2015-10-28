#ifndef PATLONGLIVEDTRACKING_H 
#define PATLONGLIVEDTRACKING_H 1

// Include files
#include <string>
#include <vector>
#include "GaudiKernel/Point3DTypes.h"
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"

#include "Event/STCluster.h"

// forward declarations
class IPatDebugTTTool;
class DeSTDetector;
class PatDownTrack;

namespace LHCb {
  class Track;
}

/** @class PatLongLivedTracking PatLongLiveTracking.h
 *  Algorithm to reconstruct tracks with seed and TT, for daughters of long lived particles like Kshort, Lambda, ...
 *  Partial reimplementation of PatDownstream
 *
 * Parameters:
 * - InputLocation: Input location of seed tracks
 * - OutputLocation: Output location of downstream tracks.
 * - ForwardLocation: Location of forward tracks, for hit rejection
 * - MatchLocation: Location of match tracks, for seed rejection
 * - XPredTolConst: x-window for preselection is XPredTolConst/p + XPredTolOffset
 * - XPredTolOffset: x-window for preselection is XPredTolConst/p + XPredTolOffset
 * - TolMatchConst: x-window for matching x hits is TolMatchConst/p + TolMatchOffset
 * - TolMatchOffset: x-window for matching x hits is TolMatchConst/p + TolMatchOffset
 * - MaxWindowSize: maximum window size for matching x hits
 * - TolUConst: window for u hits is TolUConst/p + TolUOffset
 * - TolUOffset: window for u hits is TolUConst/p + TolUOffset
 * - TolVConst: window for v hits is TolVConst/p + TolVOffset
 * - TolVOffset: window for v hits is TolVConst/p + TolVOffset
 * - MaxChi2: Maximum chi2 for tracks with at least 4 hits
 * - MaxChi2ThreeHits: Maximum chi2 for tracks with 3 hits
 * - MinPt: Minimum pT of the track     
 * - MinMomentum: Minimum momentum of the track
 * - MinTTx: half-width  of beampipe rectangle
 * - MinTTy: half-height of of beampipe rectangle
 * - ZMagnetParams: Parameters to determine the z-position of the magnet point. Tune with PatKsParams.
 * - MomentumParams: Parameters to determine the momentum. Tune with PatKsParams.
 * - YParams: Parameters to determine the bending in y.  Tune with PatKsParams.
 * - ZTT: z-position of middle of TT.   
 * - ZTTa: z-position of first TT station
 * - MaxDeltaPConst: Window for deltaP is: MaxDeltaPConst/p + MaxDeltaPOffset
 * - MaxDeltaPOffset: Window for deltaP is: MaxDeltaPConst/p + MaxDeltaPOffset
 * - XCorrectionConst: Correction for x-position of search window in preselection is XCorrectionConst/p + XCorrestionOffset
 * - XCorrectionOffset: Correction for x-position of search window in preselection is XCorrectionConst/p + XCorrestionOffset
 * - MaxXTracks: Maximum number of x-tracklets to process further
 * - MaxChi2DistXTracks: Maximum chi2 difference to x-tracklet with best chi2
 * - MaxXUTracks:  Maximum number of xu-tracklets to process further
 * - MinChi2XProjection: Maximum chi2 of x-tracklet
 * - OverlapTol: Tolerance for adding overlap hits
 * - YTol: YTolerance for adding / removing hits.
 * - SeedCut: Cut on Fisher-discriminant to reject bad seed tracks.
 * - StateErrorX2: Error^2 on x-position (for making LHCb::Track)
 * - StateErrorY2: Error^2 on y-position (for making LHCb::Track)
 * - StateErrorTX2: Error^2 on tx-slope (for making LHCb::Track)
 * - StateErrorTY2: Error^2 on ty-slope (for making LHCb::Track)
 * - StateErrorP:  Error^2 on momentum (for making LHCb::Track)
 * - RemoveUsed: Remove seed tracks and used TT hits (with chi2-cut on long track)?
 * - RemoveAll: Remove seed tracks and used TT hits (withoug chi2-cut on long track)?
 * - LongChi2: Chi2-cut for the removal 
 * - SeedKey: Key of MCParticle for debugging
 * - WithDebugTool: Use the debug tool? 
 * - DebugTool: Name of the debug tool  
 * - PrintTracks: Print the tracks?     
 * - TimingMeasurement: Do timing measurements? 
 *
 *  @author Michel De Cian. Sascha Stahl and Olivier Callot for the original PatDownstream
 *  @date   2015-06-03
 *
 *
 */

class PatLongLivedTracking : public GaudiAlgorithm {

public: 
  /// Standard constructor
  PatLongLivedTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatLongLivedTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

 
    
  
private:
  
  void ttCoordCleanup();  ///< Tag already used coordinates
  
  void prepareSeeds(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks); ///< Tag already used T-Seeds
    
  void getPreSelection( PatDownTrack& track ); ///< Get a preselection of hits around a first track estimate

  void fitAndRemove( PatDownTrack& track, bool onlyFit ); ///< Perform a chi2 fit to the track and remove outliers
  
  void findMatchingHits( const PatDownTrack& track, const int plane ); ///< Find x hits matching the first track estimate

  void storeTrack( PatDownTrack& track, LHCb::Tracks* finalTracks, LHCb::Track* tr ); ///< Store the track as LHCb::Track
  
  void addUHits( const PatDownTrack& track, const unsigned int maxNumTracks ); ///< Add hits in u layer

  void addVHits( PatDownTrack& track ); ///< Add hits in v layer. The hit with the best chi2 is taken.

  void tagUsedTT( const LHCb::Track* tr ); ///< Tag hits that were already used elsewhere

  void fitXProjection( PatDownTrack& track, PatTTHit* firstHit ); ///< Fit the different x candidates and take the best ones (according to chi2)
  
  bool acceptCandidate( PatDownTrack& track, int& maxPoints ); ///< Is this candidate good enough?

  void addOverlapRegions( PatDownTrack& track ); ///< Add hits in the overlap regions

  double evaluateFisher( const LHCb::Track* track );///< Evaluate the Fisher discriminant for a preselection of seed tracks
  
  void initEvent();///< Init the event: Put hits in local containers which are sorted

  void xFit( PatDownTrack& track ); ///< Fit tracklet with hits in x layers only.

  /// Helper class to find lower bound of collection of hits
  class lowerBoundX {
  public:
    bool operator() (const PatTTHit* lhs, const double testVal){ return lhs->x() < testVal; }
  };
  
  // -- TT hits don't need the correction for the differnce in the reference frame, this should be a bit faster
  // -- than the original implementation
  inline void updateTTHitForTrackFast(PatTTHit* hit, const double y0, const double dyDz){
    const auto y  = ( y0 + dyDz * hit->z() );
    hit->setX( hit->hit()->x(y) ); 
  }
  
  /// Does this track point inside the beampipe?  
  bool insideBeampipe(const PatDownTrack& track){
    return (m_minTTx > fabs( track.xAtZ( m_zTTa ) )) && (m_minTTy > fabs( track.yAtZ( m_zTTa ) ));
  }

  /// Add to nStereo if hit is in a stereo layer
  inline bool addIsStereo(const PatTTHit* hit) const{
    return m_addIsStereoHelper[hit->planeCode()];
  }
  
  /// Helper to evaluate the Fisher discriminant
  inline double getFisher(const std::array<double,5> vals){
    double fishVal =  m_fishConst;
    for(int i = 0; i < 5; i++) fishVal += vals[i]*m_fishCoefficients[i];
    return fishVal;
  }
  
  /// Helper to evaluate the maximum discrepancy between momentum from kink and curvature in T-stations
  inline double maxDeltaP( const PatDownTrack& track ){
    return m_maxDeltaPConst/std::abs( track.momentum() ) + m_maxDeltaPOffset;
  }
  
  /// Helper to evaluate the correction to the x position in the TT
  inline double xPosCorrection( const PatDownTrack& track ){
    return std::copysign( m_xCorrectionOffset + m_xCorrectionConst/std::abs( track.momentum() ), track.momentum() );
  }
  
  
  // -- timing information
  int           m_downTime;
  int           m_preselTime;
  int           m_findMatchingHitsTime;
  int           m_fitXProjectionTime;
  int           m_fitAndRemoveTime;
  int           m_xFitTime;
  int           m_addUHitsTime;
  int           m_addVHitsTime;
  int           m_acceptCandidateTime;
  int           m_storeTrackTime;
  int           m_overlapTime;
  
  // -- other member variables which are not settable
  bool          m_printing; 
  bool          m_magnetOff;
   
  
  // -- pointers to tools
  ILHCbMagnetSvc*                     m_magFieldSvc; 
  Tf::TTStationHitManager<PatTTHit>*  m_ttHitManager;
  IPatDebugTTTool*                    m_debugTool;
  ISequencerTimerTool*                m_timerTool;

  // -- settable quantities
  std::string   m_inputLocation;                                                
  std::string   m_outputLocation;                                               
  std::string   m_forwardLocation;                                              
  std::string   m_matchLocation;                                                
  double        m_xPredTolOffset;                                               
  double        m_xPredTolConst;                                                
  double        m_tolMatchOffset;                                               
  double        m_tolMatchConst;    
  double        m_maxWindow;                                                   
  double        m_tolUConst;
  double        m_tolUOffset;
  double        m_tolVConst;
  double        m_tolVOffset;
  double        m_maxChi2;                                                     
  double        m_maxChi2ThreeHits;      
  double        m_minPt;    
  double        m_minMomentum;                                                 
  double        m_minTTx; ///< x distance of beampipe hole 
  double        m_minTTy; ///< y distance of beampipe hole 
                                              
  // -- parameters for making the track
  std::vector<double> m_zMagnetParams;   
  std::vector<double> m_momentumParams; 
  std::vector<double> m_yParams;     

  // -- more settable quantities
  double        m_zTTa;                  ///< Position of first station                  
  double        m_zTT;                   ///< Average position for track estimate        
  double        m_maxDeltaPConst;
  double        m_maxDeltaPOffset;
  double        m_xCorrectionConst;
  double        m_xCorrectionOffset;
  unsigned int  m_maxXTracks;
  double        m_maxChi2DistXTracks;
  unsigned int  m_maxXUTracks;
  double        m_maxChi2XProjection;
  double        m_overlapTol;
  double        m_yTol;
  double        m_seedCut;
  double        m_stateErrorX2;                                                        
  double        m_stateErrorY2;                                                        
  double        m_stateErrorTX2;                                                       
  double        m_stateErrorTY2;
  double        m_stateErrorP; 

  bool          m_removeUsed;                                                   
  bool          m_removeAll;       
  double        m_longChi2;
  int           m_seedKey;
  bool          m_withDebugTool;
  std::string   m_debugToolName;
  bool          m_printTracks;   
  bool          m_doTiming; 

  // -- internal helper variables
  std::array<double,7>    m_magPars;                                              
  std::array<double,3>    m_momPars;                                              
  double                  m_fishConst;
  std::array<double,5>    m_fishCoefficients;                                                                          
  std::array<int,4>       m_addIsStereoHelper;
                                                                                
  
  // -- hit collections
  std::array<PatTTHits,4> m_preSelHits;
  PatTTHits               m_xHits;
  PatTTHits               m_uvHits;
  std::array<PatTTHits,4> m_hitsLayers;
  PatTTHits               m_matchingXHits;
  PatTTHits               m_uHitsTemp;
  
  // -- track collections
  PatDownTracks           m_goodXTracks;
  PatDownTracks           m_goodXUTracks;
  PatDownTracks           m_trackCandidates;
  
  

};
#endif // PATKSHORT_H
