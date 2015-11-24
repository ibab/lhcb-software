#ifndef PRHYBRIDSEEDING_H 
#define PRHYBRIDSEEDING_H 1

// Include files 
// from Gaudi
#include "PrHybridSeedTrack.h"

//uncomment this line if you want to do truth matching and do plots
//#include "GaudiAlg/GaudiAlgorithm.h"


#include "GaudiAlg/ISequencerTimerTool.h"
#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"
#include "PrGeometryTool.h"
#include "TfKernel/RecoFuncs.h"
#include "Event/MCParticle.h"

/** @class PrHybridSeeding PrHybridSeeding.h
 *  Stand-alone seeding for the FT T stations
 *  Used for the upgrade tracker TDR
 *  - InputName : Name of the input container for the forward tracks. Set to '""' to not reuse the FT part of the forward tracks
 *  - OutputName : Name of the output container for the seed tracks. By Default it's LHCb::TrackLocation::Seed
 *  - HitManagerName : Name of the hit manager
 *  - DecodeData : Decode the data ( default is false , as normally done in the Forward Tracking) Set to true if you run just the seeding
 *  - XOnly: use only the x-layers to reconstruct tracks
 *  - TimingMeasurement : Do timing measurement and print table at the end
 *  - PrintSettings : Print all values of the propertis at the beginning
 *  - NCases : Number of Cases for the algorithm ( value must be <=3 )
 *  - SlopeCorr : False by default. It change the errors on the hits from 1./err => 1./(err*cos(tx)*cos(tx))
 *  - stereo2: Pre-Computation of the 3 first best hough clusters with size 4,5,6 ( no overlap ) . It speeds up the algorithm but need some optimization
 *  - maxNbestCluster[Case]: Amount of first N best clusters to process for each Case
 *  - MaxNHits : Force the algorithm to find tracks with at maximum N Hits
 *  - RemoveClones : Flag that allow to run the global clones removal ( true by default )
 *  - minNCommonUV : Number of common hits in the global clone removal step
 *  - RemoveClonesX : Flag that allow to run the intermediate clone killing for x-z projection
 *  - FlagHits : Flag that allow to flag the hits on track found by each Case
 *  - RemoveFlagged : If set to true, whatever flagged hits found by the Case-i is not used in Case-j , where j>i
 *  - UseCubicCorrection: Modify the track model for x(z) = ax+bx*dz + cx*dz*dz*(1+dRatio*dz)
 *  - dRatio : dRatio value
 *  - CConst : Constant to compute the backward projection x(0)~ a_x - b_x*zref + cx*C ; C = Integral (0,zRef) Integral (0,z) By(z')*dz*dz'
 *  - UseCorrPosition : Correct the position for the simultaneous fit using the yOnTrack shift , i.e. z(y) instead of z(0);
 *  - SizeToFlag[Case] : Tracks with NHits>=SizeToFlag will have the hits flagged
 *  - Flag_MaxChi2DoF_11Hits[Case] : If Hits<12 Flag only hits on track having Chi2DoF<Flag_MaxChi2DoF_11Hits[Case]
 *  - Flag_MaxX0_11Hits[Case] : If Hits<12 Flag only hits on track having |x0(backProjection)| < Flag_MaxX0_11Hits[Case]
 *   
 *  ----------------------------- Parameters x-z projection search
 *  - 2-hit combination from T1-x + T3-x : given txinf = xT1/ZT1 
 *  - L0_AlphaCorr[Case]  : Rotation angle obtained looking to txinf vs ( Delta ) , where Delta = xT3(True) - xT1+txinf(zT3-zT1). 
 *  - L0_tolHp[Case]      : After rotating Delta' =  Delta + L0_alphaCorrd[Case] vs tx_inf ,   L0_alphaCorr[Case]*txinf - L0_tolHp[Case] <  xT3  <  L0_alphaCorr[Case]*txinf + L0_tolHp[Case]
 *  - 3-hit combination given straight line joining T1X and T3X. x0 is the straight line prediction from the two picked hits at z=0. 
 *  - tx_picked = (xT3-xT1)/(zT3-zT1). x0 = xT1 - zT1 * tx_picked
 *  - xPredT2 = x0 + zT2*tx_picked ( linear prediction from the 2 hit combination
 *  - x0Corr[Case] : defines a new xPredT2' = xPredT2 + x0*x0Corr[Case] ( rotation in the plane (xTrue - xPredT2) vs x0  to allign for the tolerances.
 *  --- considering only x0>0 ( equal by symmetry  for x0 <0)  ( see https://indico.cern.ch/event/455022/contribution/2/attachments/1186203/1719828/main.pdf  for reference ) 
 *  - X0SlopeChange[Case] : value of x0 at which start to open a larger upper tolerance ( max )
 *  - z0SlopeChangeDown[Case] : value of x0 at which start to open a larger lower toleance ( min )
 *  - ToleranceX0Up[Case] : upper tolerance when x0< X0SlopeChange[Case] 
 *  - ToleranceX0Down[Case] : lower tolerance when x0 < X0SlopeChangeDown[Case]
 *  - x0Cut[Case] : Value of X0 where to define the new tolerance up and down ( which then implicitely imply the opening tolerance up to X0SlopeChange(Down)[Case] . Must be > X0SlopeChange(Down)
 *  - TolAtX0CutOpp[Case] : lower tolerance for the deviation to collect hits in T2 at x0Cut from the xPredT2'
 *  - TolAtX0Cut[Case]     : upper tolerance for the deviation to collect hits in T2 at x0Cut from the xPredT2'
 *  - maxParabolaSeedHits: max number of hits to process collected in T2 given a 2 hit combination.
 *  ------ Collect remaining layers once a 3 hit combination is formed
 *  - TolXRemainign[Case] : from 3 hit the parabola ( + cubic correction ) is computed and the remaining xlayers hits are collected if  the hits are found in within TolXRemaining ( | hit::x(0 ) - xPred | < tolXPremaining )
 *  ----- Track is fitted in this scheme:
 *  - maxChi2HitsX[Case] : if Max Chi2(Hit) > maxChi2HitsX[Case] fit is failed and the outliers removed down to m_minXPlanes hits
 *  - maxChi2DoFX[Case] : max value of the Chi2 per DoF of the xz projections for each Case
 *  -------------------------------UV Search
 *  - Collect compatible hits in UV layers:
 *  - RemoveHole : remove hits found to have sqrt( x*x + y*y ) < RadiusHole
 *  - RadiusHole : see previous line
 *  - Positive defined tolerances for the y search  ( swapped when looking to lower module )
 *  - yMin : yMin Value to collect compatible hits in stereo
 *  - yMax : yMax Value to collect compatible hits in stereo    { upper track search in upper modyules : yMin < y < yMax }
 *  - yMin_TrFix : y Min Value to collect compatible hits in stereo when triangle fix is on 
 *  - yMax_TrFix : y Max Value to collect compatible hits in stereo when triangle fix is on     { upper track search in lower modules yMin_TrFix < t < yMax_TrFix
 *  - DoAsym : do asymmetric hit search taking into account stereo layers
 *  - TriangleFix : use triangle fixing
 *  - TriangleFix2ndOrder : use the info in Hit::yMax and Hit::yMin to remove the hits in upper modules in upper track leaking to y<0 
 *  -----------------------------Select hough cluster
 *  - UseLineY : Do a preliminary selection of the clusters fitting for a Line in Y 
 *  - minUV6[Case] : minUVLayers when XZ candidate has 6 hits  (planes)    
 *  - minUV5[Case] : minUVLayers when XZ candidate has 5 hits  (planes)    
 *  - minUV4[Case] : minUVLayers when XZ candidate has 4 hits  (planes)
 *  - Chi2LowLine[Case]  : if NXZ + NUV (Planes) <=10  XZChi2DoF + YLineChi2DoF < Chi2LowLine   
 *  - Chi2HighLine[Case] : if NXZ + NUV (Planes) >10 XZChi2DoF + YLineChi2DoF < Chi2HighLine   
 *  - minTot[Case]       : remove outliers until reaching minToT[Case] hits
 *  - Hough like cluster selection : select cluster ( sorted by |y/z| ) if |y/z| last - |y/z| first < TolTyOffset + TolTySlope * |y/z| first , where |y/z| first is always < than |y/z| last
 *  - TolTyOffset[Case]
 *  - TolTySlope[Case]
 *  --------------------------- Simultaneously fitting of the tracks
 *  - maxChi2Hits_11and12Hit[Case] : N Layers > 10 ( 11 and 12 hits ) outliers removed if MaxChi2Hit > maxChi2Hits_11and12Hit      
 *  - maxChi2Hits_less11Hit[Case]  : N Layers < 11 (9,10) outliers removed if MaxChi2Hit< maxChi2Hits_less11Hit   
 *  - maxYatZeroLow[Case]    :  If N Layers < 11: kill tracks having y(z=0) > maxYatZeroLow[Case] ( important for ghost suppression ) 
 *  - maxYatzRefLow[Case]    : If N Layers < 11 : kill tracks having y(zRef)> maxYatzRefLow [Case ] ( important for ghost suppression ) 
 *  - maxChi2PerDoF[Case]    : Max Chi2 per DoF of found tracks.
 *  @author Renato Quagliani
 *  @date   2015-03-11
 */

