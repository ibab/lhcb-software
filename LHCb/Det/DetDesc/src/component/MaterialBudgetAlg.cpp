// $Id: MaterialBudgetAlg.cpp,v 1.2 2002-05-04 13:13:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// STL & STD 
#include <functional>
#include <algorithm>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Vector3D.h"
// AIDA 
#include "AIDA/IHistogram2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// DetDesc 
#include "DetDesc/ITransportSvc.h"
// local
#include "MaterialBudgetAlg.h"

// ============================================================================
/** @file MaterialBudgetAlg.cpp
 *  
 *  Implementation file for class MaterialBudgetAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 23/04/2002 
 */
// ============================================================================

// ============================================================================
/** @var MaterialBudgetAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<MaterialBudgetAlg>         s_factory ;
const        IAlgFactory&MaterialBudgetAlgFactory = s_factory ;
// ============================================================================

namespace Local
{
  /** @fn dirHbookName
   *  
   *  The function is imported from Calo/CaloUtils package 
   * 
   *  simple function to convert any valid Gaudi 
   *  address(name in Transient Store)
   *  to address, which is simultaneously valid for Hbook directory
   *
   *    examples:  
   *   "verylongname"  -->  "verylong/name"
   *
   *  @param   old    old address 
   *  @param   maxLen maximum allowed length of directory name (8 for Hbook) 
   *  @return  new  address 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   06/07/2001
   */  
  inline std::string dirHbookName 
  ( const std::string& addr       , 
    const int          maxLen = 8 )
  {
    // ignore empty locations 
    if( addr.empty() ) { return std::string(); }
    //
    std::string old( addr );
    // remove long names
    if( 0 < maxLen &&  maxLen < (int) old.size() ) 
      { 
        std::string::iterator p1,p2;
        p1 = old.begin();
        const char sep('/');
        while( old.end() != p1 )
          {
            p1 = 
              std::find_if( p1        , 
                            old.end() , 
                            std::bind2nd(std::not_equal_to<char>(),sep));
            p2 = std::find( p1 , old.end() , sep ) ;
            if( ( p2 - p1 ) <= (int) maxLen  ) { p1 = p2 ; continue ; }
            old.insert( p1 + maxLen , sep ) ;  
            p1 = old.begin() ; 
          }
      }
    ///
    return old;
  }; 
 
};

// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc service locator 
 */
// ============================================================================
MaterialBudgetAlg::MaterialBudgetAlg
( const std::string& name    ,
  ISvcLocator*       svc     )
  : Algorithm       ( name , svc     ) 
  , m_trSvcName     ( "TransportSvc" )
  , m_trSvc         ( 0              )  
  , m_rndmSvcName   ( "RndmGenSvc"   )
  , m_rndmSvc       ( 0              )
  , m_vrtx          ( 3 , 0.0        )
  , m_vertex        (                )
  , m_shoots        ( 1000           )
  , m_z             (  1   * meter   )
  , m_xsize         (  0.5 * meter   )
  , m_ysize         (  0.5 * meter   )
  , m_budget        (  0             )
  , m_normalization (  0             )
{  
  declareProperty( "TransportService" , m_trSvcName   ) ;
  declareProperty( "RndmService"      , m_rndmSvcName ) ;
  declareProperty( "ShootingPoint"    , m_vrtx        ) ;
  declareProperty( "Shoots"           , m_shoots      ) ;
  declareProperty( "zPlane"           , m_z           ) ;
  declareProperty( "xSize"            , m_xsize       ) ;
  declareProperty( "zSize"            , m_ysize       ) ;  
};
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
MaterialBudgetAlg::~MaterialBudgetAlg() {}; 

// ============================================================================
/** standard initialization of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode MaterialBudgetAlg::initialize() 
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  // locate transport service 
  StatusCode sc = service ( m_trSvcName , m_trSvc   , true ) ;
  if( sc.isFailure()   ) 
    {
      log << MSG::ERROR 
          << " Could not locate ITransportSvc = '"
          << m_trSvcName << "'" << endreq ;
      return sc                  ; 
    }
  if( 0 == m_trSvc     ) 
    { 
      log << MSG::ERROR 
          << " Could not locate ITransportSvc = '"
          << m_trSvcName << "'" << endreq ;
      return StatusCode::FAILURE ; 
    }
  m_trSvc   -> addRef() ;
  
  // locate random numbers service 
  sc =            service ( m_rndmSvcName   , m_rndmSvc , true ) ;
  if( sc.isFailure()   )
    {
      log << MSG::ERROR 
          << " Could not locate IRndmGenSvc = '"
          << m_rndmSvcName << "'" << endreq ;
      return sc                  ; 
    }
  if( 0 == m_rndmSvc   ) 
    { 
      log << MSG::ERROR 
          << " Could not locate IRndmGenSvc = '"
          << m_rndmSvcName << "'" << endreq ;
      return StatusCode::FAILURE ; 
    }
  m_rndmSvc -> addRef() ;
  
  // activate the vertex
  if( m_vrtx.size() <= 3 )
    { while( 3 != m_vrtx.size() ) { m_vrtx.push_back( 0.0 ); } }
  else 
    {
      log << MSG::WARNING 
          << " Ignore extra fields in 'ShootingPoint' "<< endreq ;
    }
  m_vertex.setX( m_vrtx[0] ) ;
  m_vertex.setY( m_vrtx[1] ) ;
  m_vertex.setZ( m_vrtx[2] ) ;
  
  // book the histogram
  const std::string hbookdir( Local::dirHbookName( "/stat/" + name() ) );
  m_budget = 
    histoSvc()->book( hbookdir                            ,   // directory 
                      1                                   ,   // ID 
                      "Material Budget"                   ,   // title 
                      100 , -1 * m_xsize , m_xsize        ,   // x-bins 
                      100 , -1 * m_ysize , m_ysize        ) ; // y-bins
  if( 0 == m_budget ) 
    {
      log << MSG::ERROR 
          << " Coult not book the Material Budget Histogram!"
          << endreq ;
      return StatusCode::FAILURE ; 
    }
  // book the histogram
  m_normalization = 
    histoSvc()->book( hbookdir                            ,   // directory 
                      2                                   ,   // ID 
                      "Normalization for Material Budget" ,   // title 
                      100 , -1 * m_xsize , m_xsize        ,   // x-bins 
                      100 , -1 * m_ysize , m_ysize        ) ; // y-bins
  if( 0 == m_normalization ) 
    {
      log << MSG::ERROR 
          << " Could not book the Material Budget Normalization Histogram!"
          << endreq ;
      return StatusCode::FAILURE ; 
    }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
/** standard finalization of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode MaterialBudgetAlg::finalize()
 {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  // release used services 
  if( 0 != m_trSvc   ) { m_trSvc   -> release() ; m_trSvc   = 0 ; }
  if( 0 != m_rndmSvc ) { m_rndmSvc -> release() ; m_rndmSvc = 0 ; }
  
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** standard execution of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode MaterialBudgetAlg::execute() 
{
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( -1. , 1. ) );
  
  int n = m_shoots ;
  while( n-- > 0 )
    {
      
      const HepPoint3D point( m_xsize * flat.shoot() , 
                              m_ysize * flat.shoot() , m_z );
      
      const double dist = m_trSvc -> distanceInRadUnits( m_vertex , point );
      
      m_budget        -> fill( point.x() , point.y() , dist ) ;
      m_normalization -> fill( point.x() , point.y()        ) ; 
    }
  // 
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================

