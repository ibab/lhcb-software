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
// local
// ============================================================================
// #include "CaloUtils/CaloTrackMatch.h"
#include "CaloTrackMatch.h"
// ============================================================================

// ============================================================================
/** @file 
 *  Implementation file for class CaloElectronMatch
 *  @date 2006-05-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class CaloElectronMatch
  : public virtual ICaloTrackMatch 
  , public          CaloTrackMatch
{
  // friend factory for instantiation ;
  friend class ToolFactory<CaloElectronMatch> ;
public:
  /// initialize the tool 
  virtual StatusCode initialize () 
  {
    StatusCode sc = CaloTrackMatch::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    m_showerMax = calo()->plane( CaloPlane::ShowerMax ) ;
    return StatusCode::SUCCESS ;
  } ;
public:
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
  } ;
  /** extract the TrState which was actually used for last matching
   *  @attention TrState is owned by the tool itself 
   *  The better solution could be to return the pointer 
   *  to TrStateP 
   *  @return pointer to the state actually used for last matching
   */
  virtual const LHCb::State* state   () const { return &_state() ; } ;
protected:
  /// standard constructor 
  CaloElectronMatch
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloTrackMatch  ( type , name , parent ) 
    //
    , m_position ( NULL ) 
    , m_caloMatch  () 
    , m_trackMatch ()
    , m_plane      ()  
    , m_showerMax  ()
    , m_cBad ( 0 ) 
    , m_tBad ( 0 )
  {
    declareInterface<ICaloTrackMatch> ( this ) ;
    // 
    _setProperty ( "Calorimeter"  , DeCalorimeterLocation::Ecal ) ;
    _setProperty ( "Tolerance"    , "5" ); // 5 millimeters
    _setProperty ( "Extrapolator" , "TrackRungeKuttaExtrapolator/Regular" ) ;
    //    _setProperty ( "Extrapolator" , "TrackMasterExtrapolator/Regular" ) ;
  } ;
  /// virtual and protected destructor 
  virtual ~CaloElectronMatch(){} ;
private:
  typedef CaloTrackMatch::Match_<3> Match ;
  //
  const LHCb::CaloPosition* m_position   ;
  Match                     m_caloMatch  ;
  Match                     m_trackMatch ;
  Gaudi::Plane3D            m_plane      ;
  Gaudi::Plane3D            m_showerMax  ;
  const LHCb::CaloPosition* m_cBad       ;
  const LHCb::Track*        m_tBad       ;
} ;
// ============================================================================
DECLARE_TOOL_FACTORY(CaloElectronMatch)
// ============================================================================
/// the main matching method
// ============================================================================
StatusCode CaloElectronMatch::match 
( const LHCb::CaloPosition*   caloObj , 
  const LHCb::Track*          trObj   ,
  double&                     chi2    ) 
{
  using namespace LHCb ;
  
  chi2 = bad() ;                                                // reset chi2 
  //
  // check the stored values!
  if ( m_cBad == caloObj || m_tBad == trObj ) { return StatusCode::FAILURE ; }
  
  if ( 0 == caloObj ) 
  { return Error ( "match(): CaloPosition* points to NULL" ) ; }
  
  if ( !use( trObj ) ) { return Error ( "match(): track is not OK"  ) ; }
  
  if ( m_position != caloObj ) 
  {
    // update the position
    StatusCode sc = fill ( *caloObj , m_caloMatch ) ;
    if ( sc.isFailure() ) 
    {
      m_cBad = caloObj ;
      return Warning ( "match(): Error from fill(2D) ", sc, 0) ; 
    }
    // find the proper plane in detector
    const LHCb::CaloPosition::Center& par = caloObj->center() ;
    const Gaudi::XYZPoint point ( par ( 0  ) , par ( 1 ) , caloObj->z() ) ;
    if ( tolerance() <  m_plane.Distance ( point ) ) 
    {  m_plane = calo()->plane  ( point ) ; }  
    // keep the track of the position
    m_position = caloObj ;
  }
  //  
 // get the correct state 
  const LHCb::State* st = 0 ;
  {
    st = CaloTrackTool::state ( *trObj , LHCb::State::ECalShowerMax ) ;
    if ( 0 == st ) 
    {
      StatusCode sc = propagate ( *trObj , m_showerMax  , _state() ) ;
      // _state() = trObj->closestState( m_showerMax ) ; 
      // StatusCode sc = propagate ( _state(), caloObj->z()) ;
      if ( sc.isFailure() ) 
      {
        m_tBad = trObj ;
        return Warning ( "match(): failure from propagate (1) " , sc ) ; 
      }
      _state().setLocation(  LHCb::State::ECalShowerMax ) ;
      const_cast<LHCb::Track*>(trObj) ->addToStates ( _state() ) ;
      st = CaloTrackTool::state ( *trObj , LHCb::State::ECalShowerMax ) ;
    }
    // check the validity of the state 
    if ( tolerance() < ::fabs( m_plane.Distance ( st->position() ) ) ) 
    // if ( 1 * Gaudi::Units::mm < ::fabs( caloObj->z() - st->position().z() ) )  // let's decrease z-tolerance here to 1 mm
    {
      _state() = *st ;
      StatusCode sc = propagate ( _state() , m_plane ) ;
      // StatusCode sc = propagate ( _state() , caloObj->z() ) ;
      if ( sc.isFailure() ) 
      { 
        m_tBad = trObj ;
        return Warning ( "match(): failure from propagate (2) " , sc ) ; 
      }
      st = &_state() ;
    }
  }
  
  Assert ( 0 != st , "LHCb::State* points to NULL!" );
  StatusCode sc = fill ( *st , m_trackMatch ) ;
  if ( sc.isFailure() ) { return Warning ( "match(): error for fill(3D)") ; }
  
  // make a real evaluation 
  chi2 = CaloTrackMatch::chi2 ( m_caloMatch , m_trackMatch ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