#include "GaudiAlg/GaudiAlgorithm.h"

class PrHybridSeeding : public GaudiAlgorithm{
public: 
  /// Standard constructor
  PrHybridSeeding( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~PrHybridSeeding( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
    
  /** @brief Collect Hits in X layers producing the xz projections
   * @param part (if 1, y<0 ; if 0 , y>0)
   */
  void findXProjections( unsigned int part, unsigned int iCase);
  /** @brief Add Hits on the x-z projection found
   *   @param part lower (1) or upper (0) half
   */
  void addStereo( unsigned int part , unsigned int iCase);
  
  /** @brief Collect Hits in UV layers given the tolerances
   * @param xProje x-z plane track projection
   * @param uvZones UV Layers where to look the hits
   * @return vector of u-v hits compatible with y tolerances
   */
  void CollectUV( PrHybridSeedTrack& xProje, const std::vector<PrHitZone*>& uvZones, PrHits& myStereo);
  
  

  /** @brief Add Hits from Stereo layers on top of the x-z projections found.
      The hough clusters are pre-stored in a 3X3 matrix so that the algorithm is much faster
      Need some re-tuning of parameters. it allows to not let explode the timing when TolTy is large
  */
  void addStereo2( unsigned int part , unsigned int iCase);
  typedef PrHits::iterator HitIter;
  typedef std::pair<HitIter,HitIter> IteratorPair;
  typedef std::vector< IteratorPair > HitIterPairs; //Vector of iterators
  typedef std::vector<HitIterPairs> HitIteratorsPairs; //Array of vector of iterators
  
  

  //** @brief, Given two iterators in the Stereo compatible hits sorted by Y/Z the hough cluster is extended given the tolerance in Ty and the number of different layers in the cluster is returned
  void ExtendCluster( PrHits::iterator& itBeg, PrHits::iterator& itEnd, unsigned int iCase, PrHits& myStereo , int &nLay);

  
  /** @brief, It checks if the hough cluster satisfy the tolerance 
   */
  bool CheckCluster( PrHits::iterator& itBeg, PrHits::iterator& itEnd, double tol);
  
  
  /** @brief, It increases the iterators
   */  
  void IncreaseIters( PrHits::iterator& itBeg, PrHits::iterator& itEnd, unsigned int minUV, PrHits& myStereo);
  
  
  /** @brief Check if the found line in the hough cluster satisfy the selection criteria
   */
  bool LineOK( double chi2low, double chi2high, PrLineFitterY& line, PrHybridSeedTrack& temp);
  
  
  /** @brief Remove Clones of produced tracks for each Case and part
   */
  void removeClonesX( unsigned int part, unsigned int iCase , bool xOnly);
  
  
  /** @brief Remove Clones of produced tracks once all cases are run
   */
  void removeClones(unsigned int maxCommon , unsigned int part);
  
  
  /** @brief Set the Chi2 value and Chi2DoF for the track after the X+U-V search
   */
  void setChi2(PrHybridSeedTrack& track);
  
  
  /** @brief Flag Hits under conditions at the end of the iCase loop 
   */
  void flagHits(unsigned int icase, unsigned int part);
  
  
  /** @brief Fit the track combining the XZ and YZ projections
   *  @param track The track to fit
   *  @param Refit Iteration in the Refitting after removal worst hit
   *  @return bool Success of the XY+XZ Fit
   **/
  bool fitSimultaneouslyXY( PrHybridSeedTrack& track ,unsigned int iCase);
  
  /** @brief Fit the track combining the only in the XZ plane
   *  @param track The track to fit
   *  @param Refit Iteration in the Refitting after removal worst hit
   *  @return bool Success of the XZ Fit
   **/
  bool fitXProjection( PrHybridSeedTrack & track ,unsigned int iCase );
  
  
  /** @brief Remove the hit which gives the largest contribution to the chi2 and refit XZ
   *  @param track The track to fit
   *  @return bool Success of the fit
   */
  bool removeWorstAndRefitX( PrHybridSeedTrack& track , unsigned int iCase );
  
  /** @brief Remove the hit which gives the largest contribution to the chi2 and refit XZ + YZ
   *  @param track The track to fit
   *  @return bool Success of the fit
   */
  bool removeWorstAndRefit( PrHybridSeedTrack& track , unsigned int iCase );
  
  /** @brief Set the chi2 of the track
   *  @param track The track to set the chi2 of 
   */
  void setChi2X( PrHybridSeedTrack& track );
  /** @brief Transform the tracks from the internal representation into LHCb::Tracks
   *  @param tracks The tracks to transform
   */
  void makeLHCbTracks( LHCb::Tracks* result , unsigned int part );
  
  /** @brief Print some information of the hit in question
   *  @param hit The hit whose information should be printed
   *  @param title Some additional information to be printed
   */
  void printHit( const PrHit* hit, std::string title="" );
  
  /** @brief Print some information of the track in question
   *  @param hit The track whose information should be printed
   */
  void printTrack( PrHybridSeedTrack& track );
  
  /** @brief Internal method to construct parabolic parametrisation out of three hits, using Cramer's rule.
   *  @param hit1 First hit
   *  @param hit2 Second hit
   *  @param hit3 Third hit
   *  @param a quadratic coefficient
   *  @param b linear coefficient
   *  @param c offset
   */
  void solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, double& a, double& b, double& c);
  /** @brief Internal method to construct parabolic parametrisation + cubic correction included out of three hits, using Cramer's rule.
   *  @param hit1 First hit
   *  @param hit2 Second hit
   *  @param hit3 Third hit
   *  @param a quadratic coefficient
   *  @param b linear coefficient
   *  @param c offset
   */
  void solveParabola2(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, double& a1, double& b1, double& c1);
  
  /// Classe to find lower bound of x of PrHits
  class lowerBoundX {
  public:
    bool operator() (const PrHit* lhs, const double testval ) const { return lhs->x() < testval; }
  };
  /// Classe to find upper bound of x of PrHits
  class upperBoundX {
  public:
    bool operator() (const double testval, const PrHit* rhs) const { return testval < rhs->x(); }
  };
  /// Class to compare x positions of PrHits
  class compX {
  public:
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->x() < rhs->x(); }
  };
  ///Class to find lower bound of LHCbIDs of PrHits
  class lowerBoundLHCbID {
  public:
    bool operator() (const PrHit* lhs, const LHCb::LHCbID id ) const { return lhs->id() < id; }
  };
  ///Class to compare LHCbIDs of PrHits
  class compLHCbID {
  public:
    bool operator() (const PrHit* lhs, const PrHit* rhs ) const { return lhs->id() < rhs->id(); }
  };
  
private:
  //-------------Names for input container(if forward as input), output container name, HitManager name
  bool m_recoverTrack;
  double m_chi2Recover ;
  bool m_SlopeCorr;
  bool m_stereo2;
  
