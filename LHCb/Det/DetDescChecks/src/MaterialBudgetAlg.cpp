// $Id: MaterialBudgetAlg.cpp,v 1.7 2004-03-01 15:03:44 ibelyaev Exp $
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
#include "CLHEP/Units/PhysicalConstants.h"
// AIDA 
#include "AIDA/IHistogram1D.h"
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
#include "DirHbookName.h"

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

// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc service locator 
 */
// ============================================================================
MaterialBudgetAlg::MaterialBudgetAlg
( const std::string& name    ,
  ISvcLocator*       svc     )
  : GaudiHistoAlg   ( name , svc     ) 
  , m_trSvcName     ( "TransportSvc" )
  , m_trSvc         ( 0              )  
  , m_rndmSvcName   ( "RndmGenSvc"   )
  , m_rndmSvc       ( 0              )
  , m_vrtx          ( 3 , 0.0        )
  , m_vertex        (                )
  , m_shots         ( 1000           )
  , m_z             ( 12 * meter     )
  , m_xMax          (  4 * meter     )
  , m_yMax          (  3 * meter     )
  , m_xMin          (  0 * meter     )
  , m_yMin          (  0 * meter     )
  , m_etaMax        (  5.5           )
  , m_phiMax        (  270.          )
  , m_etaMin        (  2.0           )
  , m_phiMin        (  90.           )
  , m_nbx           ( 360            )
  , m_nby           ( 300            )
  , m_grid          (  0             )
  , m_psrap         (  0             )
  , m_xbinref       ( 20.0           )
  , m_ybinref       ( 20.0           )
  , m_zref          ( 10000.0        )
  , m_budget2D      (  0             )
  , m_dist2DXY      (  0             )
  , m_budget1DX     (  0             )
  , m_budget1DY     (  0             )
  , m_budget3       (  0             )
  , m_budget4       (  0             )
  , m_normalization (  0             )
{  
  declareProperty( "TransportService" , m_trSvcName   ) ;
  declareProperty( "RndmService"      , m_rndmSvcName ) ;
  declareProperty( "ShootingPoint"    , m_vrtx        ) ;
  declareProperty( "Shots"            , m_shots       ) ;
  declareProperty( "zPlane"           , m_z           ) ;
  declareProperty( "xMax"             , m_xMax        ) ;
  declareProperty( "xMin"             , m_xMin        ) ;
  declareProperty( "yMax"             , m_yMax        ) ;
  declareProperty( "yMin"             , m_yMin        ) ;
  declareProperty( "etaMax"           , m_etaMax      ) ;
  declareProperty( "etaMin"           , m_etaMin      ) ;
  declareProperty( "phiMax"           , m_phiMax      ) ;
  declareProperty( "phiMin"           , m_phiMin      ) ;
  declareProperty( "nBx"              , m_nbx         ) ;
  declareProperty( "nBy"              , m_nby         ) ;
  declareProperty( "Grid"             , m_grid        ) ;
  declareProperty( "Rapidity"         , m_psrap       ) ;
  declareProperty( "xbinref"          , m_xbinref     ) ;
  declareProperty( "ybinref"          , m_ybinref     ) ;
  declareProperty( "zref"             , m_zref        ) ;
};
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
MaterialBudgetAlg::~MaterialBudgetAlg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization of the algorithm
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode MaterialBudgetAlg::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_trSvc   = svc<ITransportSvc> ( m_trSvcName   , true ) ;
  m_rndmSvc = svc<IRndmGenSvc>   ( m_rndmSvcName , true ) ;
  
  
  // activate the vertex
  if( m_vrtx.size() <= 3 )
  { while( 3 != m_vrtx.size() ) { m_vrtx.push_back( 0.0 ); } }
  else 
  { warning() << " Ignore extra fields in 'ShootingPoint' "<< endreq ; }
  m_vertex.setX( m_vrtx[0] ) ;
  m_vertex.setY( m_vrtx[1] ) ;
  m_vertex.setZ( m_vrtx[2] ) ;
  
  // transform parameters 
  if( m_xMin >  m_xMax ) {  std::swap( m_xMin , m_xMax )  ; }
  if( m_yMin >  m_yMax ) {  std::swap( m_yMin , m_yMax )  ; }
  // adjust number of bins 
  if( 0      >= m_nbx  ) {  m_nbx = 50                    ; }
  if( 0      >= m_nby  ) {  m_nby = 50                    ; }
  
  if( m_grid != 0 && m_psrap != 0 ) 
  {
    return Error(" Asked for X-Y and Eta-Phi scans ");
  }
  
  // for grid we calculate x/yMin/Max from the size and number of bins 
  if(m_grid)
  {
    m_xMin=-(m_xbinref*m_nbx*m_z)/(2.0*m_zref);
    m_yMin=-(m_ybinref*m_nby*m_z)/(2.0*m_zref);
    m_xMax=-m_xMin;
    m_yMax=-m_yMin;
  }
  
  if (m_psrap){
    m_xMin=m_etaMin;
    m_yMin=m_phiMin;
    m_xMax=m_etaMax;
    m_yMax=m_phiMax;
  }

  
  m_budget2D = 
    histoSvc()->book( histoPath()                         ,   // directory 
                      1                                   ,   // ID 
                      "Material Budget"                   ,   // title 
                      m_nbx  ,  m_xMin  ,  m_xMax         ,   // x-bins 
                      m_nby  ,  m_yMin  ,  m_yMax         ) ; // y-bins
  if ( m_psrap ) {
    m_dist2DXY = 
      histoSvc()->book( histoPath()                      ,   // directory 
                        2                                ,   // ID 
                        "Material Budget check"          ,   // title 
                        m_nbx  ,  -600.  ,  600.         ,   // x-bins 
                        m_nby  ,  -600   ,  600.         ) ; // y-bins
  }
  m_budget1DX =
    book ( 3  , "Material Budget Eta" ,   // ID, title 
           m_yMin  ,  m_yMax , m_nby  ) ; // x-bins 
  
  m_budget1DY = 
    book ( 4 , "Material Budget Phi"  ,   // ID , title 
           m_xMin  ,  m_xMax , m_nbx ) ; // x-bins 
  
  if( 0 == m_budget2D ) 
  { return Error(" Could not book 2D Material Budget Histogram " ); }
  
  if ( m_psrap ) {
    if( 0 == m_dist2DXY ) 
    { return Error(" Could not book 2D shot Histogram ") ;}
  }
  if( 0 == m_budget1DX ) 
  { return Error(" Could not book 1D Material Budget X Histogram "); }
  if( 0 == m_budget1DY ) 
  { return Error(" Could not book 1D Material Budget Y Histogram "); }

  if(!m_grid && !m_psrap) 
    // we do not need normalization histogram for grid
    // because we have exactly one shot per bin  
  {
    // book the histogram
    m_normalization = 
      histoSvc()->book( histoPath()                         ,   // directory 
                        2                                   ,   // ID 
                        "Normalization for Material Budget" ,   // title 
                        m_nbx  ,  m_xMin  ,  m_xMax         ,   // x-bins 
                        m_nby  ,  m_yMin  ,  m_yMax         ) ; // y-bins
    if( 0 == m_normalization ) 
    { return Error (" Could not book the Material Budget Normalization"); }
  }
  
  return StatusCode::SUCCESS;
};
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
  
  // shooting a on grid:
  if(m_grid || m_psrap)
  { 
    double aveDist=0.;
    double dxgrid=0.;
    double dygrid=0.;
    // put in a transformation to go from XY to Eta-Phi.
    if ( ! m_psrap ) {
      dxgrid=m_xbinref*m_z/m_zref;
      dygrid=m_ybinref*m_z/m_zref;
    }
    else {
      dxgrid=(m_xMax-m_xMin)/m_nbx;
      dygrid=(m_yMax-m_yMin)/m_nby;
    }
    // xx and yy refer to the two non-Z dimensions, be them cartesian or 
    // whatever. x and y are cartesian.
    double xx=m_xMin+dxgrid/2;
    double yy=m_yMin+dygrid/2;
    double x = 0;
    double y = 0;
    int shot = 0;
    
    double* storeEtaValues;
    storeEtaValues = (double*) malloc(m_nbx*sizeof(double));
    
    for ( int ii = 0 ; ii < m_nbx ; ii++ ) {
      storeEtaValues[ii]=0.;
    }
    while(yy<=m_yMax)
    {
      double addPhiSlice=0.;
      int phiShot=0;
      while(xx<=m_xMax)
	    {
	      if ( m_psrap ) {
          double theta = 2.0*atan(exp(-1.0*xx));
          double phi = yy*degree;
          // make sure theta in not 90 or 270!!!!
          x = sin(theta)*cos(phi)*m_z/cos(theta);
          y = sin(theta)*sin(phi)*m_z/cos(theta);
	      }
	      else if ( m_grid ) {
          x = xx;
          y = yy;
	      }
	      shot++;
	      if( 0 == shot%int(m_shots/10) )
	      { info() << " Grid shot # " 
                 << shot          << "/" << m_nbx*m_nby << endreq ; }
        // "shooting" point at the reference plane
        const HepPoint3D point( x, y, m_z);       
        
        // evaluate the distance 
        const double dist = 
          m_trSvc -> distanceInRadUnits( m_vertex , point );
        
        // fill material budget histogram
	      if ( m_grid ) {
          m_budget2D        -> fill( point.x() , point.y() , dist ) ;
	      }
	      else if ( m_psrap ) {
          m_budget2D        -> fill( xx , yy , dist ) ;
          m_dist2DXY        -> fill( x , y , 1. ) ;
	      }
	      aveDist = aveDist + dist;
	      addPhiSlice=addPhiSlice+dist;
	      storeEtaValues[phiShot]=storeEtaValues[phiShot]+dist;
	      phiShot++;
        //move to the next point on the grid
        xx=xx+dxgrid;
        
      }
      m_budget1DX        -> fill( yy , addPhiSlice ) ;
      xx=m_xMin+dxgrid/2;
      yy=yy+dygrid;
    }
    aveDist = aveDist/shot;
    xx=m_xMin+dxgrid/2;
    for ( int iii = 0 ; iii < m_nbx ; iii++ ) {
      m_budget1DY        -> fill( xx , storeEtaValues[iii] ) ;
      xx=xx+dxgrid;
    }
    info()  << " total shots " << shot << endreq
            << " Average radiation length = "
            << aveDist*100 << "%" << endreq;
    delete[] storeEtaValues;
  }
  else // random shooting:
  {
    // get random number generator 
    Rndm::Numbers flat( m_rndmSvc , Rndm::Flat( 0. , 1. ) );
    
    const double dx = m_xMax - m_xMin ;
    const double dy = m_yMax - m_yMin ;
    
    // make 'shots'
    for( int shot = 0 ; shot < m_shots ; ++shot ) 
    {
      // point at reference plane  
      const HepPoint3D point( m_xMin + dx * flat.shoot() ,
                              m_yMin + dy * flat.shoot() ,
                              m_z                        );      
      // evaluate the distance 
      const double dist = 
        m_trSvc -> distanceInRadUnits( m_vertex , point );
      
      // fill material budget histogram 
      m_budget2D        -> fill( point.x() , point.y() , dist ) ;
      // fill the normalization histogram  
      m_normalization -> fill( point.x() , point.y()        ) ; 
    }
  }
  // 
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================

