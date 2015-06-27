// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/BoostArrayAsProperty.h"
#include "GaudiKernel/IAlgorithm.h"
// ============================================================================
// Local
// ============================================================================
#include "MessagingBase.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IPVReFitter.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/VertexFitWithTracks.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysExtract.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campain by Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2014-03-14
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class PVReFitter     
   *  Light-weight primary vertex refitter
   *  @see IPVRefitter 
   *
   *  It happens that the default tool AdaptivePVRefitter is *VERY*
   *  slow, especially for events with high multiplicity of candiates.
   *  In particular it dominates in the CPU consumptiion for 
   *  (u)DST-writers, where "per-stream" refit is invoked for all
   *  selected canddiates. AdaptivePVRefitter does *not* 
   *  remove tracks from PV, actually it remove the track and then 
   *  remove PV from scratch. From other side PVOfflinetool is much 
   *  more efficient, but it does not implement the correct interface.
   *  @see AdaptivePVReFitter
   *  @see PVOfflineTool 
   *
   *  Based on these two observation, light-weight tool is coded
   *  - It allows fast removal fo track from primary vertex. 
   *    Actually the single track removal is treated as one-step 
   *    Kalman Filter with *negative* weight. 
   *    For removal of several tracks single "multi-step" is done, 
   *    that allows to avoid Kalman smoothing and drastically 
   *    reduce number of matrix inversions 
   *  - It (optionally) allows the efficients fast adaptive 
   *    Kalman Filter primary vertex fit, using LoKi::KalmanFilter 
   *    The refit is done usung single "multi-step" method, 
   *    that allows to avoid Kalman smoothing and drastically 
   *    reduce number of matrix inversionsl
   *  @see LoKi::KalmanFilter    
   *
   *  For track-removal the tool appears to be two orders of magnitude 
   *  faster than AdaptivePVReFitter, and a bit faster than PVOfflineTool 
   *  For primary vertex refit, the tool is 
   *  5-10 times faster that AdaptivePVReFitter.
   *  There are many configratiuon parameters, that allows 
   *  to choose the track state provider/extrapolator, 
   *  track lookup, tolerances and iteration control parameters.
   *   
   *  For default configuration, for track removal the fast method is used, 
   *  but for low-multiplicity vertices, the subsequent PV-refit is invoked 
   *
   *  @todo Check Tukey's parameters: the currect setting corresponds 
   *        to LSAdaptPVFitter
   *  @see LSAdaptPVFitter
   
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2014-03-14
   *  @thanks Wouter Hulsbergen
   *  
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  // ==========================================================================
  class PVReFitter : public extends1<MessagingBase,IPVReFitter> 
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class ToolFactory<LoKi::PVReFitter> ;
    // ========================================================================
  public: // error codes 
    // ========================================================================
    enum {                    // error codes 
      /// Invalid vertex 
      InvalidVertex1  = 801 , // invalid vertex
      /// Invalid vertex 
      InvalidVertex2  = 802 , // invalid vertex
      /// Invalid particle  
      InvalidParticle = 803 , // invalid particle
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /** refit PV
     *  @see IPVReFitter 
     *  @param vx the vertex to be refit
     *  @return status code 
     */
    virtual StatusCode reFit ( LHCb::VertexBase* v ) const 
    {
      //
      if ( 0 ==  v ) { return _Error ( "Invalid     vertex " , InvalidVertex1 ) ; }
      LHCb::RecVertex* pv = dynamic_cast<LHCb::RecVertex*> ( v ) ;
      if ( 0 == pv ) { return _Error ( "Invalid Rec-vertex " , InvalidVertex2 ) ; }
      //
      return _reFit_ ( *pv ) ;
    }
    // ========================================================================
    /** remove tracks used for the particle, keeping the vertex valid 
     *  @see IPVReFitter 
     *  @param  p the particle to be removed 
     *  @param vx the vertex to be refit
     *  @return status code 
     */
    virtual StatusCode remove
    ( const LHCb::Particle* p ,  
      LHCb::VertexBase*     v ) const 
    {
      if ( 0 ==  p ) { return _Error ( "Invalid particle "   , InvalidParticle ) ; }
      if ( 0 ==  v ) { return _Error ( "Invalid     vertex " , InvalidVertex1  ) ; }
      LHCb::RecVertex* pv = dynamic_cast<LHCb::RecVertex*> ( v ) ;
      if ( 0 == pv ) { return _Error ( "Invalid Rec-vertex " , InvalidVertex2  ) ; }
      //
      // collect all tracks from the given particle 
      LHCb::Track::ConstVector tracks ;
      tracks.reserve ( 10 ) ;
      LoKi::Extract::tracks ( p , std::back_inserter ( tracks ) ) ;
      if ( tracks.empty() ) { return StatusCode::SUCCESS ; }   // RETURN
      //
      return _remove_ ( tracks , *pv ) ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the tool
    virtual StatusCode initialize () ;                   // initialize the tool
    ///   finalize the tool
    virtual StatusCode   finalize () ;                   //   finalize the tool
    // ========================================================================
  private:
    // ========================================================================
    /// standard constructor 
    PVReFitter 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) ;
    /// virtual destrcutor
    virtual ~PVReFitter () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    PVReFitter () ;                    // the default constructor is disabled 
    /// copy constructoir is disabled 
    PVReFitter            ( const PVReFitter& ) ; // copy is disabled 
    /// assignement operator is disabled 
    PVReFitter& operator= ( const PVReFitter& ) ; // disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// get the appropriate state from the track 
    // ========================================================================
    inline const LHCb::State* state 
    ( const LHCb::Track&     track , 
      const Gaudi::XYZPoint& point ) const 
    {
      //
      // velo-only case 
      //
      if ( LHCb::Track::Velo  == track.type() || LHCb::Track::VeloR == track.type() ) 
      {
        //
        if ( 0 != m_stateProvider ) 
        {
          StatusCode sc = m_stateProvider->stateFromTrajectory
            ( m_state , track , point.z () ) ;
          ++counter ( "state-from-trajectory" ) ;
          if ( sc.isSuccess() ) { return &m_state ; }   // RETURN
        }
        //
        if ( 0 != m_veloExtrapolator ) 
        {
          StatusCode sc = m_veloExtrapolator-> propagate ( m_state, point ) ;
          ++counter ("state-from-velo-extrapolator" ) ;
          if ( sc.isSuccess() ) { return &m_state ; }   // RETURN
        }
        //
      }
      else 
      {
        //
        if ( 0 != m_stateProvider ) 
        {
          StatusCode sc = m_stateProvider->state 
            ( m_state , track , point.z() , m_tolerance ) ;
          ++counter ( "state-from-provider" ) ;
          if ( sc.isSuccess() ) { return &m_state ; }   // RETURN
        }
        //
        if ( 0 != m_extrapolator ) 
        {
          StatusCode sc = m_extrapolator-> propagate( m_state , point ) ;
          ++counter ( "state-from-extrapolator" ) ;
          if ( sc.isSuccess() ) { return &m_state ; }   // RETURN
        }
        //
      }
      //
      ++counter ( "state-from-closeststate" ) ;
      return &track.closestState ( point.z() ) ;
    }
    // ========================================================================
    /// remove the tracks from primary vertex 
    StatusCode   _remove_ 
    ( const LHCb::Track::ConstVector& tracks   , 
      LHCb::RecVertex&                pv       ) const ;
    /// refit the primary vertex 
    StatusCode   _reFit_ ( LHCb::RecVertex& pv ) const ;
    /// load the data 
    unsigned int _load_ 
    ( const LHCb::RecVertex&     pv        ,
      const Gaudi::XYZPoint&     seed      ,
      const unsigned short       iter      ,
      const Gaudi::SymMatrix3x3* ci    = 0 ) const ;
    // ========================================================================
    inline double tukey 
    ( const double       chi2ip , 
      const unsigned int iter   ) const 
    {
      const double a  = std::max ( std::abs ( m_tukey[0] ) , 
                                   std::abs ( m_tukey[1] ) - 
                                   std::abs ( m_tukey[2] ) * iter ) ;
      const double a2 = a * a ;
      if ( chi2ip > a2 ) { return 0 ; }
      const double weight = 1 - chi2ip / a2 ;
      return weight * weight ;
    }
    // ========================================================================
  private: // properties 
    // ========================================================================
    /// the name of        track state provider tool 
    std::string    m_stateProviderName    ; // the name of track state provider
    /// the name of        track extrapolator   tool 
    std::string    m_extrapolatorName     ; // track extrapolator name 
    /// the name of linear track extrapolator   tool 
    std::string    m_veloExtrapolatorName  ; // linear extrapolator name 
    /// the proparagation tolerance 
    double         m_tolerance      ;
    /// check tracks by LHcbIDs ? 
    bool           m_checkIDs       ;
    /// check the ancestors for the given track 
    bool           m_checkAncestors ;
    /// perform the full refit of the vertiex after track removal? 
    bool           m_reFit      ;
    /// maximal number of iterations
    unsigned short m_maxIter    ;
    /// minimal number of iterations for chi2-stop 
    unsigned short m_iterChi2   ;
    /// minimal number of iterations for distance-stop 
    unsigned short m_iterDist   ; 
    /// delta-chi2     stopping criteria
    double         m_delta_chi2 ;    
    /// delta-distance stopping criteria
    double         m_delta_dist ;
    /// minimum number of tracks in PV
    unsigned int   m_minTracksInPV ;
    std::string    m_minTS;
    /// tukey's parameters 
    // double         m_tukey[3]   ;
    boost::array<double,3> m_tukey ;
    // ========================================================================
  private: // data itself 
    // ========================================================================
    ITrackStateProvider* m_stateProvider    ;
    ITrackExtrapolator*  m_extrapolator     ;
    ITrackExtrapolator*  m_veloExtrapolator ;
    // ========================================================================
  private:
    // ========================================================================
    typedef LoKi::KalmanFilter::TrEntry4    TrEntry   ;
    typedef LoKi::KalmanFilter::TrEntries4  TrEntries ;
    //
    mutable TrEntries   m_entries ;
    mutable TrEntry     m_entry   ;
    mutable LHCb::State m_state   ;
    // ========================================================================
  }; //                                           end of class LoKi::PVReFitter 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// standard constructor 