  bool m_OnlyUp;
  bool m_OnlyDown;
  bool m_useType;
  
  std::string        m_inputName;
  std::string        m_outputName;
  std::string        m_hitManagerName;
  //-------------Global configuration of the algorithm
  bool               m_decodeData;
  bool               m_xOnly;
  unsigned int   m_minXPlanes;
  unsigned int   m_maxNHits;
  unsigned int   m_nCases;
  bool               m_doTiming;
  bool               m_printSettings;
  bool               m_useCubic;
  bool               m_removeClones;
  bool               m_removeClonesX;
  bool               m_FlagHits;
  bool               m_removeFlagged;
  
  //------------X-search parametrisation
  //1st / Last Layer search windows
  
  std::vector<double> m_alphaCorrection;
  std::vector<double> m_TolFirstLast;
  
  
  //Add of the third hit in middle layers (p and Pt dependent, i.e., case dependent)
  std::vector<double> m_x0Corr;
  std::vector<double> m_x0SlopeChange;
  std::vector<double> m_TolX0SameSign;
  std::vector<double> m_x0Cut;
  std::vector<double> m_tolAtX0Cut;
  
  std::vector<double> m_tolX0Oppsig;
  std::vector<double> m_x0SlopeChange2;
  std::vector<double> m_x0CutOppSig;
  std::vector<double> m_tolAtx0CutOppSig;
  
