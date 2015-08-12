// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloDigit.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloNeighbours.h"
// ============================================================================
// local 
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================
// constructor
// ============================================================================
CaloEnergyForTrack::CaloEnergyForTrack
( const std::string& type   , // ? 
  const std::string& name   , 
  const IInterface*  parent ) 
  : Calo::CaloTrackTool ( type , name , parent ) 
  , m_address       (   ) 
  , m_bad           ( 1.e+10 )
  , m_digits        ( 0 )
  // 
  , m_location      ( LHCb::State::Spd ) 
  , m_planes        (       ) 
  , m_morePlanes    ( 0     )
  , m_addNeighbors  ( 0     )
{
  declareInterface <ICaloTrackIdEval>   ( this ) ;
  declareInterface <ICaloDigits4Track>  ( this ) ;
  declareInterface <IIncidentListener>  ( this ) ;
  //
  // ==========================================================================
  declareProperty 
    ( "DataAddress" , 
      m_address     , 
      "TES-location of corresponding CaloDigits") ;
  declareProperty 
    ( "BadValue"    , 
      m_bad         , 
      "The bad valur to be returned" ) ;
  declareProperty 
    ( "MorePlanes"  , 
      m_morePlanes  , 
      "Use more planes" ) ;
  declareProperty 
    ( "AddNeighbours" , 
      m_addNeighbors  , 
      "Add more neighbour levels" ) ;
  // ==========================================================================
} 
// ============================================================================
// initialize the tool 
// ============================================================================
StatusCode CaloEnergyForTrack::initialize() 
{
  StatusCode sc = Calo::CaloTrackTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  if ( m_address.empty()  ) { return Error("Invalid 'DataAddress'"); }    
  IIncidentSvc* isvc = svc<IIncidentSvc>( "IncidentSvc" , true ) ;
  isvc->addListener ( this , IncidentType::BeginEvent ) ;
  //
  std::string det = detectorName() ;
  const long caloID = CaloCellCode::CaloNumFromName( det.c_str() ) ;
  switch ( caloID ) 
  {
  case 0  : 
    // SPD 
    m_location = LHCb::State::Spd ;
    m_planes   = Planes ( 1 , calo()->plane ( CaloPlane::Middle ) ) ;
    break ;                                                     // BREAK 
  case 1  :
    // PRS  
    m_location = LHCb::State::Prs ;
    m_planes   = Planes ( 1 , calo()->plane ( CaloPlane::Middle ) ) ;      
    break ;
  case 2  :
    // ECAL 
    m_location = LHCb::State::ECalShowerMax ;
    m_planes   = Planes ( 1 , calo()->plane ( CaloPlane::Front     ) ) ;
    m_planes.push_back  (     calo()->plane ( CaloPlane::ShowerMax ) ) ;
    m_planes.push_back  (     calo()->plane ( CaloPlane::Middle    ) ) ;
    m_planes.push_back  (     calo()->plane ( CaloPlane::Back      ) ) ;
    break ; 
  case 3  :
    // HCAL 
    m_location = LHCb::State::MidHCal ;
    m_planes   = Planes ( 1 , calo()->plane ( CaloPlane::Front     ) ) ;
    m_planes.push_back  (     calo()->plane ( CaloPlane::Middle    ) ) ;
    m_planes.push_back  (     calo()->plane ( CaloPlane::Back      ) ) ;
    break ;
  default :
    return Error ( "Invald calorimeter TYPE! '" + detectorName() + "'" ) ;
  }    
  //
  if ( propsPrint() || msgLevel ( MSG::DEBUG ) ) 
  { info() << "State Location is set to '" << m_location << "'" << endmsg ; }
  //
  if ( m_planes.empty() ) { return Error ( "Empty list of Calo-planes" )  ; }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
/*  collect the cellID-s along the line 
 *  @param line   (INPUT)  the line  
 *  @param cells  (OUTPUT) the container of cells 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const CaloEnergyForTrack::Line& line   , 
  LHCb::CaloCellID::Set&          cells  ) const 
{
  cells.clear() ;
  
  // get the interesection points of the line with the planes 
  Points points ; 
  {
    points.reserve ( m_planes.size() + m_morePlanes ) ;
    // get the intersection points  
    for ( Planes::const_iterator iplane = m_planes.begin() ; 
          m_planes.end() != iplane ; ++iplane ) 
    {
      double  mu = 0 ;
      Gaudi::XYZPoint point ;
      Gaudi::Math::intersection ( line , *iplane , point , mu ) ;
      points.push_back ( point ) ;
    }
    // add some additional points (if requested) 
    if ( 2 <= points.size() && 0 < m_morePlanes )
    {
      // the same line but in another reparameterization:
      const Line l2 = Line ( points.front() , points.back() - points.front() ) ;
      const double dmu = 1.0 / ( 1.0 + m_morePlanes ) ;
      for ( unsigned short i = 0 ; i < m_morePlanes ; ++i ) 
      { points.push_back ( l2.position ( dmu * ( i + 1.0 ) ) ) ; }  
    }
  }
  // get all touched calorimeter cells: convert points to cells 
  for ( Points::const_iterator ipoint = points.begin() ;
        points.end() != ipoint ; ++ipoint )
  {
    // get the cell !
    const CellParam* pars = calo()->Cell_( *ipoint ) ;
    /// skip invalid cells 
    if ( 0 == pars || !pars->valid()  ) { continue ;}
    // collect all valid cells 
    cells.insert ( pars->cellID() ) ;            
  }
  
  // add neighbours 
  if ( 0 < m_addNeighbors ) 
  { LHCb::CaloFunctors::neighbours ( cells , m_addNeighbors , calo() ) ; }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  collect the cellID-s along the line 
 *  @param line   (INPUT)  the line  
 *  @param cells  (OUTPUT) the container of cells 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const CaloEnergyForTrack::Line& line   , 
  LHCb::CaloCellID::Vector&       cells  ) const 
{
  cells.clear() ;
  Cells _cells ;
  StatusCode sc = collect ( line , _cells ) ;
  if ( sc.isFailure() ) { return sc ; }
  cells.insert ( cells.end() , _cells.begin() , _cells.end() ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  collect the cellID-s along the path of the tracks 
 *  @param track  (INPUT)  the track 
 *  @param cells  (OUTPUT) the container of cells 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const LHCb::Track*     track  , 
  LHCb::CaloCellID::Set& cells  ) const 
{
  cells.clear() ;
  
  // get the correct state 
  const LHCb::State* state = CaloTrackTool::state ( *track , m_location ) ;
  if ( 0 == state )
  { 
    // propagate it! 
    StatusCode sc = propagate ( *track  , m_planes.front() , m_state ) ;
    if ( sc.isFailure() ) 
    { return Error ( "process(): failure from propagate,  return 'bad'" , sc ) ; }
    // initialize the state 
    state = &m_state ;
  }  
  // get the line from the state:
  const Line l1 = line ( *state ) ;
  
  return collect ( l1 , cells ) ;
}
// ============================================================================
/*  collect the cellID-s along the path of the tracks 
 *  @param track  (INPUT)  the track 
 *  @param cells  (OUTPUT) the container of cells 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const LHCb::Track*        track  , 
  LHCb::CaloCellID::Vector& cells  ) const 
{
  cells.clear() ;
  Cells _cells ;
  StatusCode sc = collect ( track , _cells ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from collect(set)" , sc ) ; }
  cells.insert ( cells.end() , _cells.begin() , _cells.end() ) ;
  return StatusCode::SUCCESS ;  
}
// ==========================================================================
/*  collect the fired digits along the path of the tracks 
 *  @param line   (INPUT)  the line  
 *  @param hits   (OUTPUT) the container of digits 
 *  @return status code 
 */
// ==========================================================================
StatusCode CaloEnergyForTrack::collect 
( const CaloEnergyForTrack::Line& line   , 
  LHCb::CaloDigit::Set&           hits   ) const 
{
  hits.clear() ;
  Cells cells ;
  StatusCode sc = collect ( line , cells ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from collect(cells)", sc ) ; }
  // convert cells into digits: 
  if ( !cells.empty() ) 
  {
    const Digits* data = digits() ;
    for ( Cells::const_iterator icell = cells.begin() ; 
          cells.end() != icell ; ++icell ) 
    {
      // get the gidit 
      const LHCb::CaloDigit* digit = data->object ( *icell ) ;
      // accumulate good fired cells 
      if ( 0 != digit ) { hits.insert ( digit ) ; }  
    } 
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  collect the fired digits along the line 
 *  @param line   (INPUT)  the line  
 *  @param digits (OUTPUT) the container of digits 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const CaloEnergyForTrack::Line&  line , 
  LHCb::CaloDigit::Vector&         hits ) const 
{
  hits.clear() ;
  LHCb::CaloDigit::Set _hits ;
  StatusCode sc = collect ( line , _hits ) ;
  if ( sc.isFailure() ) { return Error("Error from collect(set)", sc ) ; }
  hits.insert ( hits.end() , _hits.begin() , _hits.end() ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  collect the digits from the given calorimeter along the track 
 *  @param  track  (INPUT)  pointer to the object to be processed
 *  @param  digits (OUTPUT) the conatiner of collected digits 
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const LHCb::Track*    track , 
  LHCb::CaloDigit::Set& hits  )  const 
{
  hits.clear() ;
  Cells cells ;
  StatusCode sc = collect ( track , cells ) ;
  if ( sc.isFailure() ) { return Error ("Error from collect(cells)", sc ) ; }
  // convert cells into digits: 
  if ( !cells.empty() ) 
  {
    const Digits* data = digits() ;
    for ( Cells::const_iterator icell = cells.begin() ; 
          cells.end() != icell ; ++icell ) 
    {
      // get the gidit 
      const LHCb::CaloDigit* digit = data->object ( *icell ) ;
      // accumulate good fired cells 
      if ( 0 != digit ) { hits.insert ( digit ) ; }  
    } 
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  collect the digits from the given calorimeter along the track 
 *  @param  track  (INPUT)  pointer to the object to be processed
 *  @param  digits (OUTPUT) the conatiner of collected digits 
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloEnergyForTrack::collect 
( const LHCb::Track*       track , 
  LHCb::CaloDigit::Vector& hits  )  const 
{
  hits.clear() ;
  LHCb::CaloDigit::Set _hits ;
  StatusCode sc = collect ( track , _hits ) ;
  if ( sc.isFailure() ) { return Error ("Error from collect(set)", sc ) ; }
  hits.insert ( hits.end() , _hits.begin() , _hits.end() ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The main processing method 
// ============================================================================
StatusCode CaloEnergyForTrack::process   
( const LHCb::Track* track , 
  double&            value )  const 
{
  value = m_bad ;
  LHCb::CaloDigit::Set hits ;
  StatusCode sc = collect ( track , hits ) ;
  if ( sc.isFailure() ) { return Error ( "Error from 'collect(set)'", sc ) ; }
  
  // accumulate the energy from all touched cells 
  double energy = 0  ;
  for ( LHCb::CaloDigit::Set::const_iterator idigit = hits.begin() ; 
        hits.end() != idigit ; ++idigit ) 
  {
    const LHCb::CaloDigit* digit = *idigit ;
    if ( 0 != digit ) { energy += digit->e() ; }    
  }
  //
  value = energy ;                                             // RESULT
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The main processing method (functor interface)
// ============================================================================
double CaloEnergyForTrack::operator() ( const LHCb::Track* track ) const 
{
  double value = m_bad ;
  StatusCode sc = process( track , value ) ;
  Assert ( sc.isSuccess() , "Failure from process():" , sc );
  return value ;
} 
// ============================================================================
/// factory
// ============================================================================
DECLARE_TOOL_FACTORY(CaloEnergyForTrack)
// ============================================================================
// The END 
// ============================================================================


