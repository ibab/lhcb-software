// $Id: MaterialBudgetAlg.cpp,v 1.12 2009-11-15 16:46:00 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// STL & STD 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/VectorsAsProperty.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/ITransportSvc.h"
// ============================================================================
// local
// ============================================================================
#include "MaterialBudgetAlg.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/progress.hpp"
// ============================================================================
/** @file MaterialBudgetAlg.cpp
 *  
 *  Implementation file for class MaterialBudgetAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 23/04/2002 
 */
// ============================================================================
/*  Standard constructor
 *  @param name name of the algorithm
 *  @param svc service locator 
 */
// ============================================================================
DetDesc::MaterialBudget::MaterialBudget
( const std::string& name    ,
  ISvcLocator*       svc     )
  : GaudiHistoAlg   ( name , svc     ) 
  , m_trSvcName     ( "TransportSvc" )
  , m_trSvc         ( 0              )  
  , m_vertex        (                )
  , m_shots         ( 1000           )
  , m_z             ( 12 * Gaudi::Units::meter     )
  , m_xMax          (  4 * Gaudi::Units::meter     )
  , m_yMax          (  3 * Gaudi::Units::meter     )
  , m_xMin          (  0 * Gaudi::Units::meter     )
  , m_yMin          (  0 * Gaudi::Units::meter     )
  , m_etaMax        (  5.5           )
  , m_phiMax        (  270.          )
  , m_etaMin        (  2.0           )
  , m_phiMin        (  90.           )
  , m_nbx           ( 360            )
  , m_nby           ( 300            )
  , m_grid          (  true          )
  , m_psrap         (  false         )
  , m_xbinref       ( 20.0           )
  , m_ybinref       ( 20.0           )
  , m_zref          ( 10000.0        )
{  
  declareProperty ( "TransportService" , m_trSvcName   ) ;
  declareProperty 
    ( "ShootingPoint"    , 
      m_vertex           , 
      "Position of shooting vertex") ;
  declareProperty
    ( "Shots"            , 
      m_shots            , 
      "Number of random short per event") ;
  declareProperty 
    ( "zPlane"           , 
      m_z                , 
      "Z-position of reference plane") ;
  declareProperty 
    ( "xMax"             , 
      m_xMax             , 
      "Maximal X at reference plane" ) ;
  declareProperty 
    ( "xMin"             , 
      m_xMin             , 
      "Minimal X at reference plane" ) ;
  declareProperty 
    ( "yMax"             ,
      m_yMax             , 
      "Maximal Y at reference plane" ) ;
  declareProperty
    ( "yMin"             , 
      m_yMin             , 
      "Minimal Y at reference plane" ) ;
  declareProperty ( "etaMax"           , m_etaMax      ) ;
  declareProperty ( "etaMin"           , m_etaMin      ) ;
  declareProperty ( "phiMax"           , m_phiMax      ) ;
  declareProperty ( "phiMin"           , m_phiMin      ) ;
  declareProperty ( "nBx"              , m_nbx         ) ;
  declareProperty ( "nBy"              , m_nby         ) ;
  declareProperty ( "Grid"             , m_grid        ) ;
  declareProperty ( "Rapidity"         , m_psrap       ) ;
  declareProperty ( "xbinref"          , m_xbinref     ) ;
  declareProperty ( "ybinref"          , m_ybinref     ) ;
  declareProperty ( "zref"             , m_zref        ) ;
}
// ============================================================================
// destructor
// ============================================================================
DetDesc::MaterialBudget::~MaterialBudget () {}
// ============================================================================
/*  standard initialization of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::MaterialBudget::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  m_trSvc   = svc<ITransportSvc> ( m_trSvcName   , true ) ;
  
  Assert ( 0 != randSvc() , "IRndmGenSvc* poitns to NULL!" );
  
  // transform parameters 
  if ( m_xMin >  m_xMax ) {  std::swap( m_xMin , m_xMax )  ; }
  if ( m_yMin >  m_yMax ) {  std::swap( m_yMin , m_yMax )  ; }
  // adjust number of bins 
  if ( 0      >= m_nbx  ) {  m_nbx = 50                    ; }
  if ( 0      >= m_nby  ) {  m_nby = 50                    ; }
  
  if ( m_grid  && m_psrap ) 
  {
    return Error(" Asked for X-Y and Eta-Phi scans ");
  }
  
  // for grid we calculate x/yMin/Max from the size and number of bins 
  if ( m_grid )
  {
    m_xMin=-(m_xbinref*m_nbx*m_z)/(2.0*m_zref);
    m_yMin=-(m_ybinref*m_nby*m_z)/(2.0*m_zref);
    m_xMax=-m_xMin;
    m_yMax=-m_yMin;
  }
  
  if ( m_psrap)
  {
    m_xMin=m_etaMin;
    m_yMin=m_phiMin;
    m_xMax=m_etaMax;
    m_yMax=m_phiMax;
  }
  
  return StatusCode::SUCCESS;
}
// ============================================================================
/*  standard execution of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode DetDesc::MaterialBudget::execute() 
{
  
  if      ( m_grid  ) { return makeGridShots  () ; }
  else if ( m_psrap ) { return makePsrapShots () ; }
  
  return makeRandomShots() ;
}
// ============================================================================
// make a random shoots
// ============================================================================
StatusCode DetDesc::MaterialBudget::makeRandomShots() 
{
  
  // get random number generator 
  Rndm::Numbers x ( randSvc() , Rndm::Flat( m_xMin , m_xMax ) );
  Rndm::Numbers y ( randSvc() , Rndm::Flat( m_yMin , m_yMax ) );
  
  // make 'shots'
  boost::progress_display progress ( m_shots ) ;
  for ( int shot = 0 ; shot < m_shots ; ++shot , ++progress ) 
  {
    // point at reference plane  
    const Gaudi::XYZPoint point( x() , y() , m_z );      
    // evaluate the distance 
    const double dist = 
      m_trSvc -> distanceInRadUnits ( m_vertex , point );
    
    // fill material budget histogram 
    plot2D ( point.x()   , point.y()              , 
             1           , "Material Budget"      , 
             m_xMin      , m_xMax                 , 
             m_yMin      , m_yMax                 , 
             m_nbx       , m_nby                  , 
             dist                                 ) ; // weight 
    // fill the normalization histogram  
    plot2D ( point.x()   , point.y()              , 
             2           , "Budget Normalization" , 
             m_xMin      , m_xMax                 , 
             m_yMin      , m_yMax                 , 
             m_nbx       , m_nby                  ) ;
    
  }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make grid shoots
// ============================================================================
StatusCode DetDesc::MaterialBudget::makeGridShots() 
{
  if ( !m_grid ) { return StatusCode::FAILURE ; }
  
  // put in a transformation to go from XY to Eta-Phi.
  const double dxgrid = m_xbinref * m_z / m_zref;
  const double dygrid = m_ybinref * m_z / m_zref;
  
  // xx and yy refer to the two non-Z dimensions, be them cartesian or 
  // whatever. x and y are cartesian.
  
  /// make a progress bar 
  boost::progress_display progress( m_nbx * m_nby ) ;
  
  for ( double y = m_yMin + dygrid/2 ; y <= m_yMax ; y += dygrid ) 
  {
    for ( double x = m_yMin + dxgrid/2 ; x <= m_xMax ; x += dxgrid )
    {
      // "shooting" point at the reference plane
      const Gaudi::XYZPoint point ( x, y, m_z);       
      
      // evaluate the distance 
      const double dist = 
        m_trSvc -> distanceInRadUnits ( m_vertex , point );
      
      // fill material budget histogram 
      plot2D ( point.x()   , point.y()              , 
               1           , "Material Budget"      , 
               m_xMin      , m_xMax                 , 
               m_yMin      , m_yMax                 , 
               m_nbx       , m_nby                  , 
               dist                                 ) ; // weight 
      // fill the "normalization" histogram  (must be flat)
      plot2D ( point.x()   , point.y()              , 
               2           , "Budget Normalization" , 
               m_xMin      , m_xMax                 , 
               m_yMin      , m_yMax                 , 
               m_nbx       , m_nby                  ) ;
      
      // show the progress 
      ++progress ;
    }
  }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make grid shoots
// ============================================================================
StatusCode DetDesc::MaterialBudget::makePsrapShots()
{
  if ( !m_psrap ) { return StatusCode::FAILURE ; }
  
  // put in a transformation to go from XY to Eta-Phi.
  const double dxgrid = (m_xMax-m_xMin)/m_nbx;
  const double dygrid = (m_yMax-m_yMin)/m_nby;
  
  /// make a pregress bar 
  boost::progress_display progress ( m_nbx * m_nby ) ;  
  for ( double yy = m_yMin + dygrid/2 ; yy <= m_yMax ; yy += dygrid ) 
  {
    for ( double xx = m_yMin + dxgrid/2 ; xx <= m_xMax ; xx += dxgrid )
    {
      const double theta = 2.0*atan(exp(-1.0*xx));
      const double phi = yy*Gaudi::Units::degree;
      // make sure theta in not 90 or 270!!!!
      const double x = sin(theta)*cos(phi)*m_z/cos(theta);
      const double y = sin(theta)*sin(phi)*m_z/cos(theta);
      
      // "shooting" point at the reference plane
      const Gaudi::XYZPoint point( x, y, m_z);       
      
      // evaluate the distance 
      const double dist = 
        m_trSvc -> distanceInRadUnits( m_vertex , point );
      
      // fill material budget histogram 
      plot2D ( xx          , yy                      , 
               1           , "Material Budget"       , 
               m_xMin      , m_xMax                  , 
               m_yMin      , m_yMax                  , 
               m_nbx       , m_nby                   , 
               dist                                  ) ; // weight 
      // fill the "helper" histogram 
      plot2D ( x           , y                       , 
               2           , "Material Budget Check" , 
               -600        , 600                     , 
               -600        , 600                     , 
               m_nbx       , m_nby                   ) ;
      
      // show the progress 
      ++progress ;
    }
  }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DetDesc,MaterialBudget)
// ============================================================================
// The End 
// ============================================================================

