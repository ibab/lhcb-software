// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// Math Definitions
// ============================================================================
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "Kernel/CaloCellCode.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloTrackAlg.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
// ============================================================================
/** @file 
 *  Implementation file for class InCaloAcceptance
 *  @see InAccpetance 
 *
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InCaloAcceptance )
// ============================================================================
//  Standard constructor, initializes variables
// ============================================================================
InCaloAcceptance::InCaloAcceptance
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : Calo::CaloTrackTool ( type, name , parent )
  , m_fiducial( true )
  , m_loc()
{
  declareInterface<IInAcceptance>(this);  
  //
  declareProperty ( "UseFiducial" , m_fiducial ) ;
  //
  _setProperty     ( "Extrapolator" , 
                     "TrackRungeKuttaExtrapolator/Master" ) ; 
  //                     "TrackMasterExtrapolator/Master" ) ; 
  //
}
// ============================================================================
// initialization @see IAlgTool
// ============================================================================
StatusCode InCaloAcceptance::initialize() 
{
  StatusCode sc = Calo::CaloTrackTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // check the detector 
  if ( 0 == calo() ) { return Error("Invalid Detector"); }
  // select the plane 
  const int check_calorimeter_id = 
    CaloCellCode::CaloNumFromName( detectorName() );
  switch ( check_calorimeter_id  ) 
  {
  case 0  : 
    // SPD 
    m_loc   = LHCb::State::Spd ; 
    m_plane = calo()->plane ( CaloPlane::Middle    )  ;
    break ;                                                     // BREAK 
  case 1  :
    // PRS  
    m_loc   = LHCb::State::Prs ; 
    m_plane = calo()->plane ( CaloPlane::Middle    )  ;
    break ;                                                     // BREAK 
  case 2  :
    // ECAL 
    m_loc   = LHCb::State::ECalShowerMax ; 
    m_plane = calo()->plane ( CaloPlane::ShowerMax )  ;
    break ;                                                     // BREAK 
  case 3  :
    // HCAL 
    m_loc   = LHCb::State::MidHCal ;
    m_plane = calo()->plane ( CaloPlane::Middle    )  ;
    break ;                                                     // BREAK 
  default :
    return Error ( "Invalid calorimeter TYPE! '" + detectorName() + "'" ) ;
  }
  //
  if ( propsPrint() || msgLevel ( MSG::DEBUG ) ) 
  {
    info() << "State to be used for aceptance check is '" 
           << m_loc << "'" << endmsg ;
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the track is in acceptance of the given calorimeter 
// ============================================================================
bool InCaloAcceptance::inAcceptance ( const LHCb::Track* track) const 
{
  // check the goodness of the tarck 
  if ( !use ( track ) ) { return false ; }                          // RETURN
 
  
  // find the appropriate state 
  const LHCb::State* state = 
    Calo::CaloTrackTool::state ( *track , m_loc ) ;
  if ( 0 == state ) 
  {
    // if there is no proper state - add it into the track! 
    StatusCode sc = propagate ( *track , plane() , m_state ) ;
    if ( sc.isFailure() )
    { 
      Error ( "failure from propagate()", sc ).ignore() ; 
      return false ;     // RETURN 
    }
    m_state.setLocation ( m_loc ) ;
    const_cast<LHCb::Track*>(track)->addToStates( m_state ) ;
    state = &m_state ;
  }
  // check the point 
  return  ok ( state->position() ) ;  
} 
// ============================================================================
// The END 
// ============================================================================
