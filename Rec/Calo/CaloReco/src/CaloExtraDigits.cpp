// $Id: CaloExtraDigits.cpp,v 1.10 2006-11-28 13:15:16 cattanem Exp $
// ============================================================================
// Include files
// STL 
#include <cmath>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/SmartRef.h"
// DetDesc 
#include "DetDesc/IGeometryInfo.h" 
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent/Event 
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
// local
#include "CaloExtraDigits.h"

#ifdef WIN32

template<>
bool std::less<SmartRef<LHCb::CaloDigit>       >::operator() 
 ( const SmartRef<LHCb::CaloDigit>& ref1 , 
   const SmartRef<LHCb::CaloDigit>& ref2 ) const 
{ return (const LHCb::CaloDigit*) ref1 < (const LHCb::CaloDigit*) ref2 ; };

template<>
bool std::less<const SmartRef<LHCb::CaloDigit> >::operator() 
 ( const SmartRef<LHCb::CaloDigit>& ref1 , 
   const SmartRef<LHCb::CaloDigit>& ref2 ) const 
{  return (const LHCb::CaloDigit*) ref1 < (const LHCb::CaloDigit*) ref2 ; };
#endif 

// ============================================================================
/** @file CaloExtraDigits.cpp
 * 
 *  Implementation file for class : CaloExtraDigits
 *  @see CaloExtraDigits 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 01/04/2002 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( CaloExtraDigits );

// ============================================================================
/** Standard constructor
 *  @param type   tool type (?)
 *  @param name   tool name 
 *  @param parent tool parent 
 */
