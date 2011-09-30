// $Id: CaloPhotonMatch.cpp,v 1.7 2010-05-19 14:06:22 graven Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2010/03/08 01:31:34  odescham
// add neutral PID + configurable update
//
// Revision 1.5  2008/05/13 12:25:51  odescham
// restore _setProperty()
//
// Revision 1.4  2008/01/24 10:22:42  vegorych
// _setProperty was removed, AddNeigbours warning was solved
//
// Revision 1.3  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.2  2006/08/03 08:45:59  jonrob
// Add missing data initialisations
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
//  
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
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
#include "CaloUtils/CaloTrackMatch.h"
// ============================================================================

// ============================================================================
/** @file 
 *  Implementation file for class CaloPhotonMatch
 *  @date 2006-05-29 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class CaloPhotonMatch
  : public virtual ICaloTrackMatch 
  , public          CaloTrackMatch 
{
  // friend factory for instantiation ;
  friend class ToolFactory<CaloPhotonMatch> ;
public:
  /// initialize the tool 
  virtual StatusCode initialize () 
  {
    StatusCode sc = CaloTrackMatch::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    if (calo()->index() ==  CaloCellCode::EcalCalo){
      m_showerMaxLocation = LHCb::State::ECalShowerMax ;
      m_showerMax = calo()->plane( CaloPlane::ShowerMax ) ;
    }
    else if (calo()->index() ==  CaloCellCode::HcalCalo){
      m_showerMaxLocation = LHCb::State::MidHCal ;
      m_showerMax = calo()->plane( CaloPlane::Middle ) ;
    }
    else{
      return Error ( "initialize: calorimeter niether Ecal nor Hcal") ;
    }
    
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
  CaloPhotonMatch
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloTrackMatch ( type , name , parent ) 
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
    _setProperty ( "Calorimeter"  , DeCalorimeterLocation::Ecal  ) ;
    _setProperty ( "Tolerance"    , "15"    ) ; // 15 millimeters
    _setProperty ( "Extrapolator" , "TrackRungeKuttaExtrapolator/Regular" ) ;
    //    _setProperty ( "Extrapolator" , "TrackMasterExtrapolator/Regular" ) ;
  } ;
  /// virtual and protected destructor 
  virtual ~CaloPhotonMatch(){} ;
private:
  typedef CaloTrackMatch::Match_<2> Match ;
  //
  const LHCb::CaloPosition* m_position          ;
  Match                     m_caloMatch         ;
  Match                     m_trackMatch        ;
  Gaudi::Plane3D            m_plane             ;
  Gaudi::Plane3D            m_showerMax         ;
  const LHCb::CaloPosition* m_cBad              ;
  const LHCb::Track*        m_tBad              ;
  LHCb::State::Location     m_showerMaxLocation ;
} ;
// ============================================================================
DECLARE_TOOL_FACTORY(CaloPhotonMatch) ;
// ============================================================================
/// the main matching method
// ============================================================================
StatusCode CaloPhotonMatch::match 
( const LHCb::CaloPosition*   caloObj , 
  const LHCb::Track*          trObj   ,
  double&                     chi2    ) 
{
  using namespace LHCb ;
  
  chi2 = bad() ;                                                // reset chi2 
  //
  if ( m_cBad == caloObj || m_tBad == trObj ) { return StatusCode::FAILURE ; }
  //
  if ( 0 == caloObj ) 
  { return Error ( "match(): CaloPosition* points to NULL" ) ; }
  
  if ( !use ( trObj ) ) { return Error ( "match(): track is not OK"  ) ; }
  
  if ( m_position != caloObj ) 
  {
    // update the position
    StatusCode sc = fill ( *caloObj , m_caloMatch ) ;
    if ( sc.isFailure() ) 
    {
      m_cBad = caloObj ;
      return Error ( "match(): Error from fill(2D) -- ",StatusCode::FAILURE,0) ; 
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
    st = CaloTrackTool::state ( *trObj , m_showerMaxLocation ) ;
    if ( 0 == st ) 
    {
      StatusCode sc = propagate ( *trObj , m_showerMax  , _state() ) ;
      if ( sc.isFailure() ) 
      {
        m_tBad = trObj ;
        return Error ( "match(): failure from propagate (1) " , sc ) ; 
      }
      _state().setLocation( m_showerMaxLocation ) ;
      // ugly, but efficient 
      const_cast<LHCb::Track*> ( trObj ) -> addToStates ( _state() ) ;
      st = CaloTrackTool::state ( *trObj , m_showerMaxLocation ) ;
    }
    // check the state, propagate if needed  
    if ( tolerance() < ::fabs( m_plane.Distance ( st->position() ) )  ) 
    {
      _state() = *st ;
      StatusCode sc = propagate ( _state() , m_plane ) ;
      if ( sc.isFailure() ) 
      {
        m_tBad = trObj ;
        return Error ( "match(): failure from propagate (2) " , sc ) ; 
      }
      st = &_state() ;
    }
  }
  
  Assert ( 0 != st , "LHCb::State* points to NULL!" );

  StatusCode sc = fill ( *st , m_trackMatch ) ;
  if ( sc.isFailure() ) { return Error ( "match(): error for fill(2D)") ; }
  
  // make a real evaluation 
  chi2 = CaloTrackMatch::chi2 ( m_caloMatch , m_trackMatch ) ;
  
  return StatusCode::SUCCESS ;
} ;
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================