  //Add of remaining Hits in remaining X Layers
  std::vector<double> m_tolRemaining;
  //-----------
  //Add of third hit in T2
  unsigned int m_maxParabolaSeedHits;
  //Look up in remaining X layers
  //Look up in remaining X layers and Track Fitting parameters
  double           m_dRatio;
  double           m_ConstC;
  //--------_Fit X parametrisation
  std::vector<double>            m_maxChi2HitsX;
  std::vector<double>            m_maxChi2DoFX;
  
  //--------_Full Fit parametrisation
  std::vector<double>            m_maxChi2FullFit;
  std::vector<double>            m_maxChi2HitFullFitHigh;
  std::vector<double>            m_maxChi2HitFullFitLow;
  std::vector<double>            m_maxY0Low;
  std::vector<double>            m_maxYZrefLow;
  std::vector<double>            m_maxChi2HitLow;
  
  std::vector<double>            m_maxChi2HitFull ;
  
  //-----------------UV search parametrisation
  //Added
  double          m_yMin;
  double          m_yMin_TrFix;
  double          m_yMax;
  double          m_yMax_TrFix;
  double          m_doAsymmUV;
  double          m_radiusHole;
  
  //
  // double          m_coord;
  //Triangle Fix
  bool            m_useFix; 
  bool            m_removeHole;
  bool            m_useFix2ndOrder;
  
  
  bool m_useLineY;
  std::vector<double> m_Chi2LowLine;
  std::vector<double> m_Chi2HighLine;
  
