// ============================================================================
// Include files
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/CaloPosition.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloTrackMatch.h"
// ============================================================================
// Local
// ============================================================================
#include "Linear.h"
// ============================================================================
/** @file 
 *  Implementation file for class CaloBremMatch
 *  @date 2006-05-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class CaloBremMatch
  : public virtual ICaloTrackMatch 
  , public          CaloTrackMatch 
{
  // ==========================================================================
  /// friend factory for instantiation ;
  friend class ToolFactory<CaloBremMatch> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// initialize the tool 
  virtual StatusCode initialize () 
  {
    StatusCode sc = CaloTrackMatch::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    m_showerMax = calo()->plane( CaloPlane::ShowerMax ) ;
    return StatusCode::SUCCESS ;
  } 
  // ==========================================================================
public:
  // ==========================================================================
  /** the main matching method  
   *  @see ICaloTrackMatch 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  virtual StatusCode match 
  ( const LHCb::CaloPosition*   caloObj , 
    const LHCb::Track*          trObj   ,
    double&                     chi2    ) ;  
  /** The main matching method (Stl interface) 
   *  @see ICaloTrackMatch 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @return pair of status code/chi2  for matching procedure 
   */
  virtual MatchingPair    operator() 
    ( const LHCb::CaloPosition*   caloObj  , 
      const LHCb::Track*          trObj    ) 
  {
    double chi2 ;
    StatusCode sc =  match ( caloObj , trObj , chi2 ) ;
    return std::make_pair( sc , chi2 ) ;
  } 
  /** extract the TrState which was actually used for last matching
   *  @attention TrState is owned by the tool itself 
   *  The better solution could be to return the pointer 
   *  to TrStateP 
   *  @return pointer to the state actually used for last matching
   */
  virtual const LHCb::State* state   () const { return &_state() ; } 
  // ==========================================================================
protected:
  // ==========================================================================
  /// standard constructor 
  CaloBremMatch
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloTrackMatch  ( type , name , parent ) 
    , m_position   ( 0 ) 
    , m_caloMatch  () 
    , m_trackMatch ()
    , m_plane      ()  
    , m_showerMax  ()
    //
    , m_cBad     ( 0 )
    , m_tBad     ( 0 )
  {
    declareInterface<ICaloTrackMatch> ( this ) ;
    // 
    _setProperty ( "Calorimeter"  , DeCalorimeterLocation::Ecal );
    _setProperty ( "Tolerance"    , "10" ) ; // 10 millimeters
    _setProperty ( "zForFastExtrapolator" , "0" ); // 0  meters 
  } 
  /// virtual and protected destructor 
  virtual ~CaloBremMatch(){} 
  // ==========================================================================
private:
  // ==========================================================================
  typedef CaloTrackMatch::Match_<2> Match ;
  //
  const LHCb::CaloPosition* m_position   ;
  Match                     m_caloMatch  ;
  Match                     m_trackMatch ;
  Gaudi::Plane3D            m_plane      ;
  Gaudi::Plane3D            m_showerMax  ;
  const LHCb::CaloPosition* m_cBad       ;
  const LHCb::Track*        m_tBad       ;  
  // ==========================================================================
} ;
// ============================================================================
DECLARE_TOOL_FACTORY(CaloBremMatch)
// ============================================================================
/// the main matching method
// ============================================================================
StatusCode CaloBremMatch::match 
( const LHCb::CaloPosition*   caloObj , 
  const LHCb::Track*          trObj   ,
  double&                     chi2    ) 
{
  chi2 = bad() ;                                                // reset chi2 
  
  // check the stored values!
  if ( m_cBad == caloObj || m_tBad == trObj ) { return StatusCode::FAILURE ; }
  
  if ( 0 == caloObj ) 
  { return Error ( "match(): CaloPosition* points to NULL" ) ; }
  
  if ( !use( trObj ) ) { return Error ( "match(): track is not OK"  ) ; }
  
  if ( m_position != caloObj ){
    // update the position
    StatusCode sc = fillBrem ( *caloObj , m_caloMatch ) ;
    if ( sc.isFailure() ) 
    { 
      m_cBad = caloObj ;
      return Warning ( "match(): Error from fill(2D) ", sc, 0) ; 
    }
    // find the proper plane in the detector
    const LHCb::CaloPosition::Center& par = caloObj->center() ;
    const Gaudi::XYZPoint point ( par ( 0  ) , par ( 1 ) , caloObj->z() ) ;
    if ( tolerance() <  m_plane.Distance ( point ) ) 
    {  m_plane = calo()->plane  ( point ) ; }  
    // keep the track of the position
    m_position = caloObj ;
  }
  // 
  // get the correct state 
  const LHCb::State* state = 0 ;
  { // get the correct state at TT 
    state   = CaloTrackTool::state ( *trObj , LHCb::State::AtTT     ) ;
    if ( 0 == state ) 
    { state = CaloTrackTool::state ( *trObj , LHCb::State::EndRich1 ) ; }
    if ( 0 == state ) 
    { state = CaloTrackTool::state ( *trObj , LHCb::State::BegRich1 ) ; }
    if ( 0 == state ) 
    { state = CaloTrackTool::state ( *trObj , LHCb::State::EndVelo  ) ; }
    // no appropriate state is found 
    if ( 0 == state ) 
    { 
      // get the closest state to some artificial value  
      state = &(trObj->closestState( 2.0 * Gaudi::Units::meter ) ) ;
      // allowed z ?
      if ( state->z() > 4.0 * Gaudi::Units::meter ) 
      {
        if(msgLevel(MSG::DEBUG)) print ( debug() , trObj ) ;
        m_tBad = trObj ;
        return Warning ( "No appropriate states are found, see 'debug'") ; 
      }
    }
    // use the linear extrapolator 
    // StatusCode sc = propagate ( _state() , m_plane ) ;
    StatusCode sc = Utils::Linear::propagate ( *state , m_plane , _state() ) ;
    if ( sc.isFailure() ) 
    { 
      m_tBad = trObj ;
      return Warning ( "match(): failure from propagate (1) " , sc ) ; 
    }
  }
  
  StatusCode sc = fill ( _state() , m_trackMatch ) ;
  if ( sc.isFailure() ) { return Warning ( "match(): error for fill(2D)") ; }
  
  // make a real evaluation 
  chi2 = CaloTrackMatch::chi2 ( m_caloMatch , m_trackMatch ) ;
  
  return StatusCode::SUCCESS ;
} 
// ============================================================================
/// The END 
// ============================================================================