// ============================================================================
LoKi::PVReFitter::PVReFitter 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : base_class ( type , name , parent )
    //
  , m_stateProviderName     ( "TrackStateProvider:PUBLIC"      )   
  , m_extrapolatorName      ( "TrackMasterExtrapolator:PUBLIC" ) 
  , m_veloExtrapolatorName  ( "TrackLinearExtrapolator:PUBLIC" )
  , m_tolerance             ( 2 * Gaudi::Units::um )
  , m_checkIDs              ( true  ) 
  , m_checkAncestors        ( true  ) 
  , m_reFit                 ( false ) 
    //
  , m_maxIter               ( 16    ) 
  , m_iterChi2              ( 4     ) 
  , m_iterDist              ( 3     )
  , m_delta_chi2            ( 0.001 )
  , m_delta_dist            ( 1 * Gaudi::Units::um ) 
  , m_minTracksInPV         ( 4     )
  , m_minTS                 ( "4"   )
    //
  , m_tukey (  { { 3.0 , 15.0 , 3.0 } }  )      // used in LSAdaptPVFitter
    // , m_tukey (  { { 4.0 , 24.0 , 4.0 } }  ) 
    // , m_tukey (  { { 5.0 , 35.0 , 5.0 } }  ) // used in SimplePVFitter.cpp
    //  
  , m_stateProvider         (   0   )  
  , m_extrapolator          (   0   ) 
  , m_veloExtrapolator      (   0   )
    //
  , m_entries () 
  , m_entry   ()
  , m_state   () 
{
  //
  declareProperty 
    ( "TrackStateProvider"     , 
      m_stateProviderName      , 
      "Tool used as ITrackStateProvider" ) ;
  //
  declareProperty 
    ( "TrackExtrapolator"      , 
      m_extrapolatorName       , 
      "Tool used as ITrackExtrapolator"  ) ;
  //
  declareProperty 
    ( "VeloTrackExtrapolator"  , 
      m_veloExtrapolatorName   , 
      "Tool used as for extrapolation of velo-only tracks" ) ;
  //
  declareProperty 
    ( "PropagationTolerance"   , 
      m_tolerance              ,
      "The tolerance to be used for TrackStateProvider::state" ) ;
  //
  declareProperty 
    ( "CheckTracksByLHCbIDs"   , 
      m_checkIDs               ,
      "Check the tracks by LHCbIDs " ) ;
  //
  declareProperty 
    ( "CheckAncestors"                , 
      m_checkAncestors                ,
      "Check ancestors for the track" ) ;
  //
  declareProperty 
    ( "FullReFit"              , 
      m_reFit                  ,
      "Perform the full refit after track removal?" ) ;
  //
  declareProperty 
    ( "MaxIterations"      , 
      m_maxIter            , 
      "Maximal number of iterations" ) ;
  //
  declareProperty 
    ( "MinChi2Iterations"      , 
      m_iterChi2               , 
      "Minimal number of iterations before delta(chi2)     check" ) ;
  //
  declareProperty 
    ( "MinDistanceIterations"      , 
      m_iterDist               , 
      "Minimal number of iterations before delta(distance) check" ) ;
  //
  declareProperty 
    ( "DeltaDistance"         , 
      m_delta_dist            , 
      "Delta-distance as convergency criterion"    ) ;
  //
  declareProperty 
    ( "DeltaChi2"             ,  
      m_delta_chi2            , 
      "Delta-chi2     as convergency criterion"    ) ;
   //
  declareProperty 
    ( "MinTracksInPV",  
      m_minTracksInPV, 
      "Minimum number of tracks in a PV") ;

  //
// ==========================================================================
}
// ============================================================================
// virtual destrcutor
// ============================================================================
LoKi::PVReFitter::~PVReFitter () {}
// ============================================================================
// initialize the tool
// ============================================================================
StatusCode LoKi::PVReFitter::initialize ()               // initialize the tool
{
  //
  StatusCode sc = MessagingBase::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                  // RETURN
  //
  m_stateProvider        = 0 ;
  m_extrapolator         = 0 ; 
  m_veloExtrapolator     = 0 ;
  m_minTS                = std::to_string(m_minTracksInPV);
  //
  if ( !m_stateProviderName    . empty () ) 
  { m_stateProvider    = tool<ITrackStateProvider> ( m_stateProviderName    , this ) ; }
  if ( !m_extrapolatorName     . empty () ) 
  { m_extrapolator     = tool<ITrackExtrapolator>  ( m_extrapolatorName     , this ) ; }
  if ( !m_veloExtrapolatorName . empty () ) 
  { m_veloExtrapolator = tool<ITrackExtrapolator>  ( m_veloExtrapolatorName , this ) ; }
  //
  if ( 0 != m_stateProvider     ) 
  { debug() << "TrackStateProvider will be used" << endmsg ; }
  if ( 0 != m_extrapolator      ) 
  { debug() << "TrackExtrapolator  will be used" << endmsg ; }
  if ( 0 != m_veloExtrapolator  ) 
  { debug() << "VeloExtrapolator   will be used" << endmsg ; }
  //
  return StatusCode::SUCCESS ; 
}
// ============================================================================
//   finalize the tool
// ============================================================================
StatusCode LoKi::PVReFitter::finalize ()               // finalize the tool
{
  //
  m_stateProvider        = 0 ;
  m_extrapolator         = 0 ; 
  m_veloExtrapolator     = 0 ;
  //
  return MessagingBase::finalize () ;
}
// ============================================================================
namespace  // local anonymous namespace to keep local functions 
{
  // ==========================================================================
  /// comparison of double numbers 
  const LHCb::Math::Equal_To<double>  s_equal ;
  // ==========================================================================
  /// null-track 
  static const LHCb::Track* const s_track = 0 ;
  // ==========================================================================  
  std::pair<const LHCb::Track*,double> 
  trackInPV 
  ( const LHCb::Track*     track        , 
    const LHCb::RecVertex& pv           , 
    const bool             useIDs       , 
    const bool             useAncestors ) 
  {
    //
    if ( 0 == track ) { return std::make_pair ( track, 0 ) ; }
    std::pair<bool,double> c  = pv.trackWeight ( track ) ;
    if ( c.first )
    { 
      return s_equal   ( c.second , 0.0      ) ? 
        std::make_pair ( s_track  , 0.0      ) : 
        std::make_pair ( track    , c.second ) ;
    }
    //
    if ( useIDs )  
    {
      //
      typedef SmartRefVector<LHCb::Track> TRACKS  ;
      typedef std::vector<float>          WEIGHTS ;
      //
      const TRACKS&  tracks  = pv.tracks  () ;
      const WEIGHTS  weights = pv.weights () ;
      //
      const unsigned int n1 = track->nLHCbIDs() ;
      //
      for ( unsigned int i = 0 ; i < tracks.size() ; ++i ) 
      {
        // check only valid tracks 
        const LHCb::Track* tr = tracks  [i] ;
        if ( 0 == tr )           { continue ; } // CONTINUE 
        // check only non-zero weights 
        const double       w  = weights [i] ;
        if ( s_equal ( w , 0 ) ) { continue ; } // CONTINUE 
        //
        const unsigned int n2     = tr->nLHCbIDs() ;
        //
        const unsigned int common = tr->nCommonLhcbIDs ( *track  ) ;
        //
        if ( 1.33 * common >= n1 ) { return std::make_pair( tr , w ) ; }
        if ( 1.33 * common >= n2 ) { return std::make_pair( tr , w ) ; }
        //
      }
    }
    //
    if ( useAncestors ) 
    { 
      typedef SmartRefVector<LHCb::Track>  ANCESTORS ;
      const ANCESTORS& ancestors = track->ancestors() ;
      //
      for ( ANCESTORS::const_iterator ia = ancestors.begin() ; 
            ancestors.end() != ia ; ++ia ) 
      {
        // check the track (recursion here!) 
        const std::pair<const LHCb::Track*,double> p =             // RECUSION 
          trackInPV ( *ia , pv , useIDs , useAncestors ) ;
        //
        if ( p.first && !s_equal ( p.second , 0.0 ) ) { return p ; } // RETURN
      }
    }
    //
    return std::make_pair ( s_track , 0 ) ;
  }
  // ==========================================================================
} 
// ============================================================================
// remove the tracks from primary vertex 
// ============================================================================
StatusCode LoKi::PVReFitter::_remove_ 
( const LHCb::Track::ConstVector& tracks , 
  LHCb::RecVertex&                pv     ) const 
{
  //
  // collect the actual tracks in vertex to be removed 
  LHCb::Track::ConstVector removed ; removed.reserve( tracks.size() ) ;
  for ( LHCb::Track::ConstVector::const_iterator it = tracks.begin() ; 
        tracks.end() != it ; ++it ) 
  {
    //
    const std::pair<const LHCb::Track*, double> p = 
      trackInPV ( *it , pv , m_checkIDs , m_checkAncestors ) ;
    //
    const LHCb::Track* track  = p.first  ;
    const double       weight = p.second ;
    if ( 0 == track || s_equal ( weight  , 0 )  ) { continue ; }
    //
    /// @attention: here "track" or "track_0" ???
    removed.push_back ( track ) ;  // or track_0 ???   
    //
  }
  //
  // some statistics 
  ++counter ("#removed") += removed.size() ;
  //
  // - nothing to be removed 
  if ( removed.empty() ) { return StatusCode::SUCCESS ; }   // RETURN
  //
  // - too many tracks to remove 
  if ( removed.size () +  m_minTracksInPV  > pv.tracks().size() ) 
  {return _Warning( "Less than "+m_minTS+" tracks in vertex remain",
                     StatusCode::FAILURE, 3 ) ; }
  // 
  // - too many tracks to remove
  if ( removed.size () + 10  > pv.tracks().size() ) 
  { 
    // make the explicit refit instead of Kalman-Filter removal 
    // 1) remove tracks 
    for ( LHCb::Track::ConstVector::const_iterator it = removed.begin() ; 
          removed.end() != it ; ++it ) { pv.removeFromTracks ( *it ) ; }
    // 2) refit vertex 
    return _reFit_ ( pv )  ;
  }    
  //
  // - start the actual Kalman-filter machinery for removal 
  //
  m_entries.clear()                    ;
  m_entries.reserve ( removed.size() ) ;
  //
  for ( LHCb::Track::ConstVector::const_iterator it = removed.begin() ; 
        removed.end() != it ; ++it ) 
  {
    //
    const LHCb::Track*     track  = *it ;
    if ( 0 == track                         ) { continue ; }
    std::pair<bool,double> c      = pv.trackWeight ( *it ) ;
    const double           weight = c.second ;
    if ( !c.first || s_equal ( weight , 0 ) ) { continue ; }
    //
    // get the proper state 
    const LHCb::State* s = state ( *track , pv.position () ) ;
    m_entries.push_back ( TrEntry () ) ;
    // load the entry with THE OPPOSITE WEIGHT 
    const StatusCode sc  = 
      LoKi::KalmanFilter::load ( s , m_entries.back() , -1*weight ) ;
    if ( sc.isFailure() ) 
    {
      _Warning ( "Error from KalmanFilter::load, skip", sc, 0 ) ;
      m_entries.pop_back() ; 
    }
  }
  //
  // some statistics 
  ++counter ("#toremove") += removed.size() ;
  ++counter ("#loaded"  ) += m_entries.size() ;
  //
  // the data entries are loaded properly, make a step of kalman filter:
  //
  // 1) prepare the gain-matrix for PV 
  Gaudi::SymMatrix3x3 ci ;
  /// use fast cholesky inversion 
  const int ifail = Gaudi::Math::inverse ( pv.covMatrix() , ci ) ;
  //// int ifail = 0 
  //// const Gaudi::SymMatrix3x3 ci = pv.covMatrix().Inverse( ifail );
  if ( 0 != ifail ) { return _Warning( "Non-invertible covariance matrix!",
                                       StatusCode::FAILURE, 0 ) ; }
  //
  // 2) make (multi-step) of Kalman filter 
  const StatusCode sc = LoKi::KalmanFilter::step ( m_entries     , 
                                                   pv.position() , 
                                                   ci            ,
                                                   0             ) ;
  if ( sc.isFailure() ) 
  { return _Warning("Error from KalmanFilter::step", sc, 0 ) ; }   // RETURN 
  //
  // 3) finally update the vertex
  for ( LHCb::Track::ConstVector::const_iterator it = removed.begin() ; 
        removed.end() != it ; ++it ) { pv.removeFromTracks ( *it  ) ; }
  //
  const TrEntry& last = m_entries.back() ;
  //
  const Gaudi::XYZPoint   newpos ( last.m_x[0] , last.m_x[1] , last.m_x[2] ) ;
  const Gaudi::XYZPoint&  oldpos = pv.position() ;
  ++counter ( "#delta-R" )      += ( newpos.Z() - oldpos.Z() ) ;
  //
  // 4) update vertex parameters 
  // set the Chi^2 and the DoF of the vertex (fit)
  pv.setChi2AndDoF ( pv.chi2() + last.m_chi2 , 2 * pv.tracks().size() - 3 ) ;
  // set position
  pv.setPosition   ( newpos ) ;
  // set covariance 
  pv.setCovMatrix  ( last.m_c  ) ;
  // refit it ?
  if ( m_reFit || 10 > pv.tracks().size() ) 
  {
    ++counter("#refit-forced") ;
    return _reFit_ ( pv ) ; 
  }
  //
  return SUCCESS ;
}
// ============================================================================
unsigned int LoKi::PVReFitter::_load_ 
( const LHCb::RecVertex&     pv   ,
  const Gaudi::XYZPoint&     seed ,
  const unsigned short       iter ,
  const Gaudi::SymMatrix3x3* ci   ) const 
{
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  typedef TRACKS::const_iterator      TIT    ;
  //
  const TRACKS& tracks = pv.tracks() ;
  //
  m_entries.clear   () ;
  m_entries.reserve ( tracks.size() ) ;
  //
  unsigned int nTracks = 0 ;
  for ( TIT itrack = tracks.begin() ; tracks.end () != itrack ; ++itrack ) 
  {
    //
    const LHCb::Track* track = *itrack ;
    if ( 0 == track ) { continue ; }                                // CONTINUE 
    //
    m_entries.push_back( TrEntry() ) ;
    //
    // load it! 
    const LHCb::State* s = state ( *track , seed ) ;
    StatusCode sc = LoKi::KalmanFilter::load ( s , m_entries.back()  ) ;
    if ( sc.isFailure() ) 
    {
      _Warning ( "Unable to load data, skip the track", sc, 0 ) ;
      m_entries.pop_back() ;                   
      continue ;                                                // CONTINUE 
    }
    //
    TrEntry& entry = m_entries.back () ;
    entry.m_track  = track ;
    //
    if ( 0 != iter && 0 != ci )  // add weight 
    {
      // to get chi2(ip), make one step of Kalman filter, it could be expensive! 
      sc = LoKi::KalmanFilter::step ( entry , seed , *ci , 0 ) ;
      if ( sc.isFailure() ) { entry.m_weight = 1                             ; }
      else                  { entry.m_weight = tukey ( entry.m_chi2 , iter ) ; }
      //
    }
    //
    if ( !s_equal ( entry.m_weight , 0 ) ) { ++nTracks ; }
    //
  }
  //
  // return number of good loaded tracks
  return nTracks ; // return number of good loaded tracks
}
// ============================================================================
// refit the primary vertex 
// ============================================================================
StatusCode LoKi::PVReFitter::_reFit_ ( LHCb::RecVertex& pv ) const 
{
  //
  typedef SmartRefVector<LHCb::Track> TRACKS ;
  //
  const TRACKS& tracks = pv.tracks() ;
  //
  if      ( 2 > tracks.size () ) 
  { return _Warning ( "Not enough     tracks in vertex!" ) ; }
  else if ( m_minTracksInPV > tracks.size () ) 
  {_Warning      ( "Less than "+m_minTS+" tracks in vertex!" ).ignore() ; }
  //
  // 
  Gaudi::XYZPoint     x  = pv.position  () ;
  Gaudi::SymMatrix3x3 ci = pv.covMatrix () ; // just placeholder
  //
  double chi2old = 1.e+99 ;
  //
  unsigned short iIter = 0 ;
  for ( ; iIter < m_maxIter ; ++iIter ) 
  {
    // load the data around the seed point 
    const unsigned int nTracks = _load_ ( pv , x , iIter , &ci ) ;
    if      ( 2 > nTracks ) 
    { return _Warning ( "Not enough good tracks in the vertex!" ) ; }  // RETURN
    else if ( m_minTracksInPV > nTracks ) 
    { _Warning        ( "Less than "+m_minTS+" good tracks in vertex!", 
                        StatusCode::SUCCESS, 0 ).ignore() ; }
    //
    // make Kalman-Filter step 
    //
    const StatusCode sc = LoKi::KalmanFilter::step ( m_entries ) ;
    //
    TrEntry&        last  = m_entries.back() ;
    //
    const double chi2new  = last.m_chi2 ;
    //
    // calculate the differences for convergency checks 
    //
    const double chi2dist = std::abs ( chi2new - chi2old ) ;
    const double dist     = 
      std::abs ( x.X () - last.m_x [0] ) + 
      std::abs ( x.Y () - last.m_x [1] ) + 
      std::abs ( x.Z () - last.m_x [2] ) ;
    //
    // update the quantities 
    //
    chi2old    = chi2new   ;
    ci         = last.m_ci ; // no need to invert it again 
    Gaudi::Math::la2geo ( last.m_x , x ) ;
    //
    // check the convergency: 
    //
    if      ( m_iterChi2 < iIter && chi2dist <= m_delta_chi2 ) { break ; } // BREAK
    else if ( m_iterDist < iIter &&     dist <= m_delta_dist ) { break ; } // BREAK 
    //
  } // iterations 
  //
  if ( iIter >= m_maxIter ) { _Warning ( "No convergency has been reached",
                                         StatusCode::SUCCESS, 0 ) ; }
  //
  const TrEntry& last = m_entries.back() ;
  //
  // fill PV
  // 
  // set the Chi^2 and the DoF of the vertex (fit)
  pv.setChi2AndDoF ( last.m_chi2 , 2 * m_entries.size() - 3 ) ;
  // set position 
  pv.setPosition   ( x         ) ;
  // set covariance 
  pv.setCovMatrix  ( last.m_c  ) ;
  //
  // fill tracks & weights 
  pv.clearTracks() ;
  for ( TrEntries::const_iterator ie = m_entries.begin () ; m_entries.end() != ie ; ++ie )
  {
    const LHCb::Track* track = ie->m_track ;
    if ( NULL == track ) { continue ; }
    pv.addToTracks ( track , ie->m_weight )  ;
    counter ("track-weight")  += ie->m_weight ;
  }
  //
  counter ( "#iterations"   ) += ( iIter + 1 )       ;
  // total number of tracks  
  counter ( "#tracks/total" ) += pv.tracks().size () ;
  const unsigned int nGood = LoKi::KalmanFilter::nTracks ( m_entries , 1.e-4 ) ;
  // number of tracks with non-negligible weight  
  counter ( "#tracks/good"  ) += nGood ;
  // number of tracks with small weight 
  counter ( "#tracks/null"  ) += pv.tracks().size() - nGood ;
  //
  return StatusCode::SUCCESS ;
}  
// ==========================================================================

// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , PVReFitter )
// ============================================================================
// The END 
// ============================================================================



