// $Id: CaloExtraDigits.cpp,v 1.2 2002-04-23 10:49:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
// Include files
// STL 
#include <cmath>
#include <algorithm>
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IIncidentSvc.h" 
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

// ============================================================================
/** \var CaloExtraDigitsFactory  
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloExtraDigits>         s_factory ;
const        IToolFactory&CaloExtraDigitsFactory = s_factory ;
// ============================================================================

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
  : CaloTool    ( type, name , parent ) 
  , m_incSvc    ( 0        )
  , m_evtSvc    ( 0        ) 
  , m_inputData ( ""       )
  , m_digits    ( 0        )
  , m_z         ( -10 * km ) 
  , m_xTol      ( 0        ) 
  , m_yTol      ( 0        )
  , m_vertex    (          )
  , m_addSeed   ( true     )
  , m_selector  ( "Ecal"   )
{
  declareInterface<ICaloHypoTool>     (this);
  declareInterface<IIncidentListener> (this);  
};
// ============================================================================

// ============================================================================
/// Destructor (virtual and protected)
// ============================================================================
CaloExtraDigits::~CaloExtraDigits(){};
// ============================================================================

// ============================================================================
/** standard initialization of the tool
 *  @see IAlgTool
 *  @see  AlgTool
 *  @see CaloTool
 *  @return status code 
 */
// ============================================================================
StatusCode CaloExtraDigits::initialize () 
{
  // initilaize the base class 
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool!", sc ) ; }
  // check the detector data 
  const DeCalorimeter* detector = get( detSvc() , detName() , detector );
  if( 0 == detector ) { return Error("DeCalorimeter* points to NULL!");}
  setDet( detector );
  const IGeometryInfo* geoinf = det()->geometry() ;
  if( 0 == geoinf ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame 
  HepPoint3D center = geoinf->toGlobal( HepPoint3D() );
  m_z = center.z() + det()->zShowerMax ();
  // locate incident service 
  sc = serviceLocator() -> service ( "IIncidentSvc" , m_incSvc );
  if( sc.isFailure() ) { return Error("Could not locate IIncidentSvc!", sc );}
  if( 0 == m_incSvc  ) { return Error("Could not locate IIncidentSvc!"     );}
  m_incSvc->addRef() ;
  m_incSvc->addListener( this , "EndEvent"   , 10 );
  m_incSvc->addListener( this , "BeginEvent" , 10 );
  // locat evenet service 
  sc = serviceLocator() -> service ( "EventDataSvc" , m_evtSvc );
  if( sc.isFailure() ) { return Error("Could not locate EventDataSvc!", sc );}
  if( 0 == m_evtSvc  ) { return Error("Could not locate EventDataSvc!"     );}
  m_evtSvc->addRef() ;
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** standard finalization of the tool
 *  @see IAlgTool
 *  @see  AlgTool
 *  @see CaloTool
 *  @return status code 
 */
// ============================================================================
StatusCode CaloExtraDigits::finalize () 
{
  // release eventservice 
  if( 0 != m_evtSvc ) { m_evtSvc->release() ; m_evtSvc = 0 ; }
  // release incident service 
  if( 0 != m_incSvc ) { m_incSvc->release() ; m_incSvc = 0 ; }
  // finilize the base class 
  return CaloTool::finalize () ;
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
StatusCode CaloExtraDigits::process    ( CaloHypo* hypo  ) const 
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
StatusCode CaloExtraDigits::operator() ( CaloHypo* hypo  ) const 
{ 
  
  // valid hypo?
  if( 0 == hypo        ) { return Error("CaloHypo* points to NULL!" , 200 ) ; }
  const CaloPosition* position = hypo->position() ;
  if( 0 == position    ) { return Error("CaloPosition* is invalid!" , 201 ) ; }

  // locate digits 
  if( 0 == m_digits ) { m_digits = get( m_evtSvc , m_inputData , m_digits ); }
  
  // add the digits in front of seed digit of Ecal cluster
  if( m_addSeed ) 
    {
      // find ecal cluster 
      const CaloHypo::Clusters& clusters = hypo->clusters();
      if( clusters.empty() ) 
        {                  return Error("No clusters are found!"    , 202 ) ; }
      // get clusters from 'Ecal'
      CaloHypo::Clusters::const_iterator ecalCl = 
        std::find_if( clusters.begin() , clusters.end() , m_selector );
      if( clusters.end() == ecalCl ) 
        {                  return Error("No valid 'Ecal' cluster!"  , 203 ) ; }
      const CaloCluster::Entries& entries = (*ecalCl)->entries();
      CaloCluster::Entries::const_iterator seed = 
        ClusterFunctors::
        locateDigit( entries.begin ()          , 
                     entries.end   ()          ,
                     CaloDigitStatus::SeedCell ) ;
      if( entries.end() == seed ) 
        {                  return Error("No 'Seed' in cluster!"     , 204 ) ; }
      const CaloDigit* seedD = seed->digit() ;
      if( 0 == seedD ) {   return Error("'Seed' points to NULL!"    , 205 ) ; }
      CaloDigit* digit = m_digits->object( seedD->cellID() );
      if( 0 != digit ) { hypo->addToDigits( digit ); }
    }
  
  
  // line along the photon flight 
  const  HepPoint3D pos( position->x() ,
                         position->y() ,
                         position->z() );
  
  // point in the detector 
  HepPoint3D newPoint ( pos - m_vertex );
  newPoint *=  ( m_z - m_vertex.z() ) / ( pos.z() - m_vertex.z() );
  newPoint +=          m_vertex ;
  
  const CaloCellID cell = det()->Cell( newPoint );
  
  if( CaloCellID() != cell )  // valid cell! 
    {    
      CaloDigit* digit = m_digits->object( cell );
      if( 0 != digit ) { hypo->addToDigits( digit ); }
    }
  
  // loop over neighbour cells 
  if( (  0 < m_xTol || 0 < m_yTol ) && CaloCellID() != cell )
    {
      const CaloNeighbors& neighbors = det()->neighborCells( cell );
      for( CaloNeighbors::const_iterator nei = neighbors.begin() ; 
           neighbors.end() != nei ; ++nei )
        {
          const double      cellHalf   = 0.5 * det() -> cellSize   ( *nei ) ;
          if( 0 >= cellHalf ) { continue ; }               // CONTINUE ! 
          const HepPoint3D& cellCenter =       det() -> cellCenter ( *nei ) ;
          if( ( abs( cellCenter.x() - newPoint.x() ) < cellHalf + m_xTol ) && 
              ( abs( cellCenter.y() - newPoint.y() ) < cellHalf + m_yTol )  ) 
            {
              CaloDigit* digit = m_digits->object( *nei );
              if( 0 != digit ) { hypo->addToDigits( digit ); }
            }    
        }
    };
  
  {
    // remove duplicates (if any)
    CaloHypo::Digits& digits = hypo->digits() ;
    std::sort( digits.begin() , digits.end() , std::less<const CaloDigit*>() );
    CaloHypo::Digits::iterator i = 
      std::unique( digits.begin() , digits.end() );
    if( digits.end() != i ) { digits.erase( i , digits.end() ) ; }
  }
  
  
  return StatusCode::SUCCESS ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