  std::vector<unsigned int> m_minUV6;
  std::vector<unsigned int> m_minUV5;
  std::vector<unsigned int> m_minUV4;
  std::vector<unsigned int> m_minTot;
  
  // std::vector<double> m_X0ChangeCoord; (use Backward projection to define hough cluster?) ( to be implemented )
  
  std::vector<double> m_tolTyOffset;
  std::vector<double> m_tolTySlope;
    
  
  //X+Y fit configure
  std::vector<double>          m_maxChi2PerDoF;
  //Clone removal setting
  unsigned int              m_nCommonUV;
  bool                      m_ClonesUpDown;
  unsigned int              m_nCommonUVTriangle;
  unsigned int              m_nUsed;
  //Flag Hits Settings
    
  std::vector<double>         m_MaxChi2Flag;
  std::vector<double>         m_MaxX0Flag;
  std::vector<unsigned int>   m_SizeFlag;
  
  //dRatio correction to use (temporary)
  bool m_useCorrPos;    
  
  //--------------------Global things
  
  PrHitManager*   m_hitManager;
  PrGeometryTool* m_geoTool;
  
  //== Debugging controls
  std::string     m_debugToolName;
  int             m_wantedKey;
  IPrDebugTool*   m_debugTool;
  
  
  //-------------------Containers
  std::array<std::vector<PrHybridSeedTrack>,2>      m_trackCandidates;
  std::array<std::vector<PrHybridSeedTrack>,2>      m_xCandidates;
  std::vector<PrHitZone*>        m_zones;
  double m_zReference;
  ISequencerTimerTool* m_timerTool;
  int            m_timeTotal;
  int            m_timeFromForward;
  int            m_timeXProjeUp[3];
  int            m_timeStereoUp[3];
  int            m_timeCloneXUp[3];
  int            m_timeFlagUp[3];
  int            m_timeXProjeDo[3];
  int            m_timeStereoDo[3];
  int            m_timeCloneXDo[3];
  int            m_timeFlagDo[3];
  int            m_timeClone[2];
  int            m_timeConvert[2];
  // only for stereo2 
  std::vector<int>           m_maxNClusters;
  //-------------------Algorithms Constants
  // Stations Zones numbering
  static const unsigned int s_T1X1 = 0;
  static const unsigned int s_T1U  = 2;
  static const unsigned int s_T1V  = 4;
  static const unsigned int s_T1X2 = 6;
  static const unsigned int s_T2X1 = 8;
  static const unsigned int s_T2U  = 10;
  static const unsigned int s_T2V  = 12;
  static const unsigned int s_T2X2 = 14;
  static const unsigned int s_T3X1 = 16;
  static const unsigned int s_T3U  = 18;
  static const unsigned int s_T3V  = 20;
  static const unsigned int s_T3X2 = 22;
  static const unsigned int s_down = 0;
  static const unsigned int s_up   = 1;
};
#endif // PRHYBRIDSEEDING_H
