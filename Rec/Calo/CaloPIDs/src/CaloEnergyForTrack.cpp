// $Id: CaloEnergyForTrack.cpp,v 1.3 2008-06-03 15:49:56 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2008/01/24 10:22:42  vegorych
// _setProperty was removed, AddNeigbours warning was solved
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
//  
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
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
// local 
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================

// ============================================================================
/// factory
// ============================================================================
DECLARE_TOOL_FACTORY(CaloEnergyForTrack) ;
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
  , m_nSigmas       ( 0     )
  , m_addNeighbors  ( false )
{
  declareInterface <ICaloTrackIdEval>  ( this ) ;
  declareInterface <IIncidentListener> ( this ) ;
  //
  declareProperty ( "DataAddress" , m_address       ) ;
  declareProperty ( "BadValue"    , m_bad           ) ;
  declareProperty ( "MorePlanes"  , m_morePlanes    ) ;
  declareProperty ( "NSigmas"     , m_nSigmas       ) ;
  declareProperty ( "AddNeigbours" , m_addNeighbors  ) ;
} ;
// ============================================================================
/// initialize the tool 
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
  { info() << "State Location is set to '" << m_location << "'" << endreq ; }
  //
  if ( m_planes.empty() ) { return Error ( "Empty list of Calo-planes" )  ; }
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// The main processing method (functor interface)
// ============================================================================
double CaloEnergyForTrack::operator() ( const LHCb::Track* track ) const 
{
  double value = m_bad ;
  StatusCode sc = process( track , value ) ;
  Assert ( sc.isSuccess() , "Failure from process():" , sc );
  return value ;
} ;
// ============================================================================
/// The main processing method 
// ============================================================================
StatusCode CaloEnergyForTrack::process   
( const LHCb::Track* track , 
  double&            value )  const 
{
  value = m_bad ;
  if ( !use ( track ) ) { return Error ( "Track is not OK" ) ; }
  
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
      Gaudi::Math::intersection ( l1 , *iplane , point , mu ) ;
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
  Cells cells ;
  for ( Points::const_iterator ipoint = points.begin() ;
        points.end() != ipoint ; ++ipoint )
  {
    // get the cell !
    const CellParam* pars = calo()->Cell_( *ipoint ) ;
    /// skip invalid cells 
    if ( 0 == pars || !pars->valid()  ) { continue ;}
    // collect all valid cells 
    cells.insert ( pars->cellID() ) ;            
    if ( m_addNeighbors ) 
    {
      const CaloNeighbors& nei = pars->neighbors() ;
      cells.insert( nei.begin() , nei.end() ) ;
    }
  }
  // accumulate the energy from touched cells 
  double energy = 0  ;
  if ( !cells.empty() ) 
  {
    const Digits* data = digits() ;
    for ( Cells::const_iterator icell = cells.begin() ; 
          cells.end() != icell ; ++icell ) 
    {
      // get the gidit 
      const LHCb::CaloDigit* digit = data->object ( *icell ) ;
      // accumulate the energy for good fired cells 
      if ( 0 != digit ) { energy += digit->e() ; }  
    } ;
  }
  //
  value = energy ;                                             // RESULT
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
// The END 
// ============================================================================


