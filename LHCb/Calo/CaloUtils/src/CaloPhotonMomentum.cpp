// $Id: CaloPhotonMomentum.cpp,v 1.3 2005-11-07 11:57:13 odescham Exp $
// =============================================================================
// CVS tag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/06/13 12:33:52  ibelyaev
//  bug fixes
//
// Revision 1.1  2002/04/02 10:59:31  ibelyaev
//  update for new event model
// 
// =============================================================================
// Include files
#include "CaloKernel/CaloException.h"
// CaloEvent/Event 
#include "Event/CaloPosition.h"
#include "Event/CaloMomentum.h"
// local
#include "CaloUtils/CaloPhotonMomentum.h"

// =============================================================================
/** @file CaloPhotonMomentum.cpp
 *
 * Implementation file for class : CaloPhotonMomentum
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 21/03/2002 
 */
// =============================================================================

// =============================================================================
/** The only one essential method (functor interface)
 *  @param  position CaloPosition object
 *  @return new created CaloMomentum object
 */
// =============================================================================
CaloMomentum* CaloPhotonMomentum::operator() 
  ( const CaloPosition& position ) const 
{
  // energy 
  const double e = position.e() ;
  // the point
  HepPoint3D  pos( position.x () , position.y () , position.z () );
  // the vector
  HepVector3D mom( pos - m_null );
  //
  const double r2 = mom.mag2() ;
  if( r2 < 1.e-40 )
    { throw CaloException("CaloPhotonMomentum: vector length is too small!");}
  const double r  = sqrt( r2 ) ;
  const double r3 = r * r2     ;
  const double ir = 1. / r     ;
  const double x  = mom.x ()   ;
  const double y  = mom.y ()   ;
  const double z  = mom.z ()   ;
  //
  mom *= ( e / r ) ;
  ///
  CaloMomentum* momentum = new CaloMomentum();
  momentum->setMomentum( HepLorentzVector( mom , e ) );
  /// 
  CaloMomentum::Covariance&       covMom = momentum->covariance () ;
  const CaloPosition::Covariance& covPos = position.covariance  () ;
  typedef CaloMomentum::Momentum  CM;
  typedef CaloPosition            CP;
  ///
  ///
  const double dpxdx = e * (  ir - x * x / r3 ) ;
  const double dpxdy = e * (     - x * y / r3 ) ;
  const double dpxde =             x     / r    ;
  ///
  const double dpydx = e * (     - y * x / r3 ) ;
  const double dpydy = e * (  ir - y * y / r3 ) ;
  const double dpyde =             y     / r    ;
  ///
  const double dpzdx = e * (     - z * x / r3 ) ;
  const double dpzdy = e * (     - z * y / r3 ) ;
  const double dpzde =             z     / r    ;
  ///
  
  covMom( CM::X + 1 , CM::X  + 1 ) 
    =       dpxdx * dpxdx * covPos( CP::X , CP::X ) 
    + 2.0 * dpxdy * dpxdx * covPos( CP::Y , CP::X ) 
    + 2.0 * dpxde * dpxdx * covPos( CP::E , CP::X ) 
    +       dpxdy * dpxdy * covPos( CP::Y , CP::Y ) 
    + 2.0 * dpxde * dpxdy * covPos( CP::E , CP::Y ) 
    +       dpxde * dpxde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::X + 1 , CM::Y  + 1 ) 
    =       dpxdx * dpydx * covPos( CP::X , CP::X ) 
    +       dpxdy * dpydx * covPos( CP::Y , CP::X ) 
    +       dpxdx * dpydy * covPos( CP::Y , CP::X ) 
    +       dpxde * dpydx * covPos( CP::E , CP::X ) 
    +       dpxdx * dpyde * covPos( CP::E , CP::X ) 
    +       dpxdy * dpydy * covPos( CP::Y , CP::Y ) 
    +       dpxde * dpydy * covPos( CP::E , CP::Y ) 
    +       dpxdy * dpyde * covPos( CP::E , CP::Y ) 
    +       dpxde * dpyde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::X + 1 , CM::Z  + 1 ) 
    =       dpxdx * dpzdx * covPos( CP::X , CP::X ) 
    +       dpxdy * dpzdx * covPos( CP::Y , CP::X ) 
    +       dpxdx * dpzdy * covPos( CP::Y , CP::X ) 
    +       dpxde * dpzdx * covPos( CP::E , CP::X ) 
    +       dpxdx * dpzde * covPos( CP::E , CP::X ) 
    +       dpxdy * dpzdy * covPos( CP::Y , CP::Y ) 
    +       dpxde * dpzdy * covPos( CP::E , CP::Y ) 
    +       dpxdy * dpzde * covPos( CP::E , CP::Y ) 
    +       dpxde * dpzde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::X + 1 , CM::T  + 1 ) 
    =       dpxdx *         covPos( CP::E , CP::X ) 
    +       dpxdy *         covPos( CP::E , CP::Y ) 
    +       dpxde *         covPos( CP::E , CP::E ) ;
  
  covMom( CM::Y + 1 , CM::Y  + 1 ) 
    =       dpydx * dpydx * covPos( CP::X , CP::X ) 
    + 2.0 * dpydy * dpydx * covPos( CP::Y , CP::X ) 
    + 2.0 * dpyde * dpydx * covPos( CP::E , CP::X ) 
    +       dpydy * dpydy * covPos( CP::Y , CP::Y ) 
    + 2.0 * dpyde * dpydy * covPos( CP::E , CP::Y ) 
    +       dpyde * dpyde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::Y + 1 , CM::Z  + 1 ) 
    =       dpydx * dpzdx * covPos( CP::X , CP::X ) 
    +       dpydy * dpzdx * covPos( CP::Y , CP::X ) 
    +       dpydx * dpzdy * covPos( CP::Y , CP::X ) 
    +       dpyde * dpzdx * covPos( CP::E , CP::X ) 
    +       dpydx * dpzde * covPos( CP::E , CP::X ) 
    +       dpydy * dpzdy * covPos( CP::Y , CP::Y ) 
    +       dpyde * dpzdy * covPos( CP::E , CP::Y ) 
    +       dpydy * dpzde * covPos( CP::E , CP::Y ) 
    +       dpyde * dpzde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::Y + 1 , CM::T  + 1 ) 
    =       dpydx *         covPos( CP::E , CP::X ) 
    +       dpydy *         covPos( CP::E , CP::Y ) 
    +       dpyde *         covPos( CP::E , CP::E ) ;
  
  covMom( CM::Z + 1 , CM::Z  + 1 ) 
    =       dpzdx * dpzdx * covPos( CP::X , CP::X ) 
    + 2.0 * dpzdy * dpzdx * covPos( CP::Y , CP::X ) 
    + 2.0 * dpzde * dpzdx * covPos( CP::E , CP::X ) 
    +       dpzdy * dpzdy * covPos( CP::Y , CP::Y ) 
    + 2.0 * dpzde * dpzdy * covPos( CP::E , CP::Y ) 
    +       dpzde * dpzde * covPos( CP::E , CP::E ) ;
  
  covMom( CM::Z + 1 , CM::T  + 1 ) 
    =       dpzdx *         covPos( CP::E , CP::X ) 
    +       dpzdy *         covPos( CP::E , CP::Y ) 
    +       dpzde *         covPos( CP::E , CP::E ) ;
  
  covMom( CM::T + 1 , CM::T  + 1 ) 
    =                       covPos( CP::E , CP::E ) ;
  ///
  momentum->setMass  ( 0 ) ;
  momentum->setEMass ( 0 ) ;
  ///

  return momentum ;
};
// =============================================================================

// =============================================================================
// The End 
// =============================================================================
