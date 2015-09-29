// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbmath
// ============================================================================
#include "LHCbMath/Combine.h"
// ============================================================================
/** @file
 *  Implementation file for utilitied fromm LHCbMath/Comiiner.h file  
 *  @see Gaudi::Math::Combine
 *  Helper utility to combine   correlated measurements 
 *  @see P.Avery "Combining measurements with correlated errors", CBX 95 55
 *  @see http://www.phys.ufl.edu/~avery/fitting/error_correl.ps.gz
 *  @see http://www.researchgate.net.publication/2345482_Combining_Measurements_with_Correlated_Errors
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 *  Version           $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
/*  combine two measurements <code>x</code> and <code>y</code>
 *  with covarinace matrix <code>cov</code>
 *  @param x (INPUT) the first  measurement 
 *  @param y (INPUT) the second measurement 
 *  @param cov2 (INPUT) covariance matrix 
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const double               x   , 
  const double               y   , 
  const Gaudi::SymMatrix2x2& cov ) 
{
  Gaudi::Vector2 data ( x,y) ;
  Gaudi::Math::Combine<2> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// ========================================================================
/*  combine two measurements <code>x</code> and <code>y</code>
 *  with covarinace matrix <code>cov</code>
 *  @param x (INPUT) the first  measurement 
 *  @param y (INPUT) the second measurement 
 *  @param z (INPUT) the second measurement 
 *  @param cov2 (INPUT) covariance matrix 
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const double               x   , 
  const double               y   , 
  const double               z   , 
  const Gaudi::SymMatrix3x3& cov ) 
{
  Gaudi::Vector3 data ( x , y , z ) ;
  Gaudi::Math::Combine<3> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// ============================================================================
/* combine four measurements <code>x</code> and <code>y</code>
 *  with covarinace matrix <code>cov</code>
 *  @param x1 (INPUT) the first  measurement 
 *  @param x2 (INPUT) the second measurement 
 *  @param x3 (INPUT) the second measurement 
 *  @param x4 (INPUT) the second measurement 
 *  @param cov2 (INPUT) covariance matrix 
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const double               x1  , 
  const double               x2  , 
  const double               x3  , 
  const double               x4  , 
  const Gaudi::SymMatrix4x4& cov ) 
{
  Gaudi::Vector4 data ( x1 , x2 , x3 , x4 ) ;
  Gaudi::Math::Combine<4> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// ============================================================================
/* combine five measurements <code>x</code> and <code>y</code>
 *  with covarinace matrix <code>cov</code>
 *  @param x1 (INPUT) the first  measurement 
 *  @param x2 (INPUT) the second measurement 
 *  @param x3 (INPUT) the second measurement 
 *  @param x4 (INPUT) the second measurement 
 *  @param x5 (INPUT) the second measurement 
 *  @param cov2 (INPUT) covariance matrix 
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const double               x1  , 
  const double               x2  , 
  const double               x3  , 
  const double               x4  , 
  const double               x5  , 
  const Gaudi::SymMatrix5x5& cov ) 
{
  Gaudi::Vector5 data ( x1 , x2 , x3 , x4 , x5 ) ;
  Gaudi::Math::Combine<5> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// ============================================================================
/* combine two measurements <code>x1</code> and <code>x2</code>
 *  using correlation coefficient <code>rho</code>:  \f$-1\le\rho\le1\f$
 *  @param x1  (INPUT) the first  measurement 
 *  @param x2  (INPUT) the second measurement 
 *  @param rho (INPUT) correlation coefficient 
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const Gaudi::Math::ValueWithError& x1  ,
  const Gaudi::Math::ValueWithError& x2  , 
  const double                       rho ) 
{
  //
  Gaudi::Vector2      data ( x1.value() , x2.value() ) ;
  Gaudi::SymMatrix2x2 cov  ;
  cov ( 0 , 0 ) =     x1.cov2() ;
  cov ( 0 , 1 ) = rho*std::sqrt( x1.cov2() * x2.cov2() ) ;
  cov ( 1 , 1 ) =     x2.cov2() ;
  //
  Gaudi::Math::Combine<2> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// =========================================================================
/*  combine two measurements <code>x1</code> and <code>x2</code>
 *  using theie "statistical" uncertainties (assumed to be uncorrelated) 
 *  and a covariance matrix of "systematic" uncertainnties
 *  @param x1   (INPUT) the first  measurement 
 *  @param x2   (INPUT) the second measurement 
 *  @param syst (INPUT) covariance matrix of systematic uncertainnties  
 *  @return combined result
 *  @author  Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-09-28
 */
// =========================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::combine  
( const Gaudi::Math::ValueWithError& x1   ,
  const Gaudi::Math::ValueWithError& x2   , 
  const Gaudi::SymMatrix2x2&         syst ) 
{
  Gaudi::Vector2      data ( x1.value() , x2.value() ) ;
  Gaudi::SymMatrix2x2 cov  ( syst ) ;
  cov ( 0 , 0 ) += x1.cov2() ;
  cov ( 1 , 1 ) += x2.cov2() ;
  //
  Gaudi::Math::Combine<2> combiner  ( data , cov ) ;
  return combiner.result() ;
}
// =========================================================================

// ============================================================================
// The END 
// ============================================================================