// ============================================================================
CaloExtraDigits::CaloExtraDigits( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool            ( type, name , parent ) 
  , m_inputData         ( ""       )
  , m_digits            ( 0        )
  , m_z                 ( -10 * Gaudi::Units::km ) 
  , m_xTol              ( 0        ) 
  , m_yTol              ( 0        )
  , m_vertex            (          )
  , m_addSeed           ( true     )
  , m_addSeedNeighbours ( true     ) 
  , m_selector          ( "Ecal"   )
  , m_detData()
{
  declareInterface<ICaloHypoTool>     (this);
  declareInterface<IIncidentListener> (this);  
  declareProperty ( "Input"         , m_inputData         ) ;
  declareProperty ( "AddSeed"       , m_addSeed           ) ;
  declareProperty ( "AddNeighbours" , m_addSeedNeighbours ) ;
  declareProperty ( "xTol"          , m_xTol              ) ;
  declareProperty ( "yTol"          , m_yTol              ) ;
  declareProperty ( "Detector"      , m_detData           ) ;
};

// ============================================================================
/// Destructor (virtual and protected)
// ============================================================================
CaloExtraDigits::~CaloExtraDigits(){};
// ============================================================================

// ============================================================================
/** standard initialization of the tool
 *  @see IAlgTool
 *  @see  AlgTool
 *  @see GaudiTool
 *  @return status code 
 */
// ============================================================================
StatusCode CaloExtraDigits::initialize () 
{
  // initilaize the base class 
  StatusCode sc = GaudiTool::initialize ();
  if( sc.isFailure() ) 
  { return Error( "Could not initialize the base class GaudiTool!", sc ) ; }
  
  // check the detector data 
  m_det = getDet<DeCalorimeter>( m_detData );
  if( 0 == m_det ) { return Error("DeCalorimeter* points to NULL!");}

  const IGeometryInfo* geoinf = m_det->geometry() ;
  if( 0 == geoinf ) { return Error("IGeometryInfo is not available!"); }
  // center of the detector in mother reference frame 
  Gaudi::XYZPoint center = geoinf->toGlobal( Gaudi::XYZPoint() );
  m_z = center.z() + m_det->zShowerMax ();
  //  subscribe the incidents 
  incSvc()->addListener( this , IncidentType::EndEvent   , 10 );
  incSvc()->addListener( this , IncidentType::BeginEvent , 10 );
  
  // reset digits 
  m_digits = 0 ;

  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** handle the incident 
 *  @see Incident 
 *  @see IIncidentListener 
 *  @see IIncidentSvc 
 *  @param incident incident to be handled 
 */
// ============================================================================
void CaloExtraDigits::handle( const Incident& /* incident */  ) 
{ m_digits = 0 ; };
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @see ICaloHypoTool 
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloExtraDigits::process    ( LHCb::CaloHypo* hypo  ) const 
{ return (*this) ( hypo ); };
// ============================================================================


// ============================================================================
/** The main processing method (functor interface)
 *  
 *  Error codes:
 *
 *   - 200 : input CaloHypo* object points to NULL
 *   - 201 : CaloHypo::position()   points to NULL
 *   - 202 : CaloHypo::clusters()  is empty! 
 *   - 203 : No valid 'Ecal' clusters are found! 
 *   - 204 : Cluster has no 'Seed' cell! 
 *   - 205 : 'Seed' digit points to NULL!
 *
 *  @exception 
 *  @see ICaloHypoTool 
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloExtraDigits::operator() ( LHCb::CaloHypo* hypo  ) const 
{ 
  
  // valid hypo?
  
  if( 0 == hypo        ) { return Error("CaloHypo* points to NULL!" , 200 ) ; }
  const LHCb::CaloPosition* position = hypo->position() ;
  if( 0 == position    ) { return Error("CaloPosition* is invalid!" , 201 ) ; }


  // locate digits 
  if( 0 == m_digits ) { m_digits = get<LHCb::CaloDigits>( m_inputData  ) ; }
  
  // add the digits in front of seed digit of Ecal cluster
  if( m_addSeed ) 
  {
    // find ecal cluster 
    const LHCb::CaloHypo::Clusters& clusters = hypo->clusters();
    if( clusters.empty() ) 
    {                  return Error("No clusters are found!"    , 202 ) ; }
    // get clusters from 'Ecal'
    LHCb::CaloHypo::Clusters::const_iterator ecalCl = 
      std::find_if( clusters.begin() , clusters.end() , m_selector );
    if( clusters.end() == ecalCl ) 
    {                  return Error("No valid 'Ecal' cluster!"  , 203 ) ; }
    const LHCb::CaloCluster::Entries& entries = (*ecalCl)->entries();
      LHCb::CaloCluster::Entries::const_iterator seed = 
        LHCb::ClusterFunctors::
        locateDigit( entries.begin ()          , 
                     entries.end   ()          ,
                     LHCb::CaloDigitStatus::SeedCell ) ;
      if( entries.end() == seed ) 
      {                  return Error("No 'Seed' in cluster!"     , 204 ) ; }
      const LHCb::CaloDigit* seedD = seed->digit() ;
      if( 0 == seedD ) {   return Error("'Seed' points to NULL!"    , 205 ) ; }
      LHCb::CaloDigit* digit = m_digits->object( seedD->cellID() );
      if( 0 != digit ) { hypo->addToDigits( digit ); }
      // add neighbours of the seed 
      if( m_addSeedNeighbours )
      {
        const CaloNeighbors& neighbors = 
          m_det->neighborCells( seedD->cellID() );
        for( CaloNeighbors::const_iterator nei = neighbors.begin() ; 
             neighbors.end() != nei ; ++nei )
        {
          LHCb::CaloDigit* digit = m_digits->object( *nei );
          if( 0 != digit ) { hypo->addToDigits( digit ); }
        }     
      }    
  }
  
  // line along the photon flight 
  const  Gaudi::XYZPoint pos( position->x() ,
                              position->y() ,
                              position->z() );
  
  // point in the detector 

  Gaudi::XYZVector vec ( pos - m_vertex );
  vec *=  ( m_z - m_vertex.z() ) / ( pos.z() - m_vertex.z() );
  Gaudi::XYZPoint newPoint =   m_vertex + vec;

  /*
  Gaudi::XYZPoint newPoint ( pos - m_vertex );
  newPoint *=  ( m_z - m_vertex.z() ) / ( pos.z() - m_vertex.z() );
  newPoint +=    m_vertex ;
  */

  const LHCb::CaloCellID cell = m_det->Cell( newPoint );
  
  if( !(LHCb::CaloCellID() == cell) )  // valid cell! 
  {    
    LHCb::CaloDigit* digit = m_digits->object( cell );
    if( 0 != digit ) { hypo->addToDigits( digit ); }
  }
  
  // loop over neighbour cells 
  if( (  0 < m_xTol || 0 < m_yTol ) && !(LHCb::CaloCellID() == cell) )
  {
    const CaloNeighbors& neighbors = m_det->neighborCells( cell );
    for( CaloNeighbors::const_iterator nei = neighbors.begin() ; 
         neighbors.end() != nei ; ++nei )
    {
      const double      cellHalf   = 0.5 * m_det -> cellSize   ( *nei ) ;
      if( 0 >= cellHalf ) { continue ; }               // CONTINUE ! 
      const Gaudi::XYZPoint& cellCenter =       m_det -> cellCenter ( *nei ) ;
      if( ( fabs( cellCenter.x() - newPoint.x() ) < cellHalf + m_xTol ) && 
          ( fabs( cellCenter.y() - newPoint.y() ) < cellHalf + m_yTol )  ) 
      {
        LHCb::CaloDigit* digit = m_digits->object( *nei );
        if( 0 != digit ) { hypo->addToDigits( digit ); }
      }    
    }
  };
  
  {
    // remove duplicates (if any)
    LHCb::CaloHypo::Digits& digits = hypo->digits() ;
    std::sort( digits.begin() , digits.end() , 
               std::less<const SmartRef<LHCb::CaloDigit> >() );
    LHCb::CaloHypo::Digits::iterator i = 
      std::unique( digits.begin() , digits.end() );
    if( digits.end() != i ) { digits.erase( i , digits.end() ) ; }
  }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

