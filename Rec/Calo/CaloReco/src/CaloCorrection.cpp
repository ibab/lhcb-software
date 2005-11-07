// $Id: CaloCorrection.cpp,v 1.2 2005-11-07 12:12:42 odescham Exp $
// Include files 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:41  ibelyaev
// new package 
//
// Revision 1.2  2002/06/13 12:32:38  ibelyaev
//  version update
//
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
// CaloKernel 
#include "CaloKernel/OSiterator.h"
// CaloEvent/Event
#include "Event/CaloPosition.h"
// local
#include "CaloCorrection.h"

// ============================================================================
/** @file CaloCorrection.cpp
 * 
 *  Implementation file for class : CaloCorrection
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 22/03/2002 
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param  type   type of the tool (?)
 *  @param  name   the name of this concrete instance 
 *  @param  parent the tool's parent
 *  @param  fx     function for correction of x-coordinate 
 *  @param  fxp    derivative of function for correction of x-coordinate 
 *  @param  fy     function for correction of y-coordinate 
 *  @param  fyp    derivative of function for correction of y-coordinate    
 */
// ============================================================================
CaloCorrection::CaloCorrection( const std::string&        type   ,
                                const std::string&        name   ,
                                const IInterface*         parent ,
                                CaloCorrection::Function  fx     ,
                                CaloCorrection::Function  fxp    ,
                                CaloCorrection::Function  fy     ,
                                CaloCorrection::Function  fyp    )
  : CaloTool ( type, name , parent ) 
  , m_xpars   (         ) 
  , m_ypars   (         ) 
  , m_cell    ( -1      )
  , m_fx      ( fx      )
  , m_fxp     ( fxp     )
  , m_fy      ( fy      )
  , m_fyp     ( fyp     )
  , m_z       ( -1 * km )
  , m_updateZ ( false   )
{
  declareInterface<ICaloCorrection> ( this   );
  declareProperty( "XParameters" , m_xpars   );
  declareProperty( "YParameters" , m_ypars   );
  declareProperty( "CellSize"    , m_cell    );
  declareProperty( "NewZ"        , m_z       );
  declareProperty( "UpdateZ"     , m_updateZ );
};

// ============================================================================
/** destructor, virtual of course 
 */
// ============================================================================
CaloCorrection::~CaloCorrection(){};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCorrection::initialize() 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloTool!" , sc ) ; }
  // 
  if( 0 == m_fx || 0 == m_fxp )
    { return  Error("Function/Derivative  for X-correction is invalid!");}
  if( 0 == m_fy || 0 == m_fyp ) 
    { 
      Warning("Use the same functions/derivative for X and Y corrections!");
      m_fy    = m_fx  ;
      m_fyp   = m_fyp ;
    };
  if( m_xpars.empty()     )
    { return  Error("Function for Y-correction is not specified!");}
  if( m_ypars.empty()     )
    { 
      Warning("Use the same parameters for X and Y corrections!");
      m_ypars = m_xpars ;
    }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** The main processing method 
 *  @see ICaloCorrection
 *  @param  position  uncorrected position
 *  @param  seed      position of the 'seed' 
 *  @return new created corrected CaloPosition object 
 */  
// ============================================================================
CaloPosition* CaloCorrection::process    
( const CaloPosition & position , 
  const HepPoint3D   & seed     ) const 
{ return (*this)( position , seed ); }

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloCorrection
 *  @param  position  uncorrected position
 *  @param  seed      position of the 'seed' 
 *  @return new created corrected CaloPosition object 
 */  
// ============================================================================
CaloPosition* CaloCorrection::operator() 
  ( const CaloPosition & position , 
    const HepPoint3D   & seed     ) const 
{
  CaloPosition* pos = 0 ;
  StatusCode sc = process( position , seed , pos );
  // ok ? 
  if( sc.isSuccess() && 0 != pos ) { return pos ; }    // RETURN!
  // error condition 
  Error(" 'operator()': status code from 'process()' " , sc );
  if( 0 != pos ) { delete pos ; pos = 0 ; }
  // 
  return 0 ;
};

// ============================================================================
/** The main processing method 
 *  @see ICaloCorrection
 *  @param  position  uncorrected position
 *  @param  seed      position of the 'seed' 
 *  @param  corrected new created corrected CaloPosition object 
 *  @return status code 
 */  
// ============================================================================
StatusCode     CaloCorrection::process    
( const CaloPosition & position  , 
  const HepPoint3D   & seed      , 
  CaloPosition*&       corrected ) const 
{
  // avoid long names! 
  typedef CaloPosition CP;
  // create new position object 
  if( 0 == corrected ) { corrected = new CaloPosition( position ) ; }
  
  // x-position 
  const double x = (*m_fx ) ( position.x() , seed.x() , m_cell , m_xpars );
  // y-position 
  const double y = (*m_fy ) ( position.y() , seed.y() , m_cell , m_ypars );
  
  // correct position
  CP::Parameters&       parCor = corrected -> parameters () ;
  const CP::Parameters& parPos = position  .  parameters () ;
  parCor( CP::E ) = parPos( CP::E ) ;
  parCor( CP::X ) =             x   ;
  parCor( CP::Y ) =             y   ;
  
  // update Z 
  if( m_updateZ ) { corrected->setZ( m_z ); }
  
  // derivatives:
  double fxdx = (*m_fxp )( position.x() , seed.x() , m_cell , m_xpars );  
  double fydy = (*m_fyp )( position.y() , seed.y() , m_cell , m_ypars );
  // other derivatives are ZEROes, except fede = 1  
  
  // covariance 
  CP::Covariance&       covCor = corrected -> covariance () ;
  const CP::Covariance& covPos = position  .  covariance () ;
  covCor( CP::E , CP::E ) =               covPos( CP::E , CP::E ) ;
  covCor( CP::E , CP::X ) =        fxdx * covPos( CP::E , CP::X ) ;
  covCor( CP::E , CP::Y ) =        fydy * covPos( CP::E , CP::Y ) ;
  covCor( CP::X , CP::X ) = fxdx * fxdx * covPos( CP::X , CP::X ) ;
  covCor( CP::X , CP::Y ) = fxdx * fydy * covPos( CP::X , CP::Y ) ;
  covCor( CP::Y , CP::Y ) = fydy * fydy * covPos( CP::Y , CP::Y ) ;
  
  return StatusCode::SUCCESS ;

};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================
