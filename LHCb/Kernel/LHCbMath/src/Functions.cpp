// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <cmath>
#include <climits>
#include <iostream>
#include <complex>
#include <algorithm>
#include <numeric>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Functions.h"
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Power.h"
// ============================================================================
// GSL
// ============================================================================
#include "gsl/gsl_errno.h"
#include "gsl/gsl_sf_exp.h"
#include "gsl/gsl_sf_log.h"
#include "gsl/gsl_sf_erf.h"
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_psi.h"
#include "gsl/gsl_integration.h"
// ============================================================================
// ROOT
// ============================================================================
#include "TMath.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// Local
// ============================================================================
#include "GSL_sentry.h"
// ============================================================================
/** @file
 *  Implementation file for functions from the file LHCbMath/Functions.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-04-19
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $author$
 */
// ============================================================================
// Rho-functions from Jackson
// ============================================================================
/* the simplest function: constant
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_0
( double /* m  */ ,
  double /* m0 */ ,
  double /* m1 */ ,
  double /* m2 */ ) { return 1 ; }
// ============================================================================
/* the simple function for \f$ 1^- \rightarrow 0^- 0^- \f$, l = 1
 *  \f$\rho(\omega)= \omega^{-1}\f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A2
( double    m     ,
  double /* m0 */ ,
  double /* m1 */ ,
  double /* m2 */ ) { return 1./m ; }
// ============================================================================
/*  the simple function for \f$ 1^- \rightarrow 0^- 1^- \f$, l = 1
 *  \f$\rho(\omega)= \omega \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A3
( double    m     ,
  double /* m0 */ ,
  double /* m1 */ ,
  double /* m2 */ ) { return m ; }
// ============================================================================
/*  the simple function for
 *  \f$ \frac{3}{2}^+ \rightarrow \frac{1}{2}^+ 0^- \f$, l = 1
 *  $\rho(\omega)= \frac{ ( \omega + M )^2 - m^2 }{ \omega^2} \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass
 *  @param m1 the invariant mass of the first  (spinor) particle
 *  @param m2 the invariant mass of the secodn (scalar) particle
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A4
( double    m     ,
  double /* m0 */ ,
  double    m1    ,
  double    m2    )
{
  const double a = m + m1 ;
  //
  return ( a * a  - m2 * m2 ) / ( m * m ) ;
}
// ============================================================================
/*  the simple function for
 *  \f$ \frac{3}{2}^- \rightarrow \frac{1}{2}^+ 0^- \f$, l = 2
 *  $\rho(\omega)= \left[ ( \omega + M )^2 - m^2 \right]^{-1} \f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass
 *  @param m1 the invariant mass of the first  (spinor) particle
 *  @param m2 the invariant mass of the secodn (scalar) particle
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A5
( double    m     ,
  double /* m0 */ ,
  double    m1    ,
  double    m2    )
{
  const double a = m + m1 ;
  //
  return 1 / ( a * a  - m2 * m2 ) ;
}
// ============================================================================
/*  the simple function for \f$\rho^- \rightarrow \pi^+ \pi^-\f$
 *  \f$ 1- \rightarrow 0^- 0^- \f$, l = 1
 *  $\rho(\omega)= \left[ q_0^2 + q^2 \right]^{-1}f$
 *  @see Gaudi::Math::BreitWigner
 *  @see Gaudi::Math::BreitWigner::rho_fun
 *  @param m the invariant mass
 *  @param m the nominam   mass
 *  @return the value of rho-function
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-30
 */
// ============================================================================
double Gaudi::Math::Jackson::jackson_A7
( double    m   ,
  double    m0  ,
  double    m1  ,
  double    m2  )
{
  //
  const double q  = Gaudi::Math::PhaseSpace2::q ( m  , m1 , m2 ) ;
  const double q0 = Gaudi::Math::PhaseSpace2::q ( m0 , m1 , m2 ) ;
  //
  if ( 0 >= q && 0 >= q0 ) { return 1 ; }
  //
  return 1. / ( q * q + q0 * q0 ) ;
}
// ============================================================================
namespace
{
  // ==========================================================================
  typedef Gaudi::Math::GSL::GSL_Error_Handler Sentry ;
  // ==========================================================================
  /// get GSL-workspace
  gsl_integration_workspace* workspace
  ( const Gaudi::Math::WorkSpace& ws )
  {
    void* _ws =  ws.workspace() ;
    return (gsl_integration_workspace*) _ws ;
  }
  // ==========================================================================
  BOOST_STATIC_ASSERT( std::numeric_limits<float> ::is_specialized  ) ;
  BOOST_STATIC_ASSERT( std::numeric_limits<double>::is_specialized  ) ;
  // ==========================================================================
  /** @var s_INFINITY
   *  representation of positive INFINITY
   */
  const double s_INFINITY      = 0.8 * std::numeric_limits<double>::max()  ;
  // ==========================================================================
  /** @var s_INFINITY_LOG
   *  representation of positive INFINITY_LOG 
   */
  const double s_INFINITY_LOG  = std::log ( s_INFINITY ) ;
  // ==========================================================================
  /** @var s_PRECISION
   *  the default precision for various calculations,
   *       in particular GSL integration
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double s_PRECISION     = 1.e-8 ;
  // ==========================================================================
  /** @var s_PRECISION_TAIL
   *  the low-relative precision for tails in GSL integration
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double s_PRECISION_TAIL = 1.e-4 ;
  // ===========================================================================
  /** @var s_SIZE
   *  the workspace size parameter for GSL-integration
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const std::size_t s_SIZE = 200 ;
  // ==========================================================================
  /** @var s_TRUNC
   *  trunkating parameter for CrystalBall-functions
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double      s_TRUNC = 15.0 ;
  // ==========================================================================
  /** @var s_LN10
   *  \f$\ln(10)\f$ 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double      s_LN10 = std::log ( 10 ) ;
  // ==========================================================================
  // Chebyshev & Co
  // ==========================================================================
  Gaudi::Math::Chebyshev_<2> s_c2 ;
  Gaudi::Math::Chebyshev_<3> s_c3 ;
  Gaudi::Math::Chebyshev_<4> s_c4 ;
  Gaudi::Math::Chebyshev_<5> s_c5 ;
  Gaudi::Math::Chebyshev_<6> s_c6 ;
  Gaudi::Math::Chebyshev_<7> s_c7 ;
  Gaudi::Math::Chebyshev_<8> s_c8 ;
  Gaudi::Math::Chebyshev_<9> s_c9 ;
  // ==========================================================================
  inline double _chebyshev ( const unsigned int N , const double x )
  {
    switch ( N )
    {
    case 0  : return        1   ; //
    case 1  : return        x   ; //
    case 2  : return s_c2 ( x ) ; //
    case 3  : return s_c3 ( x ) ; //
    case 4  : return s_c4 ( x ) ; //
    case 5  : return s_c5 ( x ) ; //
    case 6  : return s_c6 ( x ) ; //
    case 7  : return s_c7 ( x ) ; //
    case 8  : return s_c8 ( x ) ; //
    case 9  : return s_c9 ( x ) ; //
      //
    default : break ;
    }
    //
    return 2 * x * _chebyshev ( N - 1 , x ) - _chebyshev ( N - 2 , x ) ;
  }
  // ==========================================================================
  // Legendre & Co
  // ==========================================================================
  Gaudi::Math::Legendre_<2>  s_l2 ;
  Gaudi::Math::Legendre_<3>  s_l3 ;
  Gaudi::Math::Legendre_<4>  s_l4 ;
  Gaudi::Math::Legendre_<5>  s_l5 ;
  Gaudi::Math::Legendre_<6>  s_l6 ;
  Gaudi::Math::Legendre_<7>  s_l7 ;
  Gaudi::Math::Legendre_<8>  s_l8 ;
  Gaudi::Math::Legendre_<9>  s_l9 ;
  // ==========================================================================
  inline double _legendre  ( const unsigned int N , const double x )
  {
    //
    switch ( N )
    {
    case 0  : return        1   ; //
    case 1  : return        x   ; //
    case 2  : return s_l2 ( x ) ; //
    case 3  : return s_l3 ( x ) ; //
    case 4  : return s_l4 ( x ) ; //
    case 5  : return s_l5 ( x ) ; //
    case 6  : return s_l6 ( x ) ; //
    case 7  : return s_l7 ( x ) ; //
    case 8  : return s_l8 ( x ) ; //
    case 9  : return s_l9 ( x ) ; //
      //
    default : break ;
    }
    //
    return
      ( ( 2 * N - 1 ) * x * _legendre ( N - 1 , x ) -
        (     N - 1 ) *     _legendre ( N - 2 , x ) ) / N ;
  }
  // ==========================================================================
  // Hermite & Co
  // ==========================================================================
  Gaudi::Math::Hermite_<2>  s_h2 ;
  Gaudi::Math::Hermite_<3>  s_h3 ;
  Gaudi::Math::Hermite_<4>  s_h4 ;
  Gaudi::Math::Hermite_<5>  s_h5 ;
  Gaudi::Math::Hermite_<6>  s_h6 ;
  Gaudi::Math::Hermite_<7>  s_h7 ;
  Gaudi::Math::Hermite_<8>  s_h8 ;
  Gaudi::Math::Hermite_<9>  s_h9 ;
  // ==========================================================================
  inline double _hermite  ( const unsigned int N , const double x )
  {
    //
    switch ( N )
    {
    case 0  : return        1   ; //
    case 1  : return        x   ; //
    case 2  : return s_h2 ( x ) ; //
    case 3  : return s_h3 ( x ) ; //
    case 4  : return s_h4 ( x ) ; //
    case 5  : return s_h5 ( x ) ; //
    case 6  : return s_h6 ( x ) ; //
    case 7  : return s_h7 ( x ) ; //
    case 8  : return s_h8 ( x ) ; //
    case 9  : return s_h9 ( x ) ; //
      //
    default : break ;
    }
    //
    return
      x * _hermite ( N - 1 , x ) - ( N - 1 ) * _hermite ( N - 2 , x ) ;
  }
  // ==========================================================================
  // Constants
  // ==========================================================================
  /** @var s_SQRTPIHALF
   *  helper constant \f$ \sqrt{\frac{\pi}{2}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_SQRTPIHALF = std::sqrt( M_PI_2 ) ;
  // ==========================================================================
  /** @var s_SQRT2PI
   *  helper constant \f$ \sqrt{2\pi}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_SQRT2PI    =       std::sqrt ( 2 * M_PI ) ;
  // ===========================================================================
  /** @var s_SQRT2PIi
   *  helper constant \f$ \frac{1}{\sqrt{2\pi}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_SQRT2PIi    =      1./s_SQRT2PI ;
  // ===========================================================================
  /** @var s_SQRT2 
   *  helper constant \f$\sqrt{2}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2013-08-25
   */
  const double s_SQRT2      =       std::sqrt ( 2.0 )      ;
  // ===========================================================================
  /** @var s_SQRT2i 
   *  helper constant \f$\frac{1}{\sqrt{2}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2013-08-25
   */
  const double s_SQRT2i      =       1/std::sqrt ( 2.0 )    ;
  // ===========================================================================
  /** @var s_HALFSQRTPI
   *  helper constant \f$ \frac{\sqrt{\pi}}{2}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_HALFSQRTPI = 0.5 * std::sqrt(     M_PI ) ;
  // ==========================================================================
  // Bukin & Co
  // ==========================================================================
  /** @var s_Bukin
   *  useful constant for Bukin's function
   *  \f$ \sqrt{ 2 \log { 2 } } \f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_Bukin   = std::sqrt ( 2.0 * std::log ( 2.0 ) ) ;
  // ==========================================================================
  /** @var s_ln2
   *  useful constant for Bukin's function
   *  \f$ \log { 2 } \f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_ln2 = std::log ( 2.0 ) ;
  // ==========================================================================
  /// equality criteria for doubles
  LHCb::Math::Equal_To<double> s_equal ;       // equality criteria for doubles
  // ==========================================================================
  /** helper function for itegration of Bukin's function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  double bukin_GSL ( double x , void* params )
  {
    const Gaudi::Math::Bukin* bukin = (Gaudi::Math::Bukin*) params ;
    return (*bukin)(x) ;
  }
  // ==========================================================================
  // Novosibirsk & Co
  // ==========================================================================
  /** @var s_Novosibirsk
   *  useful constant for evaliuation of ``Novosibirsk'' function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  const double s_Novosibirsk = std::sqrt ( std::log ( 4.0 ) ) ;
  // ==========================================================================
  /** helper function for itegration of Novosibirsk's function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double novosibirsk_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Novosibirsk* novosibirsk =
      (Gaudi::Math::Novosibirsk*) params ;
    //
    return (*novosibirsk)(x) ;
  }
  // ==========================================================================
  /** evaluate the helper function  \f[ f = \frac{\log{1+x}}{x} \f]
   *  it allows to calculate Bukin' function in efficient and regular way
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  inline double x_log
  ( const double x , double precision = s_PRECISION )
  {
    //
    if      ( s_equal   ( x , 0 )       ) { return 1 ; } // RETURN
    else if ( x <= -1                   ) { return 0 ; } // RETURN ?
    else if ( s_equal   ( x , -1 )      ) { return 0 ; } // RETURN ?
    else if ( std::fabs ( x     ) < 0.1 )
    {
      double result = 1.0 ;
      double delta  = x   ;
      //
      precision = std::fabs ( precision ) ;
      precision = std::min  ( precision , std::fabs ( s_PRECISION_TAIL ) ) ;
      if ( 0 > x ) { precision *= 0.1 ; }
      //
      unsigned int n = 2 ;
      do
      {
        result += delta / n ;
        delta  *=       - x ; // note minus here
        ++n                 ;
      }
      while ( std::fabs ( delta ) > precision && n < 1000 ) ;
      //
      return result ;                                         // RETURN
    }
    //
    // the generic evaluation
    //
    return std::log ( 1.0 + x ) / x ;
  }
  // ==========================================================================
  /** evaluate the helper function \f[ f = \frac{\sinh {x}{x} \f]
   *  it allows to calculate Novosibirsk's function in efficient and regular way
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  inline double x_sinh
  ( const double x , double precision = s_PRECISION )
  {
    //
    if      ( s_equal   ( x , 0 )    ) { return 1 ; }
    else if ( std::fabs ( x ) < 0.1  )
    {
      double result = 1.0  ;
      double delta  = x    ;
      //
      precision = std::fabs ( precision ) ;
      precision = std::min  ( precision , std::fabs ( s_PRECISION_TAIL ) ) ;
      unsigned int n = 1 ;
      //
      do
      {
        delta  *= x * x / ( n + 1 )  / ( n + 2 )  ;
        result += delta ;
        n      += 2 ;
      }
      while ( std::fabs ( delta ) > 0.1 * precision && n < 1000 ) ;
      //
      return result ;
    }
    //
    if ( 100 < std::fabs ( x )  ) { return  s_INFINITY ; }
    //
    // the generic evaluation
    //
    return std::sinh ( x ) / x ;
  }
  // ==========================================================================
  /** the simple wrapper for the standard error function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double error_func ( const double x )
  {
    //
    if      ( -30 > x ) { return -1 ; }
    else if ( +30 < x ) { return  1 ; }
    //
    Sentry sentry ;
    //
    gsl_sf_result result ;
    const int ierror = gsl_sf_erf_e ( x , &result ) ;
    if ( ierror )
    {
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    )    /* failed because of roundoff error    */
      {}
      else
      {
        gsl_error ( "Error from erf_e function" ,
                    __FILE__ , __LINE__ , ierror ) ;
      }
      //
      if      ( -15 > x ) { return -1 ; }
      else if (  15 < x ) { return  1 ; }
    }
    //
    return result.val ;                    // RETURN
  }
  // ==========================================================================
  /** the protected exponent
   *  @author Vanya BELYAEV
   */
  double my_exp ( const double arg )
  {
    //
    Sentry sentry ;
    gsl_sf_result      result ;
    const int          ierror = gsl_sf_exp_e ( arg , &result ) ;
    //
    if ( ierror )
    {
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    ) {} /* failed because of roundoff error    */
      else
      {
        gsl_error ( "Error from exp_e function" ,
                    __FILE__ , __LINE__ , ierror ) ;
      }
      //
      if      ( -100 > arg ) { return          0 ; }
      else if (  100 < arg ) { return s_INFINITY ; }
    }
    //
    return result.val ;
  }
  // ==========================================================================
  /** the protected logarithm
   *  @author Vanya BELYAEV
   */
  double my_log ( const double arg )
  {
    //
    if      ( 0          >= arg ) { return -s_INFINITY_LOG ; } // REUTRN
    else if ( s_INFINITY <= arg ) { return  s_INFINITY_LOG ; } // RETURN 
    //
    Sentry sentry ;
    gsl_sf_result result ;
    const int     ierror = gsl_sf_log_e ( arg , &result ) ;
    if ( ierror )
    {
      if ( ierror == GSL_EDOM     ||    /* input domain error, e.g sqrt(-1)    */
           ierror == GSL_ERANGE   ||    /* output range error, e.g. exp(1e100) */
           ierror == GSL_EINVAL   ||    /* invalid argument supplied by user   */
           ierror == GSL_EUNDRFLW ||    /* underflow                           */
           ierror == GSL_EOVRFLW  ||    /* overflow                            */
           ierror == GSL_ELOSS    ||    /* loss of accuracy                    */
           ierror == GSL_EROUND    )    /* failed because of roundoff error    */
      {}
      else
      {
        gsl_error ( "Error from exp_e function" , __FILE__ , __LINE__ , ierror ) ;
      }
      //
      if      (  1.e-100 > arg  ) { return -s_INFINITY_LOG ; }
      else if (  1.e+100 < arg  ) { return  s_INFINITY_LOG ; }
      //
    }
    //
    return result.val ;
  }
  // ==========================================================================
  /** helper function for itegration
   *  \f[ f =  \exp { \kappa x^2 + \xi x }  \f]
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-04-19
   */
  double gauss_GSL ( double x , void* params )
  {
    const double* gauss = (double*) params ;
    //
    const double kappa = gauss[0] ;
    const double xi    = gauss[1] ;
    //
    const double arg   = kappa * x * x + xi * x ;
    //
    return my_exp ( arg ) ;
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_a^b \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter
   *  @param beta  the beta  parameter
   *  @param a     the low  integration limit
   *  @param b     the high integration limit
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double gaussian_int
  ( const double alpha ,
    const double beta  ,
    const double a     ,
    const double b     )
  {
    //
    if      ( s_equal ( a , b ) ) { return  0.0                        ; }
    else if (           a > b   ) { return -1 * gaussian_int ( alpha ,
                                                               beta  ,
                                                               b     ,
                                                               a     ) ; }
    //
    // alpha ?
    //
    if       ( s_equal ( alpha , 0 ) )      // the most trivial trivial case
    {
      /// the most trivial trivial case
      if     ( s_equal ( beta , 0 ) ) { return b - a ; }  // RETURN
      /// get the simple exponential integral
      return ( my_exp  ( beta * b ) - my_exp ( beta * a ) ) / beta ;
    }
    // it just the standard  error function
    else if ( alpha > 0 )        // it just the standard  error function
    {
      //
      const double alpha_sqrt = std::sqrt ( alpha ) ;
      //
      const double factor1 = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
      const double factor  = factor1 * s_HALFSQRTPI ;
      //
      const double root    = 0.5 * beta / alpha ;
      const double a_prime = alpha_sqrt * ( a - root ) ;
      const double b_prime = alpha_sqrt * ( b - root ) ;
      //
      return factor * ( error_func ( b_prime ) -
                        error_func ( a_prime ) ) ;
    }
    //
    // use GSL to evaluate the integral
    Sentry sentry ;
    //
    gsl_function F            ;
    F.function = &gauss_GSL   ;
    double params[2]          ;
    params[0]  = -alpha       ;    // ATTENTION: minus sign here!
    params[1]  =  beta        ;
    F.params   = params       ;
    //
    //
    double result   = 1.0 ;
    double error    = 1.0 ;
    //
    Gaudi::Math::WorkSpace ws ;
    const int ierror = gsl_integration_qag
      ( &F                ,            // the function
        a     ,   b       ,            // low & high edges
        s_PRECISION       ,            // absolute precision
        s_PRECISION       ,            // relative precision
        s_SIZE            ,            // size of workspace
        GSL_INTEG_GAUSS31 ,            // integration rule
        workspace ( ws )  ,            // workspace
        &result           ,            // the result
        &error            ) ;          // the error in result
    //
    if ( ierror )
    {
      gsl_error ( "Gaudi::Math::gaussian_int " ,
                  __FILE__ , __LINE__ , ierror ) ;
    }
    //
    return result ;
    //
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_a^{\inf} \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter
   *  @param beta  the beta  parameter
   *  @param a     the low integration limit
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double gaussian_int_R ( const double alpha ,
                          const double beta  ,
                          const double a     )
  {
    //
    if ( s_equal ( alpha , 0 ) )
    {
      //
      if ( beta < 0 )
      {
        // get the exponential integral
        return - my_exp ( beta * a ) / beta ;       // RETURN
      }
      else                     { return s_INFINITY ; }  // RETURN
    }
    else if ( alpha < 0 )      { return s_INFINITY ; }  // RETURN
    //
    //
    const double alpha_sqrt = std::sqrt ( alpha ) ;
    //
    const double factor1 = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
    const double factor  = factor1 * s_HALFSQRTPI ;
    //
    const double root    = 0.5 * beta / alpha ;
    const double a_prime = alpha_sqrt * ( a - root ) ;
    //
    return factor * ( 1  - error_func ( a_prime ) ) ;
  }
  // ==========================================================================
  /** get the gaussian integral:
   *  \f[ f = \int_{-\inf}^{b} \exp { -\alpha x^2 + \beta x } \mathrm{d}x \f]
   *  @param alpha the alpha parameter
   *  @param beta  the beta  parameter
   *  @param b     the high integration limit
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double gaussian_int_L ( const double alpha ,
                          const double beta  ,
                          const double b     )
  {
    //
    if ( s_equal ( alpha , 0 ) )
    {
      //
      if ( beta > 0 )
      {
        // get the exponential integral
        return std::exp ( beta * b ) / beta ;           // RETURN
      }
      else                     { return s_INFINITY ; }  // RETURN
    }
    else if ( alpha < 0 )      { return s_INFINITY ; }  // RETURN
    //
    //
    const double alpha_sqrt = std::sqrt ( alpha ) ;
    //
    const double factor1    = my_exp ( 0.25 * beta * beta / alpha ) / alpha_sqrt ;
    const double factor     = factor1 * s_HALFSQRTPI ;
    //
    const double root       = 0.5 * beta / alpha ;
    const double b_prime    = alpha_sqrt * ( b - root ) ;
    //
    return factor * ( error_func ( b_prime ) + 1  ) ;
  }
  // ==========================================================================
  /** evaluate very simple power-law intergal 
   *  
   *  \f[ I = \int_{x_{low}}^{x_{high}} \left( \frac{A}{B+Cx}\right)^{N} dx \f]
   * 
   *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
   */
  double tail_integral 
  ( const double A    , 
    const double B    , 
    const double C    , 
    const double N    , 
    const double low  , 
    const double high )
  {
    //
    // few really very simple cases:
    if      ( s_equal ( N , 0 ) ) { return high - low ; }
    else if ( s_equal ( A , 0 ) ) { return 0 ; }
    else if ( s_equal ( C , 0 ) ) { return std::pow ( A / B , N ) * ( high - low ) ; }
    //
    // again the trivial cases 
    if ( s_equal ( low , high ) ) { return 0 ; }
    else if      ( low > high   ) { return -1 * tail_integral ( A , B , C , N , high , low ) ; }
    //
    //  y = (B+C*x)/A
    //
    const double y_low  = ( B + C * low  ) / A ;
    const double y_high = ( B + C * high ) / A ;
    //
    // the special case 
    if ( s_equal ( N , 1 ) ) { return A / C * my_log ( y_high / y_low ) ; } // RETURN 
    //
    // the regular case 
    return A / C * ( std::pow ( y_high , 1 - N ) - 
                     std::pow ( y_low  , 1 - N ) ) / ( 1 - N ) ;
  }
  // ==========================================================================
  /** @var x_sqrt2
   *  \f$\sqrt{2}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double s_sqrt2  = std::sqrt ( 2.0 ) ;
  // ==========================================================================
  /** @var x_sqrt2_i
   *  \f$\frac{1}{\sqrt{2}}\f$
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  const double s_sqrt2_i = 1 / s_sqrt2 ;
  // ==========================================================================
  /** helper function for itegration of Gram-Charlier A function
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double gram_charlier_A_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::GramCharlierA* gca =
      (Gaudi::Math::GramCharlierA*) params ;
    //
    return (*gca)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Breit-Wigner shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double breit_wigner_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::BreitWigner* bw =
      (Gaudi::Math::BreitWigner*) params ;
    //
    return (*bw)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Flatte shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double flatte_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Flatte* f = (Gaudi::Math::Flatte*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_3_1_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::PhaseSpace3* ps =
      (Gaudi::Math::PhaseSpace3*) params ;
    //
    return ps -> ps2_aux (x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_3_2_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::PhaseSpace3* ps =
      (Gaudi::Math::PhaseSpace3*) params ;
    //
    return (*ps)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_NL_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::PhaseSpaceNL* ps =
      (Gaudi::Math::PhaseSpaceNL*) params ;
    //
    return (*ps)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of PhaseSpace23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double phase_space_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::PhaseSpace23L* ps23L =
      (Gaudi::Math::PhaseSpace23L*) params ;
    //
    return (*ps23L)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of LASS shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2013-10--5
   */
  double LASS_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::LASS* lass = (Gaudi::Math::LASS*) params ;
    //
    return (*lass)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of LASS23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double LASS_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::LASS23L* lass = (Gaudi::Math::LASS23L*) params ;
    //
    return (*lass)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Bugg23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-23
   */
  double Bugg_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Bugg23L* bugg = (Gaudi::Math::Bugg23L*) params ;
    //
    return (*bugg)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Bugg shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-23
   */
  double Bugg_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Bugg* bugg = (Gaudi::Math::Bugg*) params ;
    //
    return (*bugg)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of BW23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-23
   */
  double BW_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::BW23L* bw = (Gaudi::Math::BW23L*) params ;
    //
    return (*bw)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Flatte23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-24
   */
  double Flatte_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Flatte23L* f = (Gaudi::Math::Flatte23L*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Gounaris23L shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2012-05-24
   */
  double Gounaris_23L_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Gounaris23L* f = (Gaudi::Math::Gounaris23L*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of StudentT-shape
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2013-01-05
   */
  double studentT_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::StudentT* f = (Gaudi::Math::StudentT*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
  /** helper function for itegration of Voigt shape
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23
   */
  double voigt_GSL ( double x , void* params )
  {
    //
    const Gaudi::Math::Voigt* f = (Gaudi::Math::Voigt*) params ;
    //
    return (*f)(x) ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
namespace
{
  // ==========================================================================
  /// get the complex Breit amplitude
  std::complex<double> breit_amp
  ( const double x     ,
    const double m0    ,
    const double gamma )
  {
    //
    static const std::complex<double> s_j ( 0 , 1 ) ;
    //
    const std::complex<double> v =
      m0 * m0 - x * x - s_j * m0 * gamma ;
    //
    // attention: normalization factors and phase space are here!
    //
    // const double d = 2 / M_PI ;
    // const double d = 2 * std::abs ( m0 * gamma  * x ) / M_PI ;
    //
    return  1.0 / v ;
  }
  // ==========================================================================
  typedef double (*rho_fun) ( double , double , double , double ) ;
  //// calculate the current width
  double gamma_run ( const double gam0    ,
                     const double x       ,
                     const double m1      ,
                     const double m2      ,
                     const double m0      ,
                     const unsigned int L ,
                     rho_fun fun = 0      )
  {
    //
    if ( m1 + m2 >= x ) { return 0 ; }   // RETURN
    //
    const double q  = Gaudi::Math::PhaseSpace2::q ( x  , m1 , m2 ) ;
    const double q0 = Gaudi::Math::PhaseSpace2::q ( m0 , m1 , m2 ) ;
    //
    if ( 0 >= q || 0 >= q0 ) { return 0 ; }  // RETURN
    //
    const double r  = 0 != fun ? (*fun) ( x  , m0 , m1 , m2 ) : 1.0 ;
    const double r0 = 0 != fun ? (*fun) ( m0 , m0 , m1 , m2 ) : 1.0 ;
    //
    if ( 0 >= r0 )           { return 0 ; }  // RETURN
    //
    return gam0 * Gaudi::Math::pow ( q / q0 , 2 * L + 1 ) * ( r / r0 ) ;
  }
  // ==========================================================================
} // end of the anonymous namespace
// ============================================================================
// evaluate Chebyshev polynomial
// ============================================================================
double Gaudi::Math::Chebyshev::operator() ( const double x ) const
{ return _chebyshev ( m_N , x ) ; }
// ============================================================================
// evaluate Legendre polynomial
// ============================================================================
double Gaudi::Math::Legendre::operator() ( const double x ) const
{ return _legendre ( m_N , x ) ; }
// ============================================================================
// evaluate Hermite polynomial
// ============================================================================
double Gaudi::Math::Hermite::operator() ( const double x ) const
{ return _hermite  ( m_N , x ) ; }
// ============================================================================
// BifurcatedGauss
// ============================================================================
/*  constructor from all parameters
 *  @param peak    the peak posiion
 *  @param sigma_L (left sigma)
 *  @param sigma_R (right sigma)
 */
// ============================================================================
Gaudi::Math::BifurcatedGauss::BifurcatedGauss
( const double peak   ,
  const double sigmaL ,
  const double sigmaR )
  : std::unary_function<double,double> ()
  , m_peak    ( peak )
  , m_sigmaL  ( std::fabs ( sigmaL ) )
  , m_sigmaR  ( std::fabs ( sigmaR ) )
//
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::BifurcatedGauss::~BifurcatedGauss (){}
// ============================================================================
// evaluate Bifurcated Gaussian
// ============================================================================
double Gaudi::Math::BifurcatedGauss::pdf ( const double x ) const
{
  const double dx = x - m_peak ;
  //
  const double norm = s_SQRTPIHALF * ( sigmaL() + sigmaR() ) ;
  //
  return
    dx < 0 ?
    my_exp ( -0.5 * dx * dx / sigmaL () / sigmaL () ) / norm :
    my_exp ( -0.5 * dx * dx / sigmaR () / sigmaR () ) / norm ;
}
// ============================================================================
// get the integral
// ============================================================================
double Gaudi::Math::BifurcatedGauss::integral () const { return 1 ; }
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::BifurcatedGauss::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  const double norm = s_SQRTPIHALF * ( sigmaL() + sigmaR() ) ;
  //
  // left half-gaussian
  if       ( high <= m_peak )
  {
    return gaussian_int ( 0.5 / sigmaL() / sigmaL() ,
                          0                       ,
                          low  - m_peak           ,
                          high - m_peak           ) / norm ;

  }
  // right half-gaussian
  else if ( low >= m_peak )
  {
    return gaussian_int ( 0.5 / sigmaR() / sigmaR() ,
                          0                       ,
                          low  - m_peak           ,
                          high - m_peak           ) / norm ;

  }
  //
  return
    integral ( low    , m_peak ) +
    integral ( m_peak , high   ) ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setSigmaL ( const double value )
{
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( m_sigmaL , value_ ) ) { return false ; }
  m_sigmaL = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setSigmaR ( const double value )
{
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( m_sigmaR , value_ ) ) { return false ; }
  m_sigmaR = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setPeak( const double value )
{
  if ( s_equal ( m_peak , value ) ) { return false ; }
  m_peak   = value  ;
  //
  return true ;
}
// ============================================================================
double Gaudi::Math::BifurcatedGauss::sigma   () const
{ return 0.5  * ( sigmaL () + sigmaR () )            ; }
// ============================================================================
double Gaudi::Math::BifurcatedGauss::asym    () const
{ return 0.5  * ( sigmaL () - sigmaR () ) / sigma () ; }
 // ============================================================================


// ============================================================================
/*  constructor from all agruments 
 *  @param mu     location/peak posiiton 
 *  @param alpha  "scale" parameter 
 *  @param beta   "shape" parameter 
 */
// ============================================================================
Gaudi::Math::GenGaussV1::GenGaussV1 
( const double mu    ,
  const double alpha , 
  const double beta  ) 
  : std::unary_function<double,double>() 
  , m_mu     ( mu                 ) 
  , m_alpha  ( std::abs ( alpha ) ) 
  , m_beta   ( std::abs ( beta  ) ) 
  , m_gbeta1 ( 0 )
  , m_gbeta2 ( 0 )
{
  //
  setBeta ( beta ) ;
  //
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::GenGaussV1::~GenGaussV1(){}
// ============================================================================
bool Gaudi::Math::GenGaussV1::setMu        ( const double value ) 
{
  if ( s_equal ( value , m_mu) ) { return false ; }
  m_mu = value ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGaussV1::setAlpha     ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_alpha ) ) { return false ; }
  m_alpha = value_ ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGaussV1::setBeta     ( const double value ) 
{
  //
  const double value_ = std::max ( std::abs ( value ) , 1.5/GSL_SF_GAMMA_XMAX ) ;
  //
  if ( s_equal ( value_ , m_beta ) ) { return false ; }
  //
  m_beta = value_ ;
  //
  if   ( beta() * GSL_SF_GAMMA_XMAX < 6 ) 
  { 
    m_gbeta1  = 0 ;
    m_gbeta2  = gsl_sf_lngamma ( 3 / beta() ) ;    
    m_gbeta2 -= gsl_sf_lngamma ( 1 / beta() ) ;
    m_gbeta2  = gsl_sf_exp     ( m_gbeta2   ) ;
  }
  else 
  { 
    m_gbeta1 = 1. / gsl_sf_gamma ( 1 / beta() )            ;
    m_gbeta2 =      gsl_sf_gamma ( 3 / beta() ) * m_gbeta1 ;
  }
  //
  return true ;
}
// ============================================================================
double Gaudi::Math::GenGaussV1::pdf ( const double x ) const 
{
  //
  const double delta  = std::abs ( x - m_mu )         ;
  const double delta1 =            delta  / m_alpha   ;
  const double delta2 = std::pow ( delta1 , m_beta  ) ;
  //
  if ( delta2 > 60 || 0 == m_gbeta1 || beta() * GSL_SF_GAMMA_XMAX < 4 ) 
  {
    double result  = gsl_sf_log ( 0.5 * beta() / alpha() ) ;
    result        -= delta2 ;
    result        -= gsl_sf_lngamma ( 1 / beta() ) ;
    return gsl_sf_exp ( result ) ;
  }
  //
  double result   = 0.5 * beta() / alpha() ;
  result         *= gsl_sf_exp   ( -delta2 ) ;
  result         *= m_gbeta1  ;
  //
  return result ;
}
// ============================================================================
double Gaudi::Math::GenGaussV1::cdf ( const double x ) const 
{
  //
  const double delta  = std::abs ( x - m_mu )         ;
  const double delta1 =            delta  / m_alpha   ;
  const double delta2 = std::pow ( delta1 , m_beta  ) ;
  //
  const double c = gsl_sf_gamma_inc_P ( 1/beta() , -delta2 ) ;
  //
  return x < m_mu ?  0.5 - c : 0.5 + c ;
}
// ============================================================================ 
double Gaudi::Math::GenGaussV1::integral ( const double low  , 
                                           const double high ) const 
{
  if ( s_equal ( low , high ) ) { return 0 ; }
  return cdf ( high ) - cdf ( low ) ;
}
// ============================================================================ 
double  Gaudi::Math::GenGaussV1::variance () const 
{ return alpha() * alpha() *             m_gbeta2   ; }
// ============================================================================
double  Gaudi::Math::GenGaussV1::sigma    () const 
{ return alpha()           * std::sqrt ( m_gbeta2 ) ; }
// ============================================================================
double  Gaudi::Math::GenGaussV1::kurtosis () const 
{
  double result  =   gsl_sf_lngamma ( 5 / beta() ) ;
  result        +=   gsl_sf_lngamma ( 1 / beta() ) ;
  result        -= 2*gsl_sf_lngamma ( 3 / beta() ) ;
  //
  return gsl_sf_exp ( result ) - 3 ;
}
// ============================================================================



// ============================================================================
/* constructor from all agruments 
 *  @param xi     location/peak posiiton 
 *  @param alpha  "scale" parameter 
 *  @param kappa  "shape" parameter 
 */
// ============================================================================
Gaudi::Math::GenGaussV2::GenGaussV2 
( const double xi    ,
  const double alpha , 
  const double kappa )  // kappa=0 correponds to gaussian  
  : std::unary_function<double,double>() 
  , m_xi      ( xi                 ) 
  , m_alpha   ( std::abs ( alpha ) ) 
  , m_kappa   (            kappa   ) 
{
  //
  setKappa ( kappa ) ;
  //
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::GenGaussV2::~GenGaussV2(){}
// ============================================================================
bool Gaudi::Math::GenGaussV2::setXi        ( const double value ) 
{
  if ( s_equal ( value , m_xi) ) { return false ; }
  m_xi = value ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGaussV2::setAlpha     ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_alpha ) ) { return false ; }
  m_alpha = value_ ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGaussV2::setKappa  ( const double value ) 
{
  //
  double value_ = value ;
  //
  if ( s_equal ( value_ , 0       ) ) { value_ = 0 ; }
  if ( s_equal ( value_ , m_kappa ) ) { return false ; }
  //
  m_kappa = value_;
  //
  return true ;
}
// ============================================================================
double  Gaudi::Math::GenGaussV2::y ( const double x ) const
{
  if  ( s_equal( m_kappa , 0 ) ) { return ( x - xi() ) / alpha() ; }
  //
  const double delta =  - ( x - xi () ) * kappa() / alpha () ;
  //
  return 
    delta > 1 ?
    -gsl_sf_log        ( 1 + delta ) / kappa() :
    -gsl_sf_log_1plusx (     delta ) / kappa() ;  
}
// ============================================================================
double Gaudi::Math::GenGaussV2::pdf ( const double x ) const 
{
  //
  const double z = xi() + alpha() / kappa() ;
  //
  if      ( kappa () > 0 && ( x > z || s_equal ( x , z ) ) ) { return 0 ; }
  else if ( kappa () < 0 && ( x < z || s_equal ( x , z ) ) ) { return 0 ; }
  //
  const double y_   = y ( x ) ;
  //
  const double gau  = my_exp ( -0.5 * y_ * y_ ) / s_SQRT2PI ;
  //
  return gau / ( 1 - kappa() * ( x - xi () ) ) ;
}
// ============================================================================
double Gaudi::Math::GenGaussV2::cdf ( const double x ) const 
{
  //
  const double z = xi() + alpha() / kappa() ;
  //
  if      ( kappa () > 0 && ( x > z || s_equal ( x , z ) ) ) { return 1 ; }
  else if ( kappa () < 0 && ( x < z || s_equal ( x , z ) ) ) { return 0 ; }
  //
  const double y_   = y ( x ) ;
  //
  const double e_   = gsl_sf_erf ( y_ * s_SQRT2i ) ;
  //
  return 0.5 * ( 1 + e_ ) ;
}
// ============================================================================
double Gaudi::Math::GenGaussV2::integral ( const double low  , 
                                           const double high ) const 
{
  if ( s_equal ( low , high ) ) { return 0 ; }
  return cdf ( high ) - cdf ( low ) ;
}
// ============================================================================
double Gaudi::Math::GenGaussV2::mean () const 
{
  if ( s_equal ( kappa () , 0 ) ) { return xi () ; }
  //
  const double k2 = 0.5 * kappa() * kappa() ;
  //
  return xi() - 0.5 * alpha() * kappa() * gsl_sf_exprel ( k2 ) ;
}
// ============================================================================
double Gaudi::Math::GenGaussV2::variance () const 
{
  if ( s_equal ( kappa() , 0 ) ) { return alpha () * alpha() ; }
  //
  const double k2 = kappa() * kappa() ;
  //
  return alpha () * alpha() * gsl_sf_exp ( k2 ) * gsl_sf_exprel ( k2 ) ;
}
// ============================================================================
double Gaudi::Math::GenGaussV2::sigma () const 
{ return std::sqrt ( variance() ) ; }
// ============================================================================
double Gaudi::Math::GenGaussV2::skewness () const 
{
  const double k2     = kappa () * kappa() ;
  //
  const double a1     = gsl_sf_exprel (     k2 ) ;
  const double a3     = gsl_sf_exprel ( 3 * k2 ) ;
  //
  const double a      = std::pow ( a1 , 1.5 ) ;
  //
  const double result = 3 *  ( a1 - a3 ) / a ;
  //
  return kappa() * result ;
  //  
}
// ============================================================================
double Gaudi::Math::GenGaussV2::kurtosis () const 
{
  //
  const double ek2 = gsl_sf_exp ( kappa() * kappa() ) ;
  //
  return  Gaudi::Math::pow ( ek2 , 4 )  
    + 2 * Gaudi::Math::pow ( ek2 , 3 ) 
    + 3 * Gaudi::Math::pow ( ek2 , 2 ) - 6 ;
}
// ============================================================================
#include "boost/math/distributions/skew_normal.hpp"
// ============================================================================
/*  constructor from all agruments 
 *  @param xi     location/peak posiiton 
 *  @param omega  "scale" parameter 
 *  @param alpha  "shape" parameter 
 */
// ============================================================================
Gaudi::Math::SkewGauss::SkewGauss
( const double xi    ,
  const double omega , 
  const double alpha )  // alpha=0 correponds to gaussian  
  : std::unary_function<double,double>() 
  , m_xi      ( xi                 ) 
  , m_omega   ( std::abs ( omega ) ) 
  , m_alpha   (            alpha   ) 
{}
// ============================================================================
// desctructor
// ============================================================================
Gaudi::Math::SkewGauss::~SkewGauss(){}
// ============================================================================
bool Gaudi::Math::SkewGauss::setXi        ( const double value ) 
{
  if ( s_equal ( value , m_xi) ) { return false ; }
  m_xi = value ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::SkewGauss::setOmega     ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_omega ) ) { return false ; }
  m_omega = value_ ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::SkewGauss::setAlpha  ( const double value ) 
{
  //
  if ( s_equal ( value , m_alpha ) ) { return false ; }
  m_alpha = value ;
  if ( s_equal ( 0     , m_alpha ) ) { m_alpha = 0  ; }
  //
  return true ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::pdf ( const double x ) const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  return boost::math::pdf ( skew_ , x ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::cdf ( const double x ) const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  return boost::math::cdf ( skew_ , x ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::integral ( const double low  , 
                                          const double high ) const 
{
  if ( s_equal ( low , high ) ) { return 0 ; }
  //
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return 
    boost::math::cdf ( skew_ , high ) - 
    boost::math::cdf ( skew_ , low  ) ;  
}
// ============================================================================
double Gaudi::Math::SkewGauss::mean () const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return boost::math::mean ( skew_ ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::variance () const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return boost::math::variance ( skew_ ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::skewness () const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return boost::math::skewness ( skew_ ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::kurtosis () const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return boost::math::kurtosis ( skew_ ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::mode  () const 
{
  typedef boost::math::skew_normal_distribution<>  skew ;
  skew skew_ ( m_xi , m_omega , m_alpha ) ;
  //
  return boost::math::kurtosis ( skew_ ) ;
}
// ============================================================================
double Gaudi::Math::SkewGauss::sigma  () const 
{ return std::sqrt ( variance () ) ; }
// ============================================================================





// ============================================================================
// WorskSpace
// ============================================================================
// constructor
// ============================================================================
Gaudi::Math::WorkSpace::WorkSpace () : m_workspace ( 0 ){}
// ============================================================================
// (fictive) copy constructor
// ============================================================================
Gaudi::Math::WorkSpace::WorkSpace
( const Gaudi::Math::WorkSpace& /* right */ )
  : m_workspace ( 0 )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::WorkSpace::~WorkSpace ()
{
  if ( 0 != m_workspace )
  {
    gsl_integration_workspace * _ws = (gsl_integration_workspace*) m_workspace ;
    m_workspace = 0 ;
    gsl_integration_workspace_free ( _ws );
  }
}
// ============================================================================
// get the integration workspace
// ============================================================================
void* Gaudi::Math::WorkSpace::workspace () const
{
  if ( 0 == m_workspace ) 
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  return m_workspace ;
}
// ============================================================================
// fictive assignement operator
// ============================================================================
Gaudi::Math::WorkSpace&
Gaudi::Math::WorkSpace::operator=
  ( const Gaudi::Math::WorkSpace& /* right */ ) { return *this ; }
// ============================================================================


// ============================================================================
// Bukin
// ============================================================================
/*  constructor from all parameters
 *  @param peak  the peak position
 *  @param sigma the effective sigma, defined as FWHM/2.35
 *  @param xi    the asymmetry parameter
 *  @param rhoL  the left  tail paramter
 *  @param rhoR  the right tail paramter
 */
// ============================================================================
Gaudi::Math::Bukin::Bukin
( const double peak   ,
  const double sigma  ,
  const double xi     ,
  const double rho_L  ,
  const double rho_R  )
//
  : std::unary_function<double,double> ()
//
  , m_peak      ( M_PI + peak  )
  , m_sigma     ( M_PI + sigma )
  , m_xi        ( M_PI + xi    )
  , m_rho_L     ( M_PI + rho_L )
  , m_rho_R     ( M_PI + rho_R )
//
  , m_x1        ( M_PI )
  , m_x2        ( M_PI )
//
  , m_integral  ( -1000 )
//
  , m_workspace ()
{
  //
  setXi    ( xi    ) ; // must be the first
  //
  setPeak  ( peak  ) ;
  //
  setSigma ( sigma ) ;
  //
  setRhoL  ( rho_L ) ;
  //
  setRhoR  ( rho_R ) ;
  //
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Bukin::~Bukin () {}
// ============================================================================
bool Gaudi::Math::Bukin::setPeak  ( const double value )
{
  if ( s_equal ( value , m_peak ) ) { return false ; }
  //
  m_peak   = value ;
  //
  const double xi_ = m_xi / std::sqrt ( 1 + m_xi * m_xi ) ;
  m_x1     = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2     = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::Bukin::setSigma ( const double value )
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  //
  m_sigma  = value_ ;
  //
  const double xi_ = m_xi/std::sqrt ( 1 + m_xi * m_xi ) ;
  m_x1 = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2 = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::Bukin::setXi    ( const double value )
{
  // no need for update
  if ( s_equal ( value , m_xi ) ) { return false ; } // no need for update
  //
  m_xi     = value ;
  //
  const double xi      = m_xi    ;
  const double xi2     =   xi*xi ;
  const double xi2sqrt = std::sqrt ( 1 + xi2 ) ;
  //
  const double alpha = 2 * xi  * xi2sqrt / s_Bukin   ;
  const double beta  = 2 * xi  * ( xi - xi2sqrt )    ;
  // well, it is actually alpha/beta:
  const double ab    = xi2sqrt / ( xi - xi2sqrt ) / s_Bukin ;
  //
  m_A     = alpha             ;
  //
  m_B2    = 1/x_log ( beta )  ;
  m_B2   *= m_B2              ;
  m_B2   *= ab*ab             ;
  //
  //
  const double delta = xi + xi2sqrt - 1 ;
  const double tail  =
    0.5 * s_Bukin * xi2sqrt * ( 1 + xi + xi2sqrt )
    / ( xi + xi2sqrt ) / x_log ( delta ) ;
  //
  // left  tail parameter
  //
  m_L  = tail ;
  m_L /= ( xi2sqrt - xi ) ;
  m_L /= ( xi2sqrt - xi ) ;
  //
  // right tail parameter
  //
  m_R  = tail ;
  m_R /= ( xi2sqrt + xi ) ;
  m_R /= ( xi2sqrt + xi ) ;
  //
  // central region
  //
  const double xi_ = m_xi / std::sqrt ( 1 + m_xi * m_xi ) ;
  m_x1 = m_peak + m_sigma * s_Bukin * ( xi_ - 1 ) ;
  m_x2 = m_peak + m_sigma * s_Bukin * ( xi_ + 1 ) ;
  //
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::Bukin::setRhoL  ( const double value )
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_rho_L  ) ) { return false ; }
  //
  m_rho_L    = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::Bukin::setRhoR  ( const double value )
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_rho_R  ) ) { return false ; }
  //
  m_rho_R    = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
// evaluate Bukin's function
// ============================================================================
double Gaudi::Math::Bukin::pdf ( const double x ) const
{
  //
  //  left tail :
  //
  if       ( m_x1 >= x )
  {
    const double dx  = x - m_x1               ;
    const double dx2 = dx / ( m_peak - m_x1 ) ;
    return 0.5 * my_exp (   m_L * dx / m_sigma  - m_rho_L * dx2 * dx2 ) ;
  }
  //
  // right tail :
  //
  else if ( m_x2 <=  x )
  {
    const double dx  = x - m_x2               ;
    const double dx2 = dx / ( m_peak - m_x2 ) ;
    return 0.5 * my_exp ( - m_R * dx / m_sigma  - m_rho_R * dx2 * dx2 ) ;
  }
  //
  // central region
  //
  const double dx   = ( x - m_peak ) / m_sigma ;
  const double A    = x_log ( m_A * dx ) ;
  //
  return my_exp ( - s_ln2 * dx * dx * A * A * m_B2 ) ;
}
// =========================================================================
// get the integral between low and high limits
// =========================================================================
double Gaudi::Math::Bukin::integral
( const double low  ,
  const double high ) const
{
  //
  if      ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  else if (           low > high   ) { return - integral ( high ,
                                                           low  ) ; } // RETURN
  //
  //
  // split into reasonable sub-intervals
  if      ( low < m_x1  && m_x1 < high )
  {
    return
      integral (  low , m_x1 ) +
      integral ( m_x1 , high ) ;
  }
  else if ( low < m_x2 && m_x2 < high )
  {
    return
      integral (  low , m_x2 ) +
      integral ( m_x2 , high ) ;
  }
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma )  , 0.0 ) ;
  if ( 0 < width &&  6 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // left tail:
  if      ( high <= m_x1 )  // left tail
  {
    const double d =  m_peak - m_x1 ;
    //
    return 0.5 * gaussian_int ( m_rho_L / d / d ,
                                m_L  / m_sigma  ,
                                low  - m_x1     ,
                                high - m_x1     ) ;
  }
  // right tail:
  else if ( low >= m_x2 )  // right tail
  {
    const double d = m_peak - m_x2 ;
    //
    return 0.5 * gaussian_int ( m_rho_R / d / d   ,
                                - m_R  / m_sigma  ,
                                low  - m_x2       ,
                                high - m_x2       ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &bukin_GSL ;
  F.params   = const_cast<Bukin*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    //
    gsl_error ( "Gaudi::Math::Bukin::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::Bukin::integral () const
{
  if ( m_integral <= 0 )
  {
    Bukin* bukin = const_cast<Bukin*> ( this ) ;
    bukin -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================
// calculate  the integral
// =========================================================================
void Gaudi::Math::Bukin::integrate()
{
  //
  m_integral = 0.0 ;
  // left tail:
  {
    const double d     = m_peak - m_x1     ;
    const double alpha = m_rho_L / d / d   ;
    const double beta  = m_L     / m_sigma ;
    //
    m_integral +=  0.5 * gaussian_int_L ( alpha , beta , 0 ) ;
  }
  // right tail
  {
    const double d     =    m_peak - m_x2     ;
    const double alpha =    m_rho_R / d / d   ;
    const double beta  =  - m_R     / m_sigma ;
    //
    m_integral += 0.5 * gaussian_int_R ( alpha , beta  , 0 ) ;
  }
  //
  m_integral += integral ( m_x1 , m_x2 ) ;
  //
}
// ============================================================================


// ============================================================================
// Novosibirsk function
// ============================================================================
/*  constructor from all parameters
 *  @param m0    the peak posiion
 *  @param sigma the effective sigma
 *  @param tau   the tail paramter
 */
// ============================================================================
Gaudi::Math::Novosibirsk::Novosibirsk
( const double m0         ,
  const double sigma      ,
  const double tau        )
  : std::unary_function<double,double> ()
//
  , m_m0        ( m0                   )
  , m_sigma     ( std::fabs ( sigma )  )
  , m_tau       ( std::tanh ( tau   )  )
//
  , m_lambda    ( 0.0   )
//
  , m_integral  ( -1000 )
  , m_workspace ()
{
  //
  m_lambda = x_sinh ( m_tau * s_Novosibirsk ) ;
  //
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Novosibirsk::~Novosibirsk() {}
// ============================================================================
// set parameter m0
// ============================================================================
bool Gaudi::Math::Novosibirsk::setM0    ( const double value )
{
  //
  if ( s_equal ( m_m0 ,  value ) ) { return false ; }
  //
  m_m0 = value ;
  //
  return true ;
}
// ============================================================================
// set parameter sigma
// ============================================================================
bool Gaudi::Math::Novosibirsk::setSigma ( const double value )
{
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  //
  m_sigma    = value_ ;
  m_integral = -1000 ;
  //
  return true ;
}
// ============================================================================
// set parameter tau
// ============================================================================
bool Gaudi::Math::Novosibirsk::setTau ( const double value )
{
  //
  const double value_ = std::tanh ( value )   ;
  if ( s_equal ( value_ , m_tau ) ) { return false ; }
  //
  m_tau      = value_ ;
  m_integral = -1000 ;
  //
  m_lambda   = x_sinh ( m_tau * s_Novosibirsk ) ;
  //
  return true ;
}
// ============================================================================
// evaluate Novosibirsk's function
// ============================================================================
double Gaudi::Math::Novosibirsk::pdf  ( const double x ) const
{
  //
  const double dx     = ( x - m_m0 ) / m_sigma ;
  //
  const double arg    = m_lambda * dx * m_tau ;
  //
  if ( arg <= -1 || s_equal ( arg , -1 ) ) { return 0 ; } // RETURN
  //
  const double l      =  x_log ( arg ) * m_lambda * dx ;
  //
  const double result = l * l  + m_tau * m_tau ;
  //
  return  my_exp ( -0.5 * result ) ;
}
// =========================================================================
// get the integral between low and high limits
// =========================================================================
double Gaudi::Math::Novosibirsk::integral
( const double low  ,
  const double high ) const
{
  //
  if      ( s_equal ( low , high ) ) { return                  0.0 ; } // RETURN
  else if (           low > high   ) { return - integral ( high ,
                                                           low   ) ; } // RETURN
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_sigma  ;
  const double x2     = m_m0 + 10 * m_sigma  ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  if      ( low < x_low  && x_low < high )
  {
    return
      integral (   low , x_low  ) +
      integral ( x_low ,   high ) ;
  }
  else if ( low <  x_high && x_high < high )
  {
    return
      integral (   low  , x_high  ) +
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma )  , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL :
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::Novosibirsk::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::Novosibirsk::integral () const
{
  if ( m_integral <= 0 )
  {
    Novosibirsk* novosibirsk = const_cast<Novosibirsk*> ( this ) ;
    novosibirsk -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================
// calculate  the integral
// =========================================================================
void Gaudi::Math::Novosibirsk::integrate()
{
  //
  const double x1     = m_m0 - 10 * m_sigma ;
  const double x2     = m_m0 + 10 * m_sigma ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  // use GSL to evaluate the tails:
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  // left tail:
  //
  double tail_l   =  0.0 ;
  double error_l  = -1.0 ;
  //
  const int ierror_l = gsl_integration_qagil
    ( &F                ,         // the function
      x_low             ,         // "high" edge
      s_PRECISION       ,         // absolute precision
      s_PRECISION_TAIL  ,         // relative precision
      s_SIZE            ,         // size of workspace
      workspace ( m_workspace ) , // workspace
      &tail_l           ,         // the result
      &error_l          ) ;        // the error in result
  //
  if ( ierror_l )
  {
    gsl_error ( "Gaudi::Math::Novosibirsk::QAGIL" ,
                __FILE__ , __LINE__ , ierror_l ) ;
    tail_l = 0.0 ;
  }
  //
  //
  // right tail:
  //
  double tail_r   =  0.0 ;
  double error_r  = -1.0 ;
  //
  const int ierror_r = gsl_integration_qagiu
    ( &F                ,         // the function
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision
      s_PRECISION_TAIL  ,         // relative precision
      s_SIZE            ,         // size of workspace
      workspace ( m_workspace ) , // workspace
      &tail_r           ,         // the result
      &error_r          ) ;       // the error in result
  //
  if ( ierror_r )
  {
    gsl_error ( "Gaudi::Math::Novosibirsk::QAGIU" ,
                __FILE__ , __LINE__ , ierror_r ) ;
    tail_r = 0.0 ;
  }
  //
  // get the final result
  //
  m_integral = tail_l + integral ( x_low , x_high ) + tail_r ;
  //
}
// ============================================================================
// Crystal Ball & Co
// ============================================================================
/*  constructor from all parameters
 *  @param m0 m0 parameter
 *  @param alpha alpha parameter
 *  @param n     n-parameter
 */
// ============================================================================
Gaudi::Math::CrystalBall::CrystalBall
( const double m0    ,
  const double sigma ,
  const double alpha ,
  const double n     )
  : std::unary_function<double,double> ()
  , m_m0         ( m0 )
  , m_sigma      (  1 )
  , m_alpha      (  2 )
  , m_n          (  2 )
//
  , m_A  ( -1000 ) 
  , m_B  ( -1000 ) 
  , m_C  ( -1000 ) 
{
  //
  setM0     ( m0    ) ;
  setAlpha  ( alpha ) ;
  setSigma  ( sigma ) ;
  setN      ( n     ) ;
  //
  m_A = my_exp ( -0.5 * m_alpha * m_alpha ) ;
  m_B =  0.5 * ( 1 + gsl_sf_erf ( - m_alpha * s_SQRT2i ) ) ;
  if   ( !s_equal ( m_n , 0 ) && !s_equal ( m_alpha , 0 )  ) 
  { m_C  = ( m_n + 1 )  / std::abs( m_alpha )  / m_n  * s_SQRT2PIi ; }
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::CrystalBall::~CrystalBall (){}
// ============================================================================
bool  Gaudi::Math::CrystalBall::setM0 ( const double value )
{
  //
  if ( s_equal ( value , m_m0 ) ) { return false ; }
  //
  m_m0       = value ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setSigma ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  //
  m_sigma    = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setAlpha  ( const double value )
{
  //
  if ( s_equal ( value , m_alpha ) ) { return false ; }
  //
  m_alpha    = value  ;
  //
  m_A        = my_exp ( -0.5 * m_alpha * m_alpha ) ;
  //
  const double aa  = std::abs ( m_alpha ) ;
  const double np1 = n() + 1 ;
  // 
  if   ( s_equal ( n () , 0 ) || s_equal ( m_alpha , 0 )  ) { m_C = -1000 ; }
  else { m_C  = np1 / aa / n()  * s_SQRT2PIi ; }
  //
  m_B  = 0.5 * ( 1 + gsl_sf_erf ( - m_alpha * s_SQRT2i ) ) ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setN      ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_n     ) ) { return false ; }
  //
  m_n        = value_ ;
  if ( s_equal ( m_n , 0 ) ) { m_n = 0 ; }
  //
  const double aa  = std::abs ( m_alpha ) ;
  if   ( s_equal ( n () , 0 ) || s_equal ( m_alpha , 0 )  ) { m_C = -1000 ; }
  else { m_C  = ( n() + 1 )  / aa / n() * s_SQRT2PIi ; }
  //
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function
// ============================================================================
double Gaudi::Math::CrystalBall::pdf ( const double x ) const
{
  //
  const double dx    = ( x - m_m0 ) / m_sigma ;
  //
  // the tail
  //
  if  ( dx < -m_alpha )
  {
    const double np1  = n() + 1 ;
    const double frac = np1 / ( np1 - std::abs( m_alpha ) * ( m_alpha + dx ) )  ;
    return std::pow ( frac , np1 ) * m_A * s_SQRT2PIi / sigma() ;
  }
  //
  // the peak
  //
  return my_exp ( -0.5 * dx * dx ) * s_SQRT2PIi / sigma() ;
}
// ============================================================================
// get the integral between low and high
// ============================================================================
double Gaudi::Math::CrystalBall::integral
( const double low ,
  const double high ) const
{
  //
  if      ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  else if (           low > high   ) { return - integral ( high ,
                                                           low  ) ; } // RETURN
  //
  const double x0 = m_m0 - m_alpha * m_sigma ;
  //
  // split into proper subintervals
  //
  if      ( low < x0 && x0 < high )
  { return integral ( low , x0 ) + integral ( x0 , high ) ; }
  //
  // Z = (x-x0)/sigma 
  //
  const double zlow  = ( low  - m_m0 ) / sigma() ;
  const double zhigh = ( high - m_m0 ) / sigma() ;
  //
  // peak
  //
  if ( x0 <= low  )
  { return s_SQRT2PIi * gaussian_int ( 0.5   , 0 , zlow  , zhigh ) ; }
  //
  // tail
  //
  const double np1 = n() + 1 ;
  //
  const double A = np1 ;
  const double B = np1 ;
  const double C = - std::abs ( alpha () ) ;
  //
  const double result = s_SQRT2PIi * m_A * 
    tail_integral ( A , B , C , np1 , zlow + alpha() , zhigh + alpha() ) ;
  //
  return result ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBall::integral () const
{
  /// the regular case 
  if ( 0 < m_C ) { return m_C + m_B ; }
  //
  /// trunkate it! 
  const double left = ( 0 < m_alpha ) ?  (-m_alpha-s_TRUNC) : -s_TRUNC ;
  // 
  return m_B + integral ( m0 () + left     * sigma() , 
                          m0 () - alpha () * sigma() ) ;
}
// ============================================================================
// Needham function
// ============================================================================
/* constructor from all parameters
 *  @param m0     m0       parameter
 *  @param sigma  sigma    parameter
 *  @param a0     a0       parameter
 *  @param a1     a1       parameter
 *  @param a2     a2       parameter
 */
// ============================================================================
Gaudi::Math::Needham::Needham
( const double m0    ,
  const double sigma ,
  const double a0    ,
  const double a1    ,
  const double a2    )
  : std::unary_function<double,double>()
/// @see Gaudi::Math:CrystalBall
  , m_cb  ( m0 , sigma , 1 , 0 ) // Gaudi::Math:CrystalBall
  , m_a0  ( std::abs ( a0 )  )
  , m_a1  (            a1    )
  , m_a2  (            a2    )
{
  m_cb.setAlpha ( alpha () ) ;
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Needham::~Needham(){}
// ============================================================================
bool Gaudi::Math::Needham::setA0 ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_a0 ) ) { return false ; }
  m_a0      = value_ ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ============================================================================
bool Gaudi::Math::Needham::setA1 ( const double value )
{
  if ( s_equal ( value , m_a1 ) ) { return false ; }
  m_a1 = value ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ============================================================================
bool Gaudi::Math::Needham::setA2 ( const double value )
{
  if ( s_equal ( value , m_a2 ) ) { return false ; }
  m_a2 = value ;
  return m_cb.setAlpha ( alpha () ) ;
}
// ===========================================================================
// evaluate Needham's function
// ===========================================================================
double Gaudi::Math::Needham::pdf ( const double x ) const
{ return m_cb ( x ) ; }
// ============================================================================


// ============================================================================
/*  constructor from all parameters
 *  @param m0 m0 parameter
 *  @param alpha alpha parameter
 *  @param n     n-parameter
 */
// ============================================================================
Gaudi::Math::CrystalBallRightSide::CrystalBallRightSide
( const double m0    ,
  const double sigma ,
  const double alpha ,
  const double n     )
  : std::unary_function<double,double> ()
  , m_cb         ( m0 , sigma , alpha , n ) 
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::CrystalBallRightSide::~CrystalBallRightSide (){}
// ============================================================================
//  evaluate CrystalBall's function
// ============================================================================
double Gaudi::Math::CrystalBallRightSide::pdf ( const double x ) const
{
  const double y = 2 * m0 ()  - x ;
  //
  return  m_cb.pdf ( y ) ;  
}
// ============================================================================
// get the integral between low and high
// ============================================================================
double Gaudi::Math::CrystalBallRightSide::integral
( const double low ,
  const double high ) const
{ return m_cb.integral ( 2 * m0 () - high  , 2 * m0 () - low ) ; }
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBallRightSide::integral () const 
{ return m_cb.integral () ; }
// =========================================================================

// ============================================================================
/*  constructor from all parameters
 *  @param m0 m0 parameter
 *  @param alpha alpha parameter
 *  @param n     n-parameter
 */
// ============================================================================
Gaudi::Math::CrystalBallDoubleSided::CrystalBallDoubleSided
( const double m0      ,
  const double sigma   ,
  const double alpha_L ,
  const double n_L     ,
  const double alpha_R ,
  const double n_R     )
  : std::unary_function<double,double> ()
  , m_m0         (  m0 )
  , m_sigma      (   1 )
  , m_alpha_L    (   2 )
  , m_n_L        (   2 )
  , m_alpha_R    (   2 )
  , m_n_R        (   2 )
//
  , m_AL         ( -1000 ) 
  , m_AR         ( -1000 )
  , m_B          ( -1000 ) 
  , m_TL         ( -1000 ) 
  , m_TR         ( -1000 ) 
{
  //
  setM0       ( m0      ) ;
  setSigma    ( sigma   ) ;
  setAlpha_L  ( alpha_L ) ;
  setAlpha_R  ( alpha_R ) ;
  setN_L      ( n_L     ) ;
  setN_R      ( n_R     ) ;
  //
  m_AL = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;
  m_AR = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;
  m_B  = 0.5 *  ( gsl_sf_erf (  m_alpha_R * s_SQRT2i ) - 
                  gsl_sf_erf ( -m_alpha_L * s_SQRT2i ) ) ;
  //
  if   ( !s_equal ( m_n_L , 0 ) && !s_equal ( m_alpha_L , 0 )  ) 
  { m_TL  = ( m_n_L + 1 )  / std::abs ( m_alpha_L )  / m_n_L  * s_SQRT2PIi ; }
  if   ( !s_equal ( m_n_R , 0 ) && !s_equal ( m_alpha_R , 0 )  ) 
  { m_TR  = ( m_n_R + 1 )  / std::abs ( m_alpha_R )  / m_n_R  * s_SQRT2PIi ; }
  //
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::CrystalBallDoubleSided::~CrystalBallDoubleSided(){}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setM0 ( const double value )
{
  if ( s_equal ( value , m_m0 ) ) { return false ; }
  //
  m_m0       = value ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setSigma ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_sigma ) ) { return false ; }
  //
  m_sigma    = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_L ( const double value )
{
  if ( s_equal ( value , m_alpha_L ) ) { return false ; }
  //
  m_alpha_L  = value  ;
  m_AL       = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;
  m_B        = 0.5 *  ( gsl_sf_erf (  m_alpha_R * s_SQRT2i ) - 
                        gsl_sf_erf ( -m_alpha_L * s_SQRT2i ) ) ;
  //
  if   ( s_equal ( m_n_L , 0 ) || s_equal  ( m_alpha_L , 0 )  ) {  m_TL = -1000 ; }
  else { m_TL  = ( m_n_L + 1 )  / std::abs ( m_alpha_L )  / m_n_L  * s_SQRT2PIi ; }
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_R ( const double value )
{
  if ( s_equal ( value , m_alpha_R ) ) { return false ; }
  //
  m_alpha_R  = value  ;
  m_AR       = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;
  m_B        = 0.5 *  ( gsl_sf_erf (  m_alpha_R * s_SQRT2i ) - 
                        gsl_sf_erf ( -m_alpha_L * s_SQRT2i ) ) ;
  //
  if   ( s_equal ( m_n_R , 0 ) || s_equal ( m_alpha_R , 0 )  ) { m_TR = -1000 ; }
  else { m_TR  = ( m_n_R + 1 )  / std::abs ( m_alpha_R )  / m_n_R  * s_SQRT2PIi ; }
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_L     ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_n_L    ) ) { return false ; }
  //
  m_n_L      = value_ ;
  if ( s_equal ( m_n_L , 0 ) ) { m_n_L = 0 ; }
  //
  if   ( s_equal ( m_n_L , 0 ) || s_equal ( m_alpha_L , 0 )  ) {  m_TL = -1000 ; }
  else { m_TL  = ( m_n_L + 1 )  / std::abs ( m_alpha_L )  / m_n_L  * s_SQRT2PIi ; }
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_R     ( const double value )
{
  const double value_ = std::fabs ( value );
  if ( s_equal ( value_ , m_n_R    ) ) { return false ; }
  //
  m_n_R      = value_ ;
  if ( s_equal ( m_n_R , 0 ) ) { m_n_R = 1 ; }
  //
  if   ( s_equal ( m_n_R , 0 ) || s_equal ( m_alpha_R , 0 )  ) { m_TR = -1000 ; }
  else { m_TR  = ( m_n_R + 1 )  / std::abs ( m_alpha_R )  / m_n_R  * s_SQRT2PIi ; }
  //
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::pdf ( const double x ) const
{
  //
  const double dx   = ( x - m_m0 ) / m_sigma ;
  //
  // the left tail
  if      ( dx  < -m_alpha_L )  // left tail
  {
    const double np1  = n_L() + 1 ;
    const double frac = np1 / ( np1 - std::abs ( m_alpha_L ) * ( m_alpha_L + dx ) )  ;
    return std::pow ( frac , np1 ) * m_AL * s_SQRT2PIi / sigma() ;
  }
  // the right tail
  else if  ( dx >  m_alpha_R )  // right tail
  {
    const double np1  = n_R () + 1 ;
    const double frac = np1 / ( np1 - std::abs ( m_alpha_R ) * ( m_alpha_R - dx ) )  ;
    return std::pow ( frac , np1 ) * m_AR * s_SQRT2PIi / sigma() ;
  }
  //
  // the peak
  //
  return my_exp ( -0.5 * dx * dx ) * s_SQRT2PIi / sigma() ; 
}
// ============================================================================
// get the integral between low and high
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::integral
( const double low ,
  const double high ) const
{
  //
  if      ( s_equal ( low , high ) ) { return 0.0 ; } // RETURN
  else if (           low > high   ) { return - integral ( high , low ) ; }
  //
  const double x_low  = m_m0 - m_alpha_L * m_sigma ;
  const double x_high = m_m0 + m_alpha_R * m_sigma ;
  //
  // split into proper subintervals
  //
  if ( low < x_low  && x_low  < high )
  { return integral ( low , x_low  ) + integral ( x_low  , high ) ; }
  if ( low < x_high && x_high < high )
  { return integral ( low , x_high ) + integral ( x_high , high ) ; }
  //
  // Z = (x-x0)/sigma 
  //
  const double zlow  = ( low  - m_m0 ) / sigma() ;
  const double zhigh = ( high - m_m0 ) / sigma() ;
  //
  // the peak
  //
  if ( x_low <= low && high <= x_high )
  { return  s_SQRT2PIi * gaussian_int ( 0.5   , 0 , zlow  , zhigh ) ; }
  //
  // left tail 
  //
  if ( high <= x_low ) 
  {
    const double np1 = n_L () + 1 ;
    //
    const double A   = np1 ;
    const double B   = np1 ;
    const double C   = - std::abs ( alpha_L () ) ;
    //
    return s_SQRT2PIi * m_AL * 
      tail_integral ( A , B , C , np1 , zlow + alpha_L() , zhigh + alpha_L() ) ;
  }
  //
  // right tail 
  // 
  if ( low  >= x_high ) 
  {
    //
    const double np1 = n_R () + 1 ;
    //
    const double A   = np1 ;
    const double B   = np1 ;
    const double C   = std::abs ( alpha_R () ) ;
    //
    return s_SQRT2PIi * m_AR * 
      tail_integral ( A , B , C , np1 , zlow - alpha_R() , zhigh - alpha_R() ) ;
  }
  //
  return 0 ;
}
// ============================================================================
// get the (truncated)  integral
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::integral () const 
{
  //
  if      ( 0 < m_TL && 0 <= m_TR ) { return m_TL + m_TR + m_B ; }
  else if ( 0 < m_TR ) 
  {
    /// truncate it! 
    const double left = ( 0 < alpha_L() ) ?  (-alpha_L()-s_TRUNC) : -s_TRUNC ;
    // 
    return m_TR + m_B + integral ( m0 () + left       * sigma() , 
                                   m0 () - alpha_L () * sigma() ) ;
    
  }
  else if ( 0 < m_TL ) 
  {
    /// truncate it! 
    const double right = ( 0 < alpha_R() ) ?  ( alpha_R () + s_TRUNC) : + s_TRUNC ;
    // 
    return m_TL + m_B + integral ( m0 () + alpha_R () * sigma() , 
                                   m0 () + right      * sigma() ) ;
    
  }
  //
  /// truncate both
  const double left  = ( 0 < alpha_L() ) ?  (-alpha_L () - s_TRUNC ) : -s_TRUNC ;
  /// truncate it! 
  const double right = ( 0 < alpha_R() ) ?  ( alpha_R () + s_TRUNC ) : + s_TRUNC ;
  //
  return integral ( m0 () - left  * sigma () , m0 () + right * sigma () ) ;
}
// ============================================================================
// Gram-Charlier type A
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::GramCharlierA::GramCharlierA
( const double mean   ,
  const double sigma  ,
  const double kappa3 ,
  const double kappa4 )
  : std::unary_function<double,double> ()
//
  , m_mean   ( mean )
  , m_sigma  ( std::fabs ( sigma ) )
  , m_kappa3 ( kappa3 )
  , m_kappa4 ( kappa4 )
//
  , m_workspace ()
//
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::GramCharlierA::~GramCharlierA() {}
// ============================================================================
// evaluate Gram-Charlier type A approximation
// ============================================================================
double Gaudi::Math::GramCharlierA::pdf ( const double x ) const
{
  //
  const double dx = ( x - m_mean ) / m_sigma ;
  //
  const double result_0 = my_exp ( -0.5 * dx * dx ) / m_sigma / s_SQRT2PI ;
  //
  double correction = 1 ;
  //
  correction += m_kappa3 * s_h3 ( dx ) /  6 ;
  //
  correction += m_kappa4 * s_h4 ( dx ) / 24 ;
  //
  return correction * result_0 ;
}
// ============================================================================
// integral
// ============================================================================
double Gaudi::Math::GramCharlierA::integral () const { return 1 ; }
// ============================================================================
// integral
// ============================================================================
double Gaudi::Math::GramCharlierA::integral
( const double low  ,
  const double high ) const
{
  //
  if      ( s_equal ( low , high ) ) { return                  0.0 ; } // RETURN
  else if (           low > high   ) { return - integral ( high ,
                                                           low   ) ; } // RETURN
  //
  const double x_low  = m_mean - 5 * m_sigma ;
  const double x_high = m_mean + 5 * m_sigma ;
  //
  // split for the reasonable sub intervals:
  //
  if      ( low < x_low  && x_low < high )
  {
    return
      integral (   low , x_low  ) +
      integral ( x_low ,   high ) ;
  }
  else if ( low <  x_high && x_high < high )
  {
    return
      integral (   low  , x_high  ) +
      integral ( x_high ,   high  ) ;
  }
  //
  //
  // split, if the interval is too large
  //
  const double width = std::max ( std::abs  ( m_sigma)  , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &gram_charlier_A_GSL ;
  F.params   = const_cast<GramCharlierA*> ( this ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL :
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::GramCharlierA::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setM0  ( const double value )
{
  //
  if ( s_equal ( m_mean , value ) ) { return false ; }
  //
  m_mean  = value ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setSigma  ( const double value )
{
  //
  const double value_ = std::fabs ( value ) ;
  if ( s_equal ( m_sigma , value_ ) ) { return false ; }
  //
  m_sigma  = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setKappa3 ( const double value )
{
  if ( s_equal ( m_kappa3 , value )  ) { return false ; }
  //
  m_kappa4  = value ;
  //
  return false ;
}
// ============================================================================
bool Gaudi::Math::GramCharlierA::setKappa4 ( const double value )
{
  if ( s_equal ( m_kappa4 , value )  ) { return false ; }
  //
  m_kappa4  = value ;
  //
  return false ;
}
// ============================================================================


// ============================================================================
// constructor from two masses
// ============================================================================
Gaudi::Math::PhaseSpace2::PhaseSpace2
( const double m1 ,
  const double m2 )
  : std::unary_function<double,double> ()
  , m_m1 ( std::abs ( m1 ) )
  , m_m2 ( std::abs ( m2 ) )
{}
// ============================================================================
// desctructor
// ============================================================================
Gaudi::Math::PhaseSpace2::~PhaseSpace2(){}
// ============================================================================
// evaluate 2-body phase space
// ============================================================================
double Gaudi::Math::PhaseSpace2::operator () ( const double x ) const
{ return phasespace ( x , m_m1 , m_m2 ) ; }
// get the momentum at center of mass 
double Gaudi::Math::PhaseSpace2::q_  ( const double x ) const 
{ return q ( x , m1() , m2() ) ; }
// get the momentum at center of mass 
std::complex<double>
Gaudi::Math::PhaseSpace2::q1_ ( const double x ) const 
{ return q1 ( x , m1() , m2() ) ; }
// ============================================================================
/* calculate the phase space for   m -> m1 + m2
 *  \f$ \Phi = \frac{1}{8\pi} \frac{ \lambda^{\frac{1}{2}} \left( m^2 , m_1^2, m_2_2 \right) }{ m^2 }\f$,
 *  where \f$\lambda\f$ is a triangle function
 */
// ============================================================================
double Gaudi::Math::PhaseSpace2::phasespace
( const double         m  ,
  const double         m1 ,
  const double         m2 ,
  const unsigned short L  )
{
  //
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; } // RETURN
  if ( m < m1 + m2                ) { return 0 ; } // RETURN
  //
  const double msq = m * m ;
  const double lam = triangle ( msq  , m1 * m1 , m2 * m2 ) ;
  //
  static const double s_inv8pi = 1.0 / ( 8 * M_PI ) ;
  //
  return 0 < lam ?
    s_inv8pi * Gaudi::Math::pow ( std::sqrt ( lam ) / msq , 2 * L + 1 ) : 0.0 ;
}
// ============================================================================
/*  calculate the triangle function
 *  \f$ \lambda ( a , b, c ) = a^2 + b^2 + c^2 - 2ab - 2bc - 2 ca \f$
 *  @param a parameter a
 *  @param b parameter b
 *  @param c parameter b
 */
// ============================================================================
double
Gaudi::Math::PhaseSpace2::triangle
( const double a ,
  const double b ,
  const double c )
{ return a * a + b * b + c * c - 2 * a * b - 2 * b * c - 2 * a * c ; }
// ============================================================================
/*  calculate the particle momentum in rest frame
 *  @param m  the mass
 *  @param m1 the mass of the first particle
 *  @param m2 the mass of the second particle
 *  @return the momentum in rest frame (physical values only)
 */
// ============================================================================
double Gaudi::Math::PhaseSpace2::q
( const double m  ,
  const double m1 ,
  const double m2 )
{
  //
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; }
  //
  const double lam = triangle ( m * m  , m1 * m1 , m2 * m2 ) ;
  //
  return 0 < lam ? 0.5  * std::sqrt (  lam ) / m : 0 ;
}
// ============================================================================
/** calculate the particle momentum in rest frame
 *  @param m the mass
 *  @param m1 the mass of the first particle
 *  @param m2 the mass of the second particle
 *  @return the momentum in rest frame
 *  @return the momentum in rest frame  (imaginary for non-physical branch)
 */
// ============================================================================
std::complex<double>
Gaudi::Math::PhaseSpace2::q1
( const double m  ,
  const double m1 ,
  const double m2 )
{
  //
  if ( 0 >= m || 0 > m1 || 0 > m2 ) { return 0 ; }
  //
  const double lam = triangle ( m * m , m1 * m1 , m2 * m2 ) ;
  //
  return
    0 <= lam ?
    std::complex<double> (     0.5  * std::sqrt (  lam ) / m , 0 ) :
    std::complex<double> ( 0 , 0.5  * std::sqrt ( -lam ) / m     ) ;
}
// ============================================================================

// ============================================================================
/*  constructor from three masses
 *  @param m1 the mass of the first  particle
 *  @param m2 the mass of the second particle
 *  @param m3 the mass of the third  particle
 *  @param l1 the angular momentum between 1st and 2nd particle
 *  @param l2 the angular momentum between the pair and 3rd particle
 */
// ============================================================================
Gaudi::Math::PhaseSpace3::PhaseSpace3
( const double         m1 ,
  const double         m2 ,
  const double         m3 ,
  const unsigned short l1 ,
  const unsigned short l2 )
  : std::unary_function<double,double> ()
  , m_m1 ( std::abs ( m1 ) )
  , m_m2 ( std::abs ( m2 ) )
  , m_m3 ( std::abs ( m3 ) )
  , m_l1 ( l1 )
  , m_l2 ( l2 )
{}
// ============================================================================
// deststructor
// ============================================================================
Gaudi::Math::PhaseSpace3::~PhaseSpace3 () {}
// ============================================================================
// evaluate 3-body phase space
// ============================================================================
double Gaudi::Math::PhaseSpace3::operator () ( const double x ) const
{
  //
  if ( x <= lowEdge() ) { return 0 ; }
  //
  /// set the temporary mass
  m_tmp = x ;
  //
  // make integral of ps2_aux from m_m1+m_m2 till x-m_m3
  //
  const double low  = m_m1 + m_m2 ;
  const double high = x    - m_m3 ;
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_3_1_GSL ;
  const PhaseSpace3* _ps = this  ;
  F.params   = const_cast<PhaseSpace3*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::PhaseSpace3::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// helper function to get the phase space as
// ============================================================================
double Gaudi::Math::PhaseSpace3::ps2_aux
( const double m12 ) const
{
  //
  if ( m_tmp <= lowEdge()    ) { return 0 ; }
  //
  if ( m12   <= m_m1  + m_m2 ) { return 0 ; }
  if ( m12   >= m_tmp - m_m3 ) { return 0 ; }
  //
  // represent 3-body phase space as extention of 2-body phase space
  return  m12 / M_PI *
    Gaudi::Math::PhaseSpace2::phasespace ( m12   , m_m1 , m_m2 , m_l1 ) *
    Gaudi::Math::PhaseSpace2::phasespace ( m_tmp , m12  , m_m3 , m_l2 ) ;
  //
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::PhaseSpace3::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( lowEdge() >= high  ) { return 0 ; }
  if ( lowEdge() >  low   ) { return integral ( lowEdge() , high ) ; }

  //
  if ( 0 < lowEdge() && 5 * lowEdge() < ( high - low ) )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_3_2_GSL ;
  const PhaseSpace3* _ps = this  ;
  F.params   = const_cast<PhaseSpace3*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,             // the function
      low   , high      ,             // low & high edges
      s_PRECISION       ,             // absolute precision
      s_PRECISION       ,             // relative precision
      s_SIZE            ,             // size of workspace
      GSL_INTEG_GAUSS31 ,             // integration rule
      workspace ( m_workspace2 ) ,    // workspace
      &result           ,             // the result
      &error            ) ;           // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::PhaseSpace3::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// constructor from threshold and number of particles
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::PhaseSpaceLeft
( const double         threshold ,
  const unsigned short num       )
  : std::unary_function<double,double> ()
  , m_threshold ( std::abs ( threshold ) )
  , m_num       ( num )
{}
// ============================================================================
// constructor from list of masses
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::PhaseSpaceLeft
( const std::vector<double>& masses )
  : std::unary_function<double,double> ()
  , m_threshold ( 0              )
  , m_num       ( masses.size()  )
{
  //
  for ( std::vector<double>::const_iterator im = masses.begin() ;
        masses.end() != im ; ++im )
  {
    m_threshold += std::abs ( *im ) ;
  }
  //
}
// ============================================================================
// desctructor
// ============================================================================
Gaudi::Math::PhaseSpaceLeft::~PhaseSpaceLeft(){}
// ============================================================================
// evaluate N-body phase space near left threhsold
// ============================================================================
double Gaudi::Math::PhaseSpaceLeft::operator () ( const double x ) const
{
  //
  if ( m_threshold >= x ) { return 0 ; }
  //
  return std::pow ( x - m_threshold , 3 * 0.5 * m_num - 5 * 0.5  ) ;
}
// ============================================================================
// set the new value for threshold
// ============================================================================
bool Gaudi::Math::PhaseSpaceLeft::setThreshold ( const double x )
{
  //
  if ( s_equal ( x , m_threshold ) ) { return false ; } // RETURN
  //
  m_threshold = x ;
  //
  return true ;
  //
}
// ============================================================================
// constructor from threshold and number of particles
// ============================================================================
Gaudi::Math::PhaseSpaceRight::PhaseSpaceRight
( const double         threshold ,
  const unsigned short l         ,
  const unsigned short n         )
  : std::unary_function<double,double> ()
  , m_threshold ( std::abs ( threshold ) )
  , m_N         ( std::max ( l , n ) )
  , m_L         ( std::min ( l , n ) )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::PhaseSpaceRight::~PhaseSpaceRight (){}
// ============================================================================
// evaluate N-body phase space near right threshold
// ============================================================================
double Gaudi::Math::PhaseSpaceRight::operator () ( const double x ) const
{
  //
  if ( m_threshold <= x ) { return 0 ; }
  //
  return std::pow ( m_threshold - x , 1.5 * ( m_N - m_L ) - 1  ) ;
}
// ============================================================================
// set the new value for threshold
// ============================================================================
bool Gaudi::Math::PhaseSpaceRight::setThreshold ( const double x )
{
  //
  if ( s_equal ( x , m_threshold ) ) { return false ; } // RETURN
  //
  m_threshold = x ;
  //
  return true ;
  //
}

// ============================================================================
// constructor from thresholds and number of particles
// ============================================================================
Gaudi::Math::PhaseSpaceNL::PhaseSpaceNL
( const double         threshold1 ,
  const double         threshold2 ,
  const unsigned short l          ,
  const unsigned short n          )
  : std::unary_function<double,double> ()
  , m_threshold1 ( std::min ( std::abs ( threshold1 ) , std::abs ( threshold2 ) ) )
  , m_threshold2 ( std::max ( std::abs ( threshold1 ) , std::abs ( threshold2 ) ) )
  , m_N          ( std::max ( l , n ) )
  , m_L          ( std::min ( l , n ) )
  , m_norm       ( 1 )
//
  , m_workspace  ()
//
{
  if ( ( 3 * m_N * 0.5 - 3       * 0.5 ) < GSL_SF_GAMMA_XMAX &&
       ( 3 * m_L * 0.5 - 3       * 0.5 ) < GSL_SF_GAMMA_XMAX &&
       ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) < GSL_SF_GAMMA_XMAX )
  {
    m_norm  = gsl_sf_gamma   ( 3 * m_N * 0.5 - 3       * 0.5 ) ;
    m_norm /= gsl_sf_gamma   ( 3 * m_L * 0.5 - 3       * 0.5 ) ;
    m_norm /= gsl_sf_gamma   ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) ;
  }
  else
  {
    m_norm  = gsl_sf_lngamma ( 3 * m_N * 0.5 - 3       * 0.5 ) ;
    m_norm -= gsl_sf_lngamma ( 3 * m_L * 0.5 - 3       * 0.5 ) ;
    m_norm -= gsl_sf_lngamma ( 3 * m_N * 0.5 - 3 * m_L * 0.5 ) ;
    m_norm  = gsl_sf_exp     ( m_norm ) ;
  }
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::PhaseSpaceNL::~PhaseSpaceNL() {}
// ============================================================================
// evaluate N/L-body phase space
// ============================================================================
double Gaudi::Math::PhaseSpaceNL::operator () ( const double x ) const
{
  //
  if ( m_threshold1 >= x ) { return 0 ; }
  if ( m_threshold2 <= x ) { return 0 ; }
  //
  const double y = (  x - m_threshold1 ) / ( m_threshold2 - m_threshold1 ) ;
  if ( 0 >= y || 1 <= y )  { return 0 ; }
  //
  return m_norm
    / std::abs ( m_threshold2 - m_threshold1               )
    * std::pow (     y , 3 * 0.5 *   m_L         - 5 * 0.5 )
    * std::pow ( 1 - y , 3 * 0.5 * ( m_N - m_L ) - 1       ) ;
}
// =======================================================================
// set the thresholds
// =======================================================================
bool Gaudi::Math::PhaseSpaceNL::setThresholds
( const double mn ,
  const double mx )
{
  const double v1 = std::min ( std::abs ( mn ) ,std::abs ( mx ) ) ;
  const double v2 = std::max ( std::abs ( mn ) ,std::abs ( mx ) ) ;
  //
  if ( s_equal ( v1 , m_threshold1 ) &&
       s_equal ( v2 , m_threshold2 ) ) { return false ; }
  //
  m_threshold1 = v1 ;
  m_threshold2 = v2 ;
  //
  return true ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::PhaseSpaceNL::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( m_threshold2 <= low  ) { return 0 ; }
  if ( m_threshold1 >= high ) { return 0 ; }
  //
  if ( m_threshold1 >  low  ) { return integral ( m_threshold1 ,  high        ) ; }
  if ( m_threshold2 <  high ) { return integral ( low          , m_threshold2 ) ; }
  //
  // split, if the interval is too large
  //
  const double width = 0.2 * std::abs  ( m_threshold2 - m_threshold1 ) ;
  if ( 0 < width &&  width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &phase_space_NL_GSL ;
  const PhaseSpaceNL* _ps = this  ;
  F.params   = const_cast<PhaseSpaceNL*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::PhaseSpaceNL::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::PhaseSpaceNL::integral() const
{ return integral ( m_threshold1 , m_threshold2 ) ; }
// ============================================================================
// constructor
// ============================================================================
Gaudi::Math::BreitWigner::BreitWigner
( const double         m0   ,
  const double         gam0 ,
  const double         m1   ,
  const double         m2   ,
  const unsigned short L    )
  : std::unary_function<double,double> ()
//
  , m_m0        (             m0    )
  , m_gam0      ( std::abs ( gam0 ) )
  , m_m1        ( std::abs (   m1 ) )
  , m_m2        ( std::abs (   m2 ) )
  , m_L         (              L    )
  , m_rho_fun   ( &Gaudi::Math::Jackson::jackson_0 )
//
  , m_workspace ()
//
{}
// ============================================================================
// constructor
// ============================================================================
Gaudi::Math::BreitWigner::BreitWigner
( const double                                m0   ,
  const double                                gam0 ,
  const double                                m1   ,
  const double                                m2   ,
  const unsigned short                        L    ,
  const Gaudi::Math::BreitWigner::JacksonRho  r    )
  : std::unary_function<double,double> ()
//
  , m_m0        (             m0    )
  , m_gam0      ( std::abs ( gam0 ) )
  , m_m1        ( std::abs (   m1 ) )
  , m_m2        ( std::abs (   m2 ) )
  , m_L         (              L    )
//
  , m_rho_fun   ( &Gaudi::Math::Jackson::jackson_0 )
//
  , m_workspace ()
//
{
  //
  switch ( r )
  {
  case Jackson_0  :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_0  ; break ;
  case Jackson_A2 :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A2 ; break ;
  case Jackson_A3 :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A3 ; break ;
  case Jackson_A4 :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A4 ; break ;
  case Jackson_A5 :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A5 ; break ;
  case Jackson_A7 :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_A7 ; break ;
  default         :
    m_rho_fun = &Gaudi::Math::Jackson::jackson_0  ; break ;
  }
  //
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::BreitWigner::~BreitWigner (){}
// ============================================================================
//  calculate the Breit-Wigner amplitude
// ============================================================================
std::complex<double>
Gaudi::Math::BreitWigner::amplitude ( const double x ) const
{
  //
  if ( m_m1 + m_m2 >= x ) { return 0 ; }
  //
  const double g  = gamma ( x ) ;
  if ( 0 >= g ) { return 0 ; }
  //
  return std::sqrt ( m0 () * gam0 () ) * breit_amp ( x , m0() , g ) ;
}
// ============================================================================
/*  calculate the Breit-Wigner shape
 *  \f$\frac{1}{\pi}\frac{\omega\Gamma(\omega)}{ (\omega_0^2-\omega^2)^2-\omega_0^2\Gammma^2(\omega)-}\f$
 */
// ============================================================================
double Gaudi::Math::BreitWigner::breit_wigner ( const double x ) const
{
  //
  if ( m_m1 + m_m2 >= x ) { return 0 ; }
  //
  const double g  = gamma ( x ) ;
  if ( 0 >= g ) { return 0 ; }
  //
  std::complex<double> a = amplitude ( x ) ;
  //
  return 2 * x * std::norm ( a )* g / gam0() / M_PI ;
  //
  // const double omega2 = m_m0 * m_m0 ;
  // const double delta = omega2        -          x * x ;
  // const double v     = delta * delta + omega2 * g * g ;
  //
  // return 2 * x * m_m0 * g / v / M_PI  ;
}
// ============================================================================
/*  calculate the Breit-Wigner shape
 *  \f$\frac{1}{\pi}\frac{\omega\Gamma(\omega)}{ (\omega_0^2-\omega^2)^2-\omega_0^2\Gammma^2(\omega)-}\f$
 */
// ============================================================================
double Gaudi::Math::BreitWigner::operator() ( const double x ) const
{ return breit_wigner ( x ) ; }
// ============================================================================
// calculate the current width
// ============================================================================
double Gaudi::Math::BreitWigner::gamma ( const double x ) const
{
  //
  return gamma_run ( m_gam0    ,
                     x         ,
                     m_m1      ,
                     m_m2      ,
                     m_m0      ,
                     m_L       ,
                     m_rho_fun ) ;
  //
}
// ============================================================================
bool Gaudi::Math::BreitWigner::setM0     ( const double x )
{
  const double v       = std::abs ( x ) ;
  if ( s_equal ( v , m_m0 ) ) { return false ; } // RETURN
  m_m0   = v ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::BreitWigner::setGamma0 ( const double x )
{
  const double v       = std::abs ( x ) ;
  if ( s_equal ( v , m_gam0 ) ) { return false ; } // RETURN
  m_gam0  = v ;
  return true ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::BreitWigner::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( m_m1 + m_m2 >= high ) { return                              0   ; }
  if ( m_m1 + m_m2 >  low  ) { return integral  ( m_m1 + m_m2 , high ) ; }
  //
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_gam0 ;
  const double x2     = m_m0 + 10 * m_gam0  ;
  const double x_low  = std::min ( x1 , x2 ) ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  if ( low < x_low  && x_low < high )
  {
    return
      integral (   low , x_low  ) +
      integral ( x_low ,   high ) ;
  }
  if ( low <  x_high && x_high < high )
  {
    return
      integral (   low  , x_high  ) +
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  const double width = std::max ( m_gam0 , 0.0 ) ;
  if ( 0 < width &&  3 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &breit_wigner_GSL ;
  const BreitWigner* _bw = this  ;
  F.params   = const_cast<BreitWigner*> ( _bw ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL :
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BreitWigner::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral b
// ============================================================================
double  Gaudi::Math::BreitWigner::integral () const
{
  //
  // split into reasonable sub intervals
  //
  const double x1     = m_m0 - 10 * m_gam0 ;
  const double x2     = m_m0 + 10 * m_gam0  ;
  const double x_high = std::max ( x1 , x2 ) ;
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &breit_wigner_GSL ;
  const BreitWigner* _bw = this  ;
  F.params   = const_cast<BreitWigner*> ( _bw ) ;
  //
  //
  // right tail:
  //
  double result  =  0.0 ;
  double error   = -1.0 ;
  //
  const int ierror = gsl_integration_qagiu
    ( &F                ,         // the function
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision
      s_PRECISION_TAIL  ,         // relative precision
      s_SIZE            ,         // size of workspace
      workspace ( m_workspace ) , // workspace
      &result           ,         // the result
      &error            ) ;       // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BreitWigner::QAGIU" ,
                __FILE__ , __LINE__ , ierror ) ;
    result = 0.0 ;
  }
  //
  return result + integral ( m_m1 + m_m2 , x_high );
}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0::Rho0
( const double m0       ,
  const double gam0     ,
  const double pi_mass  )
  : Gaudi::Math::BreitWigner ( m0         ,
                               gam0       ,
                               pi_mass    ,
                               pi_mass    ,
                               1          ,
                               Jackson_A7 )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Rho0::~Rho0(){}

// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Kstar0::Kstar0
( const double m0       ,
  const double gam0     ,
  const double k_mass   ,
  const double pi_mass  )
  : Gaudi::Math::BreitWigner ( m0         ,
                               gam0       ,
                               k_mass     ,
                               pi_mass    ,
                               1          ,
                               Jackson_A2 )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Kstar0::~Kstar0(){}

// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Phi0::Phi0
( const double m0       ,
  const double gam0     ,
  const double k_mass   )
  : Gaudi::Math::BreitWigner ( m0         ,
                               gam0       ,
                               k_mass     ,
                               k_mass     ,
                               1          ,
                               Jackson_A2 )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Phi0::~Phi0(){}

// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::Rho0FromEtaPrime
( const double m0        ,
  const double gam0      ,
  const double pi_mass   ,
  const double eta_prime )
  : Gaudi::Math::Rho0 ( m0 , gam0 , pi_mass )
  , m_eta_prime ( std::abs ( eta_prime ) )
{}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::Rho0FromEtaPrime
( const Gaudi::Math::Rho0& rho       ,
  const double             eta_prime )
  : Gaudi::Math::Rho0 ( rho )
  , m_eta_prime ( std::abs ( eta_prime ) )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Rho0FromEtaPrime::~Rho0FromEtaPrime(){}
// ============================================================================
// calculate the function
// ============================================================================
double Gaudi::Math::Rho0FromEtaPrime::operator() ( const double x ) const
{
  //
  if ( m_eta_prime <= x ) { return 0 ; }
  //
  const double k_gamma = Gaudi::Math::PhaseSpace2::q ( m_eta_prime , x , 0 ) ;
  if ( 0 >= k_gamma     ) { return 0 ; }
  //
  const double rho     = breit_wigner ( x ) ;
  if ( 0 >= rho         ) { return 0 ; }
  //
  return rho * Gaudi::Math::pow ( 2 * k_gamma / m_eta_prime , 3 ) * 20 ;
  //
}
// ============================================================================
//               Flatte
// ============================================================================
/* constructor  from three parameters
 *  @param m0    the mass
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 */
// ============================================================================
Gaudi::Math::Flatte::Flatte
( const double m0    ,
  const double m0g1  ,
  const double g2og1 ,
  const double mK    ,
  const double mPi   )
  : std::unary_function<double,double> ()
//
  , m_m0    ( std::fabs ( m0    ) )
  , m_m0g1  ( std::fabs ( m0g1  ) )
  , m_g2og1 ( std::fabs ( g2og1 ) )
  , m_K     ( std::fabs ( mK    ) )
  , m_Pi    ( std::fabs ( mPi   ) )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Flatte::~Flatte(){}
// ============================================================================
// get the value of Flatte function
// ============================================================================
double Gaudi::Math::Flatte::operator() ( const double x ) const
{ return flatte ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte::amplitude
( const double x     )  const { return flatte_amp ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte::flatte_amp
( const double x     )  const
{
  //
  if ( 2 * mPi () >= x ) { return 0 ; }
  //
  const std::complex<double> rho_PP =
    Gaudi::Math::PhaseSpace2::q1 ( x , mPi() , mPi() )  ;
  const std::complex<double> rho_KK =
    Gaudi::Math::PhaseSpace2::q1 ( x , mK () , mK () )  ;
  //
  static const std::complex<double> s_j ( 0 , 1 ) ;
  //
  const std::complex<double> v =
    m0() * m0 () - x * x - s_j * m0g1() * ( rho_PP + g2og1 () * rho_KK ) ;
  //
  return  1.0 / v ;
}
// ==========================================================================
// get the complex Flatte amplitude (KK-channel)
// ==========================================================================
std::complex<double>
Gaudi::Math::Flatte::flatte2_amp
( const double x ) const
{
  //
  if ( 2 * mK() >= x ) { return 0 ; }
  //
  const std::complex<double> rho_PP =
    Gaudi::Math::PhaseSpace2::q1 ( x , mPi () , mPi () ) ;
  const std::complex<double> rho_KK =
    Gaudi::Math::PhaseSpace2::q1 ( x , mK  () , mK  () ) ;
  //
  static const std::complex<double> s_j ( 0 , 1 ) ;
  //
  const std::complex<double> v =
    m0 () * m0 () - x * x - s_j * m0g1 () * ( rho_PP + g2og1 () * rho_KK ) ;
  //
  return  1.0 / v ;
}
// ==========================================================================
// get the function for pipi-channel
// ============================================================================
double Gaudi::Math::Flatte::flatte ( const double x ) const
{
  //
  if ( 2 * mPi () >= x ) { return 0 ; }
  //
  // get the amplitude...
  std::complex<double> amp = flatte_amp ( x ) ;
  //
  const double ps = Gaudi::Math::PhaseSpace2::phasespace ( x ,  mPi() , mPi() ) ;
  //
  return x * ps * std::norm ( amp ) * 2 / M_PI * m0g1() ;
}
// ==========================================================================
// get the function for KK-channel
// ============================================================================
double Gaudi::Math::Flatte::flatte2 ( const double x ) const
{
  //
  if ( 2 * mK () >= x ) { return 0 ; }
  //
  // get the amplitude...
  std::complex<double> amp = flatte2_amp ( x ) ;
  //
  const double ps = Gaudi::Math::PhaseSpace2::phasespace ( x ,  mK() , mK() ) ;
  //
  return x * ps * std::norm ( amp ) * 2 / M_PI * m0g1 () * g2og1 () ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Flatte::integral
( const double low  ,
  const double high ) const
{
  //
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( 2 * m_Pi >= high ) { return                           0   ; }
  if ( 2 * m_Pi >  low  ) { return integral  ( 2 * m_Pi , high ) ; }
  //
  // split into reasonable sub intervals
  //
  const double x_low  = m_m0
    -  3 * std::abs ( m_m0g1 / m_m0           )
    -  3 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  const double x_high = m_m0
    + 10 * std::abs ( m_m0g1 / m_m0           )
    + 10 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  if      ( low <  x_low  && x_low  < high )
  {
    return
      integral (   low  , x_low   ) +
      integral ( x_low  ,   high  ) ;
  }
  else if ( low <  x_high && x_high < high )
  {
    return
      integral (   low  , x_high  ) +
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  const double width =
    0 > m_m0 ? 0.0 :
    std::fabs ( m_m0g1 / m_m0 ) +
    std::fabs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  if ( 0 < width &&  3 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &flatte_GSL ;
  const Flatte* _f = this  ;
  F.params   = const_cast<Flatte*> ( _f ) ;
  //
  //
  double result   =  1.0 ;
  double error    = -1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL :
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::Flatte::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral b
// ============================================================================
double  Gaudi::Math::Flatte::integral () const
{
  //
  // split into reasonable sub intervals
  //
  const double x_high = m_m0
    + 10 * std::abs ( m_m0g1 / m_m0           )
    + 10 * std::abs ( m_m0g1 / m_m0 * m_g2og1 ) ;
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &flatte_GSL ;
  const Flatte* _f = this  ;
  F.params   = const_cast<Flatte*> ( _f ) ;
  //
  // right tail:
  //
  double result  =  0.0 ;
  double error   = -1.0 ;
  //
  const int ierror = gsl_integration_qagiu
    ( &F                ,         // the function
      x_high            ,         // "low" edge
      s_PRECISION       ,         // absolute precision
      s_PRECISION_TAIL  ,         // relative precision
      s_SIZE            ,         // size of workspace
      workspace ( m_workspace ) , // workspace
      &result           ,         // the result
      &error            ) ;       // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::Flatte::QAGIU" ,
                __FILE__ , __LINE__ , ierror ) ;
    result = 0.0 ;
  }
  //
  return result + integral ( 2 * m_Pi , x_high );
}
// ============================================================================
// set mass
// ============================================================================
bool Gaudi::Math::Flatte::setM0     ( const double x )
{
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_m0 ) ) { return false ; }
  //
  m_m0 = v ;
  //
  return true ;
}
// ============================================================================
// set mass times G1
// ============================================================================
bool Gaudi::Math::Flatte::setM0G1   ( const double x )
{
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_m0g1 ) ) { return false ; }
  //
  m_m0g1 = v ;
  //
  return true ;
}
// ============================================================================
// set G2 over G1
// ============================================================================
bool Gaudi::Math::Flatte::setG2oG1  ( const double x )
{
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_g2og1 ) ) { return false ; }
  //
  m_g2og1 = v ;
  //
  return true ;
}
// ============================================================================



// ============================================================================
//               Flatte
// ============================================================================
/* constructor  from three parameters
 *  @param m0    the mass
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 */
// ============================================================================
Gaudi::Math::Flatte2::Flatte2
( const double m0    ,
  const double m0g1  ,
  const double g2og1 ,
  const double mK    ,
  const double mPi   )
  : Gaudi::Math::Flatte ( m0 , m0g1 , g2og1 ,mK , mPi )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Flatte2::~Flatte2(){}
// ============================================================================
// get the value of Flatte function
// ============================================================================
double Gaudi::Math::Flatte2::operator() ( const double x ) const
{ return flatte2 ( x ) ; }
// ============================================================================
// get the complex Flatte amplitude (pipi-channel)
// ============================================================================
std::complex<double> Gaudi::Math::Flatte2::amplitude
( const double x     )  const { return flatte2_amp ( x ) ; }
// ============================================================================


// ============================================================================
// Voigtian
// ============================================================================
// constructor  from all parameters
// ============================================================================
Gaudi::Math::Voigt::Voigt
( const double m0    ,
  const double gamma ,
  const double sigma ,
  const int    r     )
  : std::unary_function<double,double>()
//
  , m_m0        ( m0 )
  , m_gamma     ( std::abs ( gamma ) )
  , m_sigma     ( std::abs ( sigma ) )
  , m_r         ( std::min ( std::max ( r , 2 ) , 5 ) )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Voigt::~Voigt(){}
// ============================================================================
// get the value of Voigt function
// ============================================================================
double Gaudi::Math::Voigt::operator() ( const double x ) const
{ return TMath::Voigt ( x - m_m0 , m_sigma , m_gamma , m_r ) ; }
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Voigt::integral
( const double low  ,
  const double high ) const
{
  //
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  const double width = std::max ( m_sigma , m_gamma ) ;
  //
  // split into reasonable sub intervals
  //
  const double x_low   = m_m0 - 3 * width ;
  const double x_high  = m_m0 + 3 * width ;
  //
  if      ( low <  x_low  && x_low  < high )
  {
    return
      integral (   low  , x_low   ) +
      integral ( x_low  ,   high  ) ;
  }
  else if ( low <  x_high && x_high < high )
  {
    return
      integral (   low  , x_high  ) +
      integral ( x_high ,   high  ) ;
  }
  //
  // split, if interval too large
  //
  if ( 0 < width && 3 * width < high - low  )
  {
    return
      integral ( low                   , 0.5 *  ( high + low ) ) +
      integral ( 0.5 *  ( high + low ) ,          high         ) ;
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function = &voigt_GSL ;
  const Voigt* _f = this  ;
  F.params   = const_cast<Voigt*> ( _f ) ;
  //
  //
  double result   =  1.0 ;
  double error    = -1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      ( high   <= x_low  ) ? s_PRECISION_TAIL :
      ( x_high <=   low  ) ? s_PRECISION_TAIL :
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::Voigt::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Voigt::integral () const { return 1 ; }
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Voigt::setM0 ( const double x )
{
  //
  if ( s_equal ( x , m_m0 ) ) { return false ; }
  //
  m_m0 = x ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Voigt::setGamma ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_gamma ) ) { return false ; }
  //
  m_gamma = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Voigt::setSigma ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_sigma ) ) { return false ; }
  //
  m_sigma = v ;
  //
  return true ;
}



// ============================================================================
// LASS: Kpi S-wave 
// ============================================================================
/*  constructor from all masses and angular momenta
 *  @param m1 the mass of the first  particle
 *  @param m2 the mass of the second particle
 *  @param a  the LASS parameter
 *  @param r  the LASS parameter
 *  @param e  the LASS parameter
 */
// ============================================================================
Gaudi::Math::LASS::LASS
( const double         m1 ,
  const double         m2 ,
  const double         m0 ,
  const double         g0 ,
  const double         a  ,
  const double         r  ,
  const double         e  )
  : std::unary_function<double,double> ()
  , m_m0  ( std::abs ( m0 ) )
  , m_g0  ( std::abs ( g0 ) )
  , m_a   ( std::abs ( a  ) )
  , m_r   ( std::abs ( r  ) )
  , m_e   ( std::abs ( e  ) )
// phase space
  , m_ps2 ( m1 , m2 )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::LASS::~LASS(){}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::LASS::setM0 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_m0 ) ) { return false ; }
  //
  m_m0 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::LASS::setG0 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_g0 ) ) { return false ; }
  //
  m_g0 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::LASS::setA ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_a ) ) { return false ; }
  //
  m_a = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::LASS::setR ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_r ) ) { return false ; }
  //
  m_r = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::LASS::setE ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_e ) ) { return false ; }
  //
  m_e = v ;
  //
  return true ;
}
// ============================================================================
// get the (complex) LASS amplitude
// ============================================================================
std::complex<double>
Gaudi::Math::LASS::amplitude ( const double x ) const
{
  //
  const double q  = m_ps2.q_ ( x ) ;
  if ( 0 >= q                ) { return 0 ; }  // RETURN
  //
  // get the width:
  const double gs = gamma_run ( m_g0        ,
                                x           ,
                                m_ps2.m1 () ,
                                m_ps2.m2 () ,
                                m_m0        ,
                                // K*(1430) is a scalar! 
                                0           ) * m_m0 / x  ;
  //
  // phase shift:
  const double cotB = 1.0 / ( m_a * q ) + 0.5 * m_r * q  ;
  // phase shift:
  const double cotR = ( m_m0 * m_m0 - x * x )  / m_m0 / gs ;
  //
  const double sinB =  1.0 / std::sqrt ( 1 + cotB*cotB ) ;
  const double cosB = cotB * sinB ;
  //
  // exp( i*pi/2 )
  static const std::complex<double> i = std::complex<double>( 0 , 1 );
  //
  // exp( i*Delta_B )
  std::complex<double> deltaB ( cosB , sinB ) ;
  //
  // the amplitude
  std::complex<double> A =
    1.0 / ( cotB - i ) + m_e * deltaB * deltaB / ( cotR - i ) ;
  //
  // scale it!
  std::complex<double> T = A * ( x / q ) ;
  //
  return T ;
}
// ============================================================================
// get the phase space factor
// ============================================================================
double Gaudi::Math::LASS::phaseSpace ( const double x ) const
{ return std::max ( 0.0 , m_ps2 ( x ) ) ; }
// ============================================================================
// evaluate LASS
// ============================================================================
double Gaudi::Math::LASS::operator () ( const double x ) const
{
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude( x ) ) ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::LASS::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= m_ps2.lowEdge  () ) { return 0 ; }
  //
  if ( low  <  m_ps2.lowEdge  () )
  { return integral ( m_ps2.lowEdge() , high ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &LASS_GSL ;
  const LASS* _ps    = this  ;
  F.params           = const_cast<LASS*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::LASS::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================



// ============================================================================
/*  constructor from four masses and angular momenta
 *  @param m1 the mass of the first  particle
 *  @param m2 the mass of the second particle
 *  @param m3 the mass of the third  particle
 *  @param m4 the mass of the mother particle (m4>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and
 *  the third particle
 *  @param l  the angular momentum between the first and the second particle
 */
// ============================================================================
Gaudi::Math::PhaseSpace23L::PhaseSpace23L
( const double         m1 ,
  const double         m2 ,
  const double         m3 ,
  const double         m  ,
  const unsigned short L  ,
  const unsigned short l  )
  : std::unary_function<double,double> ()
//
  , m_m1   ( std::abs ( m1 ) )
  , m_m2   ( std::abs ( m2 ) )
  , m_m3   ( std::abs ( m3 ) )
  , m_m    ( std::abs ( m  ) )
  , m_l    (            l    )
  , m_L    (            L    )
//
  , m_norm ( -1 )
//
  , m_workspace  ()
//
{
  m_norm = integral() ;
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::PhaseSpace23L::~PhaseSpace23L() {}
// get the momentum of 1st particle in rest frame of (1,2)
// ============================================================================
double Gaudi::Math::PhaseSpace23L::q ( const double x ) const
{ return Gaudi::Math::PhaseSpace2::q ( x , m_m1 , m_m2 ) ; }
// ============================================================================
// get the momentum of 3rd particle in rest frame of mother
// ============================================================================
double Gaudi::Math::PhaseSpace23L::p ( const double x ) const
{ return Gaudi::Math::PhaseSpace2::q ( m_m , x , m_m3 ) ; }
// ============================================================================
// calculate the phase space
// ============================================================================
double Gaudi::Math::PhaseSpace23L::operator () ( const double x ) const
{ return ps23L( x ) ; }
// ============================================================================
// calculate the phase space
// ============================================================================
double Gaudi::Math::PhaseSpace23L::ps23L ( const double x ) const
{
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return  0 ; }
  //
  // represent 3-body phase space as extention of 2-body phase space
  double ps =  x / M_PI *
    Gaudi::Math::PhaseSpace2::phasespace ( x   , m_m1 , m_m2 , m_l  ) *
    Gaudi::Math::PhaseSpace2::phasespace ( m_m ,    x , m_m3 , m_L  ) ;
  //
  return 0 < m_norm ? ps / m_norm : ps ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::PhaseSpace23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () ) { return integral ( lowEdge() , high        ) ; }
  if ( high >  highEdge () ) { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function               = &phase_space_23L_GSL ;
  const PhaseSpace23L* _ps = this  ;
  F.params                 = const_cast<PhaseSpace23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::PhaseSpace23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::PhaseSpace23L::integral () const
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================


























// ============================================================================
// LASS: Kpi S-wave for  X -> (K pi) Y decays..
// ============================================================================
/*  constructor from all masses and angular momenta
 *  @param m1 the mass of the first  particle
 *  @param m2 the mass of the second particle
 *  @param m3 the mass of the third  particle
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third
 *  @param a  the LASS parameter
 *  @param r  the LASS parameter
 */
// ============================================================================
Gaudi::Math::LASS23L::LASS23L
( const double         m1 ,
  const double         m2 ,
  const double         m3 ,
  const double         m  ,
  const double         m0 ,
  const double         g0 ,
  const unsigned short L  ,
  const double         a  ,
  const double         r  ,
  const double         e  )
  : std::unary_function<double,double> ()
// LASS-fiunction 
  , m_lass ( m1 , m2 , m0 , g0  , a , r , e ) 
// phase space
  , m_ps   ( m1 , m2 , m3 , m   , L , 0 )
//
  , m_workspace ()
{}
// ============================================================================
/*  constructor from LASS and 3-rd particle 
 *  @param lass the actual lass shape 
 *  @param m3   the mass of third particle (Y)
 *  @param m    the mass of mother particle (X)
 *  @param L    the orbital momentum between Y and (Kpi) 
 */
// ============================================================================
Gaudi::Math::LASS23L::LASS23L
( const Gaudi::Math::LASS& lass   , 
  const double             m3     ,
  const double             m      ,
  const unsigned short     L      ) 
  : std::unary_function<double,double> ()
// LASS-fiunction 
  , m_lass ( lass ) 
// phase space
  , m_ps   ( lass.m1 ()  , lass.m2 () , m3 , m   , L , 0 )
//
  , m_workspace ()
{}  
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::LASS23L::~LASS23L(){}
// ============================================================================
// get the (complex) LASS amplitude
// ============================================================================
std::complex<double>
Gaudi::Math::LASS23L::amplitude ( const double x ) const
{ return m_lass.amplitude ( x )  ; }  
// ============================================================================
// get the phase space factor
// ============================================================================
double Gaudi::Math::LASS23L::phaseSpace ( const double x ) const
{ return std::max ( 0.0 , m_ps ( x ) ) ; }
// ============================================================================
// evaluate LASS
// ============================================================================
double Gaudi::Math::LASS23L::operator () ( const double x ) const
{
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude( x ) ) ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::LASS23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= m_ps.lowEdge  () ) { return 0 ; }
  if ( low  >= m_ps.highEdge () ) { return 0 ; }
  //
  if ( low  <  m_ps.lowEdge  () )
  { return integral ( m_ps.lowEdge() , high             ) ; }
  if ( high >  m_ps.highEdge () )
  { return integral ( low            , m_ps.highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &LASS_23L_GSL ;
  const LASS23L* _ps = this  ;
  F.params           = const_cast<LASS23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::LASS23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::LASS23L::integral () const
{ return integral ( m_ps.lowEdge () , m_ps.highEdge() ) ; }
// ============================================================================


// ============================================================================
// Bugg
// ============================================================================
/*  constructor from all masses and angular momenta
 *  @param M  mass of sigma (very different from the pole positon!)
 *  @param g2 width parameter g2 (4pi width)
 *  @param b1 width parameter b1  (2pi coupling)
 *  @param b2 width parameter b2  (2pi coupling)
 *  @param s1 width parameter s1  (cut-off for 4pi coupling)
 *  @param s2 width parameter s2  (cut-off for 4pi coupling)
 *  @param a  parameter a (the exponential cut-off)
 *  @param m1 the mass of the first  particle
 */
// ============================================================================
Gaudi::Math::Bugg::Bugg
( const double         M  ,
  const double         g2 ,
  const double         b1 ,
  const double         b2 ,
  const double         a  ,
  const double         s1 ,
  const double         s2 ,
  const double         m1 )
  : std::unary_function<double,double> ()
//
  , m_M  ( std::abs ( M  ) )
  , m_g2 ( std::abs ( g2 ) )
  , m_b1 ( std::abs ( b1 ) )
  , m_b2 ( std::abs ( b2 ) )
  , m_s1 ( std::abs ( s1 ) )
  , m_s2 ( std::abs ( s2 ) )
  , m_a  ( std::abs ( a  ) )
// phase space
  , m_ps ( m1 , m1 )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Bugg::~Bugg(){}
// ============================================================================
double Gaudi::Math::Bugg::rho2_ratio ( const double x ) const
{
  if ( lowEdge() >= x ) { return 0 ; }
  //
  return
    Gaudi::Math::PhaseSpace2::phasespace ( x    , m1() , m2 () ) /
    Gaudi::Math::PhaseSpace2::phasespace ( M () , m1() , m2 () ) ;
}
// ============================================================================
std::complex<double>
Gaudi::Math::Bugg::rho4_ratio ( const double x ) const
{
  //
  if ( 2 * m1() >= x ) { return 0 ; }
  //
  return rho4 ( x ) / rho4 ( M() ) ;
}
// ============================================================================
std::complex<double>
Gaudi::Math::Bugg::rho4 ( const double x ) const
{
  const double s  = x * x ;
  //
  const double r2 = 1 - 16 * m1() * m1() / s ;
  //
  const double r  =
    std::sqrt ( std::abs ( r2 ) ) *
    ( 1 + std::exp ( ( s1 () - s )  / s2 () ) ) ;
  //
  return 0 <= r2 ?
    std::complex<double> ( r , 0 ) :
    std::complex<double> ( 0 , r ) ;
}
// ============================================================================
// Adler's pole
// ============================================================================
double Gaudi::Math::Bugg::adler ( const double x ) const
{
  if ( lowEdge() >= x ) { return 0 ; }
  //
  const double pole = 0.5 * m1 () * m1 ()  ;
  //
  return ( x * x - pole ) / ( M2 () - pole ) ;
}
// ============================================================================
// get the running width by Bugg
// ============================================================================
std::complex<double>
Gaudi::Math::Bugg::gamma ( const double x ) const
{
  //
  if ( lowEdge() >= x ) { return 0 ; }
  //
  const double s = x * x ;
  //
  const double g1 =
    b     ( x ) *
    adler ( x ) * std::exp ( -1 * ( s - M2() )  / a() ) ;
  //
  return g1 * rho2_ratio ( x ) + g2 () * rho4_ratio ( x ) ;
}
// ============================================================================
// get the amlitude  (not normalized!)
// ============================================================================
std::complex<double>
Gaudi::Math::Bugg::amplitude (  const double x ) const
{
  if ( lowEdge() >= x ) { return 0 ; }
  //
  static const std::complex<double> j ( 0 , 1 ) ;
  //
  std::complex<double> d = M2() - x * x  - j * M() * gamma ( x ) ;
  //
  return 1.0 / d ;
}
// ============================================================================
// evaluate Bugg
// ============================================================================
double Gaudi::Math::Bugg::pdf ( const double x ) const
{
  //
  if ( lowEdge() >= x ) { return 0 ; }
  //
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude ( x ) ) ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setM ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_M ) ) { return false ; }
  //
  m_M = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setG2 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_g2 ) ) { return false ; }
  //
  m_g2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setB1 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_b1 ) ) { return false ; }
  //
  m_b1 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setB2 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_b2 ) ) { return false ; }
  //
  m_b2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setS1 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_s1 ) ) { return false ; }
  //
  m_s1 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setS2 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_s2 ) ) { return false ; }
  //
  m_s2 = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Bugg::setA ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_a ) ) { return false ; }
  //
  m_a = v ;
  //
  return true ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Bugg::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () )
  { return integral ( lowEdge() , high        ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &Bugg_GSL ;
  const Bugg*    _ps = this  ;
  F.params           = const_cast<Bugg*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BUGG::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
// double  Gaudi::Math::Bugg23L::integral () const
// { return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================



// ============================================================================
// Bugg23L
// ============================================================================
/*  constructor from all masses and angular momenta
 *  @param M  mass of sigma (very different from the pole positon!)
 *  @param g2 width parameter g2 (4pi width)
 *  @param b1 width parameter b1  (2pi coupling)
 *  @param b2 width parameter b2  (2pi coupling)
 *  @param s1 width parameter s1  (cut-off for 4pi coupling)
 *  @param s2 width parameter s2  (cut-off for 4pi coupling)
 *  @param a  parameter a (the exponential cut-off)
 *  @param m1 the mass of the first  particle
 *  @param m3 the mass of the third  particle
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third
 */
// ============================================================================
Gaudi::Math::Bugg23L::Bugg23L
( const double         M  ,
  const double         g2 ,
  const double         b1 ,
  const double         b2 ,
  const double         a  ,
  const double         s1 ,
  const double         s2 ,
  const double         m1 ,
  const double         m3 ,
  const double         m  ,
  const unsigned short L  )
  : std::unary_function<double,double> ()
//
  , m_bugg ( M  , g2 , b1 , b2 , a , s1 , s2 , m1 ) 
  , m_ps   ( m1 , m1 , m3 , m  , L , 0 )
//
  , m_workspace ()
{}
// ============================================================================
/** constructor from bugg & phase space parameters 
 *  @param m3 the mass of the third  particle
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third
 */
// ============================================================================
Gaudi::Math::Bugg23L::Bugg23L
( const Gaudi::Math::Bugg& bugg ,
  const double             m3   ,  // MeV
  const double             m    ,  // MeV
  const unsigned short     L    ) 
  : std::unary_function<double,double> ()
//
  , m_bugg ( bugg ) 
  , m_ps   ( bugg.m1 () , bugg.m1 ()  , m3 , m  , L , 0 )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Bugg23L::~Bugg23L(){}
// ============================================================================
// evaluate Bugg
// ============================================================================
double Gaudi::Math::Bugg23L::pdf ( const double x ) const
{
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return 0 ; }
  //
  const double result = phaseSpace  ( x ) ;
  if ( 0 >= result ) { return 0 ; }
  //
  return result * std::norm ( amplitude ( x ) ) ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Bugg23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () )
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () )
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                 ;
  F.function         = &Bugg_23L_GSL ;
  const Bugg23L* _ps = this  ;
  F.params           = const_cast<Bugg23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BUGG23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::Bugg23L::integral () const
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================





// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const double         m0   ,
  const double         gam0 ,
  const double         m1   ,
  const double         m2   ,
  const double         m3   ,
  const double         m    ,
  const unsigned short L1   ,
  const unsigned short L2   )
  : std::unary_function<double,double>()
//
  , m_bw ( m0 , gam0 , m1  , m2 , L1      )
  , m_ps ( m1 , m2   , m3  , m  , L2 , L1 )
//
  , m_workspace ()
{}
// ============================================================================
// constructor from all parameters
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const double                               m0   ,
  const double                               gam0 ,
  const double                               m1   ,
  const double                               m2   ,
  const double                               m3   ,
  const double                               m    ,
  const unsigned short                       L1   ,
  const unsigned short                       L2   ,
  const Gaudi::Math::BreitWigner::JacksonRho r    )
  : std::unary_function<double,double>()
//
  , m_bw ( m0 , gam0 , m1  , m2 , L1 , r  )
  , m_ps ( m1 , m2   , m3  , m  , L2 , L1 )
//
  , m_workspace ()
{}
// ============================================================================
// constructor from BreitWigner
// ============================================================================
Gaudi::Math::BW23L::BW23L
( const Gaudi::Math::BreitWigner& bw ,
  const double                    m3 ,
  const double                    m  ,
  const unsigned short            L2 )
  : std::unary_function<double,double>()
//
  , m_bw ( bw )
  , m_ps ( bw.m1() , bw.m2() , m3  , m  , L2 , bw. L())
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::BW23L::~BW23L (){}
// ============================================================================
// calculate the shape
// ============================================================================
double Gaudi::Math::BW23L::operator() ( const double x ) const
{
  if (  lowEdge() >= x || highEdge()  <= x ) { return 0 ; }
  //
  const double bw = std::norm ( m_bw.amplitude ( x ) )   ;
  //
  // // get the incomplete phase space factor
  // const double ps  =                   // get the incomplete phase space factor
  //   x / M_PI *
  //   // =======================================================================
  //   // the second factor is already in our BW !!!
  //   Gaudi::Math::PhaseSpace2::phasespace ( x          , 
  //                                          m_bw.m1 () , 
  //                                          m_bw.m2 () , 
  //                                          m_bw.L  () ) *
  //   // =======================================================================
  //   Gaudi::Math::PhaseSpace2::phasespace ( m_ps.m  () ,
  //                                          x          ,
  //                                          m_ps.m3 () ,
  //                                          m_ps.L  () ) ;
  //
  return bw * m_ps ( x ) ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::BW23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () )
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () )
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function         = &BW_23L_GSL ;
  const BW23L* _ps   = this  ;
  F.params           = const_cast<BW23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BW23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::BW23L::integral () const
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================



// ============================================================================
// Flatte23L
// ============================================================================
/*  constructor  from all parameters
 *  @param m0    the mass
 *  @param m0g1  parameter \f$ m_0\times g_1\f$
 *  @param g2og2 parameter \f$ g2/g_1       \f$
 *  @param mK    kaon mass
 *  @param mPi   pion mass
 *  @param m3    the mass of the third particle
 *  @param m     the mass of mother particle
 *  @param L     the orbital momentum between the pair and the third particle
 */
// ============================================================================
Gaudi::Math::Flatte23L::Flatte23L
( const double         m0    ,     // MeV
  const double         m0g1  ,     // MeV^2
  const double         g2og1 ,     // dimensionless
  const double         mK    ,     // MeV
  const double         mPi   ,     // MeV
  const double         m3    ,     // MeV
  const double         m     ,     // MeV
  const unsigned short L     )
  : std::unary_function<double,double> ()
//
  , m_flatte    ( m0  , m0g1 , g2og1 , mK , mPi  )
  , m_ps        ( mPi , mPi  , m3    , m  , L    )
//
  , m_workspace ()
{}
// ============================================================================
/* constructor  from flatte function
 *  @param m3    the mass of the third particle
 *  @param m     the mass of mother particle
 *  @param L     the orbital momentum between the pair and the third particle
 */
// ============================================================================
Gaudi::Math::Flatte23L::Flatte23L
( const Gaudi::Math::Flatte& fun ,     // MeV
  const double               m3  ,     // MeV
  const double               m   ,     // MeV
  const unsigned short       L   )
  : std::unary_function<double,double> ()
//
  , m_flatte    ( fun )
  , m_ps        ( fun.mPi() , fun.mPi()  , m3    , m  , L    )
//
  , m_workspace ()
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Flatte23L::~Flatte23L (){}
// ============================================================================
// get the value of Flatte function
// ============================================================================
double Gaudi::Math::Flatte23L::operator() ( const double x ) const
{
  //
  if ( lowEdge () >= x || highEdge() <= x ) { return 0 ; } // RETURN
  //
  // get the amplitude...
  std::complex<double> amp = m_flatte.flatte_amp ( x ) ;
  //
  return m_ps ( x ) * std::norm ( amp ) * 2 / M_PI * m0g1() ;
}
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Flatte23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () )
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () )
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function             = &Flatte_23L_GSL ;
  const Flatte23L* _ps   = this  ;
  F.params               = const_cast<Flatte23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::BW23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::Flatte23L::integral () const
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================


// ============================================================================
// Gounaris & Sakurai shape
// ============================================================================
/* constructor from all masses and angular momenta
 *  @param M  mass of rho
 *  @param g0 width parameter
 *  @param m1 the mass of the first  particle (the same as the second)
 *  @param m3 the mass of the third  particle
 *  @param m  the mass of the mother particle (m>m1+m2+m3)
 *  @param L  the angular momentum between the first pair and the third
 */
// ============================================================================
Gaudi::Math::Gounaris23L::Gounaris23L
( const double         M  ,  // GeV
  const double         g0 ,  // GeV
  const double         m1 ,  // MeV
  const double         m3 ,  // MeV
  const double         m  ,  // MeV
  const unsigned short L  )
  : std::unary_function<double,double>  ()
//
  , m_M  ( std::abs ( M  ) )
  , m_g0 ( std::abs ( g0 ) )
//
  , m_ps ( m1 , m1 , m3 , m , L , 1 )
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Gounaris23L::~Gounaris23L(){}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Gounaris23L::setM ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_M ) ) { return false ; }
  //
  m_M = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::Gounaris23L::setG0 ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_g0 ) ) { return false ; }
  //
  m_g0 = v ;
  //
  return true ;
}
// ============================================================================
// get h-factor
// ============================================================================
double Gaudi::Math::Gounaris23L::h ( const double x ,
                                     const double k ) const
{
  //
  if ( lowEdge() > x || highEdge() < x ) { return 0 ; }
  //
  return 2 * k  / M_PI / x * std::log ( ( x + 2 * k ) / 2 / m1() ) ;
}
// ============================================================================
// get h-factor
// ============================================================================
double Gaudi::Math::Gounaris23L::h ( const double x ) const
{
  //
  if ( lowEdge() > x ) { return 0 ; }
  //
  const double k = PhaseSpace2::q ( x , m1 () , m1() ) ;
  //
  return h ( x , k ) ;
}
// ============================================================================
// get h'-factor
// ============================================================================
double Gaudi::Math::Gounaris23L::h_prime ( const double x ) const
{
  //
  if ( lowEdge() > x ) { return 0 ; }
  //
  const double k = PhaseSpace2::q ( x , m1 () , m1() ) ;
  //
  return h_prime ( x , k ) ;
}
// ============================================================================
// get h'-factor
// ============================================================================
double Gaudi::Math::Gounaris23L::h_prime ( const double x ,
                                           const double k ) const
{
  //
  if ( lowEdge() > x ) { return 0 ; }
  //
  const double f =  ( x + 2 * k ) / ( 2  * m1 () ) ;
  //
  return k / M_PI / x / x * ( - std::log ( f ) / x  + 0.5 / m1() / f ) ;
}
// ============================================================================
// get the amlitude  (not normalized!)
// ============================================================================
std::complex<double>
Gaudi::Math::Gounaris23L::amplitude (  const double x ) const
{
  //
  if ( x <= lowEdge() ) { return 0 ; }
  //
  const double k    = PhaseSpace2::q ( x    , m1 () , m1 () ) ;
  const double k0   = PhaseSpace2::q ( M () , m1 () , m1 () ) ;
  const double k03  = k0 * k0 * k0 ;
  //
  const double m0_2 = M() * M() ;
  //
  const double v1   = m0_2 - x * x ;
  //
  const double dh   = h ( x , k ) - h ( M() , k0 ) ;
  const double hp   = h_prime ( m() , k0 ) ;
  //
  const double v2 = k * k * dh + k0 * k0 * hp * ( m0_2 - x * x ) ;
  const double v3 = Gaudi::Math::pow ( k/k0 , 3 ) * m0() / x ;
  //
  return
    std::sqrt ( g0 () * m0 () ) /
    std::complex<double> ( v1 + v2 * g0() * m0_2 / k03 ,
                           v3      * g0() * m0 ()      ) ;
}
// ============================================================================
// calculate the Gounaris-Sakurai shape
// ============================================================================
double Gaudi::Math::Gounaris23L::operator() ( const double x ) const
{
  //
  if ( lowEdge() >= x || highEdge() <= x ) { return 0 ; }
  //
  std::complex<double> amp = amplitude ( x ) ;
  const double  ps = m_ps( x ) ;
  //
  return x * ps * std::norm ( amp ) * 2 / M_PI  ;
}

// ============================================================================
// get the integral between low and high limits
// ============================================================================
double  Gaudi::Math::Gounaris23L::integral
( const double low  ,
  const double high ) const
{
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  if ( high <= lowEdge  () ) { return 0 ; }
  if ( low  >= highEdge () ) { return 0 ; }
  //
  if ( low  <  lowEdge  () )
  { return integral ( lowEdge() , high        ) ; }
  //
  if ( high >  highEdge () )
  { return integral ( low       , highEdge () ) ; }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function             = &Gounaris_23L_GSL ;
  const Gounaris23L* _ps = this  ;
  F.params               = const_cast<Gounaris23L*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION     ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::Gounaris23L::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================
// get the integral
// ============================================================================
double  Gaudi::Math::Gounaris23L::integral () const
{ return integral ( lowEdge () , highEdge() ) ; }
// ============================================================================

// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : std::unary_function<double,double> ()
//
  , m_pars ( N + 1 , 0.0 )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
//
{}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein::Bernstein
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax )
  : std::unary_function<double,double> ()
//
  , m_pars ( pars )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
//
{
  if ( m_pars.empty() ) { m_pars.push_back( 0 ) ; }
}
// ============================================================================
// get the integral between xmin and xmax
// ============================================================================
double Gaudi::Math::Bernstein::integral () const
{
  //
  return
    ( m_xmax - m_xmin ) *
    std::accumulate ( m_pars.begin() , m_pars.end() , 0.0 ) / npars() ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Bernstein::setPar
( const unsigned short k , const double value )
{
  //
  if ( k >= npars() ){ return false ; }
  //
  if ( s_equal ( par ( k ) , value ) ) { return false ; }
  //
  m_pars [ k ] = value ;
  //
  return true ;
}
// ============================================================================
namespace
{
  // ==========================================================================
  // De Casteljau's algorithm
  double _casteljau_
  ( const std::vector<double>& beta ,
    const double               t0   ,
    const double               t1   )
  {
    // the trivial cases
    if      ( beta.empty()     ) { return 0       ; }
    else if ( 1 == beta.size() ) { return beta[0] ; }
    else if ( 2 == beta.size() ) { return beta[0]*t1 + beta[1]*t0 ; }
    //
    // prepare recursion
    const std::size_t N = beta.size() - 1 ;
    std::vector<double> beta_prime ( N , 0 ) ;
    for ( std::size_t i = 0 ; i < N ; ++i )
    { beta_prime[i] = beta [ i ] * t1 + beta [ i+1 ] * t0 ; }
    //
    // recursion
    return _casteljau_ ( beta_prime , t0 , t1 ) ;
  }
  // ==========================================================================
  // De Casteljau's algorithm
  template <class IT>
  double _casteljau_
  ( IT first ,
    IT last  ,
    const double               t0   ,
    const double               t1   )
  {
    // the trivial cases
    if      ( first == last    ) { return 0       ; }
    //
    const std::size_t len  = std::distance ( first , last  ) ;
    //
    if      ( 1 == len ) { return       *first                        ; }
    else if ( 2 == len ) { return t1 * (*first) + t0 * ( *(first+1) ) ; }
    //
    IT second = --last ;
    //
    // prepare recursion
    for ( IT it = first ; it != second ; ++it )
    { *it = t1 * ( *it )  + t0 * ( *( it+1 ) ) ; }
    //
    // recursion
    return _casteljau_ ( first , second , t0 , t1 ) ;
  }
  // ==========================================================================
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein::operator () ( const double x ) const
{
  /// the trivial cases
  if ( x < m_xmin || x > m_xmax ) { return 0.0  ; }
  //
  if      ( 0 == npars () ) { return 0.0        ; }
  else if ( 1 == npars () ) { return m_pars [0] ; }
  ///
  // get the t-values
  const double t0 = t ( x ) ;
  const double t1 = 1 - t0  ;
  //
  std::vector<double> beta = m_pars ;
  return _casteljau_ ( beta.begin() , beta.end() , t0 , t1 ) ;
}
// ============================================================================
namespace
{
  // ==========================================================================
  /// calcluate the intial phase for  unite vecor 
  inline void _phi0_ ( std::vector<double>& phi0 )
  {
    const std::size_t N = phi0.size() ;
    for ( std::size_t i = 0 ; i < N ; ++i )
    {
      const long double ni = N - i ;
      phi0[i] = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;
    }
  }
  // ==========================================================================
  /// calculate the binomial coefficients 
  inline long double binomial
  ( const unsigned short n , 
    const unsigned short k ) 
  {
    //
    return 
      (  0 == k || 1 == n ) ? 1.0                    :
      (  k > n - k )        ? binomial ( n , n - k ) : 
      n * binomial ( n - 1 , k - 1 ) / k  ;
  }
  // ==========================================================================
}
// ============================================================================

// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein2D::Bernstein2D
( const unsigned short      nX   ,
  const unsigned short      nY   ,
  const double              xmin ,
  const double              xmax ,
  const double              ymin ,
  const double              ymax )
  : std::binary_function<double,double,double> ()
//
  , m_nx   ( nX ) 
  , m_ny   ( nY )
//
  , m_pars ( ( nX + 1 ) * ( nY + 1 ) , 0.0 )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
  , m_ymin ( std::min ( ymin , ymax ) )
  , m_ymax ( std::max ( ymin , ymax ) )
//
  , m_cx   () 
  , m_cy   () 
//
{
  for ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
  { m_cy . push_back ( binomial ( m_ny , iy ) ) ; }
  for ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  { m_cx . push_back ( binomial ( m_nx , ix ) ) ; }
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein2D::operator () ( const double x ,
                                               const double y ) const
{
  /// the trivial cases
  if ( x < m_xmin || x > m_xmax ) { return 0.0        ; }
  if ( y < m_ymin || y > m_ymax ) { return 0.0        ; }
  //
  if      ( 0 == npars ()       ) { return 0.0        ; }
  else if ( 1 == npars ()       ) { return m_pars [0] ; }
  ///
  const double _tx = tx ( x ) ;
  const double _ty = ty ( y ) ;
  //
  double       result = 0 ;
  //
  std::vector<double> fy ( m_ny + 1 , 0 ) ;
  for ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
  {
    fy[iy] = 
      m_cy[iy] * 
      Gaudi::Math::pow (     _ty ,        iy ) * 
      Gaudi::Math::pow ( 1 - _ty , m_ny - iy ) ;
  }
  //
  for  ( unsigned short ix = 0 ; ix <= m_nx ; ++ix ) 
  {
    const double fx =   
      m_cx[ix] * 
      Gaudi::Math::pow (     _tx ,        ix ) * 
      Gaudi::Math::pow ( 1 - _tx , m_nx - ix ) ;
    //
    for  ( unsigned short iy = 0 ; iy <= m_ny ; ++iy ) 
    {
      //
      result += par ( ix , iy ) * fx * fy[iy] ;
    }
  }
  //
  return result ;
}
// ============================================================================

// ============================================================================
// set (l,m)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2D::setPar
( const unsigned short l     , 
  const unsigned short m     , 
  const double         value )
{
  if ( l > m_nx || m > m_ny )             { return false ; }
  const unsigned int k =  l * ( m_ny + 1 ) + m ;
  return setPar ( k , value ) ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2D::setPar
( const unsigned int   k     , 
  const double         value )
{
  if ( k >= npars() )                     { return false ; }
  if ( s_equal ( m_pars [ k ] , value ) ) { return false ; }
  m_pars [ k ] = value ;
  return true ;
}
// ============================================================================
// get (l,m)-parameter 
// ============================================================================
double  Gaudi::Math::Bernstein2D::par 
( const unsigned short l ,
  const unsigned short m ) const 
{
  if ( l > m_nx || m > m_ny ) { return 0 ; }
  const unsigned int k =  l * ( m_ny + 1 ) + m ;
  return par ( k ) ;
}
// ============================================================================

  


// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Bernstein2DSym::Bernstein2DSym
( const unsigned short      n    ,
  const double              xmin ,
  const double              xmax )
  : std::binary_function<double,double,double> ()
//
  , m_n    ( n ) 
//
  , m_pars ( ( n + 1 ) * ( n + 2 ) / 2 , 0.0 )
//
  , m_xmin ( std::min ( xmin , xmax ) )
  , m_xmax ( std::max ( xmin , xmax ) )
//
  , m_c () 
//
{
  for ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  { m_c . push_back ( binomial ( m_n , i ) ) ; }
}
// ============================================================================
// get the value
// ============================================================================
double Gaudi::Math::Bernstein2DSym::operator () 
  ( const double x ,
    const double y ) const
{
  /// the trivial cases
  if ( x < xmin () || x > xmax () ) { return 0.0        ; }
  if ( y < ymin () || y > ymax () ) { return 0.0        ; }
  //
  if      ( 0 == npars ()       ) { return 0.0        ; }
  else if ( 1 == npars ()       ) { return m_pars [0] ; }
  ///
  const double _tx = tx ( x ) ;
  const double _ty = ty ( y ) ;
  //
  double       result = 0 ;
  //
  std::vector<double> fy ( m_n + 1 , 0 ) ;
  for ( unsigned short i = 0 ; i <= m_n ; ++i ) 
  {
    fy[i] = 
      m_c[i] * 
      Gaudi::Math::pow (     _ty ,       i ) * 
      Gaudi::Math::pow ( 1 - _ty , m_n - i ) ;
  }
  //
  for  ( unsigned short ix = 0 ; ix <= m_n ; ++ix ) 
  {
    const double fx =   
      m_c [ix] * 
      Gaudi::Math::pow (     _tx ,       ix ) * 
      Gaudi::Math::pow ( 1 - _tx , m_n - ix ) ;
    //
    for  ( unsigned short iy = ix ; iy <= m_n ; ++iy ) 
    {
      //
      const double dr = par ( ix , iy ) * fx * fy[iy] ;  
      //
      result += ( ix == iy ) ? dr : 2*dr ; 
    }
  }
  //
  return result ;
}
// ============================================================================
// set (k)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2DSym::setPar
( const unsigned int   k     , 
  const double         value )
{
  //
  if ( k >= npars() )                     { return false ; }
  if ( s_equal ( m_pars [ k ] , value ) ) { return false ; }
  m_pars [ k ] = value ;
  //
  return true ;
}
// ============================================================================
// set (l,m)-parameter
// ============================================================================
bool Gaudi::Math::Bernstein2DSym::setPar
( const unsigned short l     , 
  const unsigned short m     , 
  const double         value )
{
  //
  if ( l > m_n || m > m_n )               { return false ; }
  //
  const unsigned int k = ( l < m ) ? 
    ( m * ( m + 1 ) / 2 + l ) : 
    ( l * ( l + 1 ) / 2 + m ) ;
  //
  return setPar ( k , value ) ;
}
// ============================================================================
// get (l,m)-parameter 
// ============================================================================
double Gaudi::Math::Bernstein2DSym::par
( const unsigned short l ,
  const unsigned short m ) const 
{
  //
  if ( l > m_n || m > m_n )               { return 0 ; }
  //
  const unsigned int k = ( l < m ) ? 
    ( m * ( m + 1 ) / 2 + l ) : 
    ( l * ( l + 1 ) / 2 + m ) ;
  //
  return par ( k ) ;
}


// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive::Positive
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : std::unary_function<double,double> ()
//
  , m_bernstein ( N , xmin , xmax )
  , m_phases    ( N , 0 )
  , m_phi0      ( N , 0 )
  , m_sin2      ( N , 0 )
{
  //
  _phi0_ ( m_phi0 ) ;
  //
  for ( unsigned short i = 0 ; i < N ; ++i )
  {
    const double s = std::sin ( m_phi0[i] ) ;
    m_sin2 [ i ] = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive::Positive
( const std::vector<double>& pars ,
  const double               xmin ,
  const double               xmax )
  : std::unary_function<double,double> ()
//
  , m_bernstein ( pars.size () , xmin , xmax )
  , m_phases    ( pars             )
  , m_phi0      ( pars.size () , 0 )
  , m_sin2      ( pars.size () , 0 )
{
  //
  _phi0_ ( m_phi0 ) ;
  //
  for ( unsigned int i =  0 ; i < m_phases.size() ; ++i )
  {
    const double s = std::sin ( m_phases[i]  + m_phi0 [ i ] ) ;
    m_sin2 [ i ]   = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive::setPar ( const unsigned short k , const double value )
{
  //
  if (  k >= m_phases.size() )         { return false ; } // FALSE
  //
  if ( s_equal ( value , par ( k ) ) ) { return false ; }
  //
  const double s  = std::sin ( value + m_phi0[k] ) ;
  const double s2 =  s * s ;
  //
  if ( s_equal ( s2 , m_sin2 [ k ] ) ) { return false ; }
  //
  m_phases [ k ] = value ;
  m_sin2   [ k ] = s2    ;
  //
  return updateBernstein ( k ) ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive::updateBernstein ( const unsigned int k )
{
  //
  double psin2 = 1 ;
  for ( unsigned int i = 0 ; i < k ; ++ i ) { psin2 *= m_sin2[i] ; }
  //
  bool update = false ;
  const std::size_t np = npars() + 1 ;
  for ( unsigned int i = k ; i < npars() ; ++i )
  {
    const double sin2 = m_sin2   [i] ;
    const double cos2 = 1 - sin2     ;
    bool up = m_bernstein.setPar ( i , psin2 * cos2 * np ) ;
    update  = up || update ;
    psin2  *= sin2 ;
  }
  //
  const bool up = m_bernstein.setPar ( npars() , psin2 * np ) ;
  update  = up || update ;
  //
  return update ;
}
// ============================================================================




// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive2D::Positive2D
( const unsigned short      nX   ,
  const unsigned short      nY   ,
  const double              xmin ,
  const double              xmax ,
  const double              ymin ,
  const double              ymax )
  : std::binary_function<double,double,double> ()
//
  , m_bernstein (   nX , nY , xmin , xmax , ymin , ymax ) 
  , m_phases    ( ( nX + 1 ) * ( nY + 1 ) - 1 , 0 )
  , m_phi0      ( ( nX + 1 ) * ( nY + 1 ) - 1 , 0 )
  , m_sin2      ( ( nX + 1 ) * ( nY + 1 ) - 1 , 0 )
{
  //
  _phi0_ ( m_phi0 ) ;
  //
  for ( unsigned short i = 0 ; i < m_sin2.size() ; ++i )
  {
    const double s = std::sin ( m_phi0[i] ) ;
    m_sin2 [ i ] = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive2D::setPar 
( const unsigned int k     , 
  const double       value )
{
  //
  if (  k >= m_phases.size() )         { return false ; } // FALSE
  //
  if ( s_equal ( value , par ( k ) ) ) { return false ; }
  //
  const double s  = std::sin ( value + m_phi0[k] ) ;
  const double s2 =  s * s ;
  //
  if ( s_equal ( s2 , m_sin2 [ k ] ) ) { return false ; }
  //
  m_phases [ k ] = value ;
  m_sin2   [ k ] = s2    ;
  //
  return updateBernstein ( k ) ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive2D::updateBernstein ( const unsigned int k )
{
  //
  double psin2 = 1 ;
  for ( unsigned int i = 0 ; i < k ; ++ i ) { psin2 *= m_sin2[i] ; }
  //
  bool update = false ;
  const std::size_t np = ( m_bernstein.nX () + 1 ) * ( m_bernstein.nY () + 1 ) ;
  for ( unsigned int i = k ; i < npars() ; ++i )
  {
    const double sin2 = m_sin2   [i] ;
    const double cos2 = 1 - sin2     ;
    bool up = m_bernstein.setPar ( i , psin2 * cos2 * np ) ;
    update  = up || update ;
    psin2  *= sin2 ;
  }
  //
  const bool up = m_bernstein.setPar ( npars() , psin2 * np ) ;
  update  = up || update ;
  //
  return update ;
}
// ============================================================================

// ============================================================================
// constructor from the order
// ============================================================================
Gaudi::Math::Positive2DSym::Positive2DSym
( const unsigned short      N    ,
  const double              xmin ,
  const double              xmax )
  : std::binary_function<double,double,double> ()
//
  , m_bernstein (   N , xmin , xmax ) 
  , m_phases    ( ( N + 1 ) * ( N + 1 ) - 1  , 0 )
  , m_phi0      ( ( N + 1 ) * ( N + 1 ) - 1  , 0 )
  , m_sin2      ( ( N + 1 ) * ( N + 1 ) - 1  , 0 )
{
  //
  _phi0_ ( m_phi0 ) ;
  //
  for ( unsigned short i = 0 ; i < m_sin2.size() ; ++i )
  {
    const double s = std::sin ( m_phi0[i] ) ;
    m_sin2 [ i ] = s * s ;
  }
  //
  updateBernstein () ;
}
// ============================================================================
// set k-parameter
// ============================================================================
bool Gaudi::Math::Positive2DSym::setPar 
( const unsigned int k     , 
  const double       value )
{
  //
  if (  k >= m_phases.size() )         { return false ; } // FALSE
  //
  if ( s_equal ( value , par ( k ) ) ) { return false ; }
  //
  const double s  = std::sin ( value + m_phi0[k] ) ;
  const double s2 =  s * s ;
  //
  if ( s_equal ( s2 , m_sin2 [ k ] ) ) { return false ; }
  //
  m_phases [ k ] = value ;
  m_sin2   [ k ] = s2    ;
  //
  return updateBernstein ( k ) ;
}
// =============================================================================
// update bernstein coefficients
// =============================================================================
bool Gaudi::Math::Positive2DSym::updateBernstein ( const unsigned int k )
{
  //
  double psin2 = 1 ;
  for ( unsigned int i = 0 ; i < k ; ++ i ) { psin2 *= m_sin2[i] ; }
  //
  bool update = false ;
  const std::size_t np = ( m_bernstein.nX () + 1 ) * ( m_bernstein.nY () + 1 ) ;
  for ( unsigned int i = k ; i < npars() ; ++i )
  {
    const double sin2 = m_sin2   [i] ;
    const double cos2 = 1 - sin2     ;
    bool up = m_bernstein.setPar ( i , psin2 * cos2 * np ) ;
    update  = up || update ;
    psin2  *= sin2 ;
  }
  //
  const bool up = m_bernstein.setPar ( npars() , psin2 * np ) ;
  update  = up || update ;
  //
  return update ;
}
// ============================================================================







// ============================================================================
// Student-T 
// ============================================================================
/*  constructor from mass, resolution and "n"-parameter 
 *  @param M     mass 
 *  @param sigma width parameter
 *  @param N     n-parameter  ( actually  n=1+|N| ) 
 */
// ============================================================================
Gaudi::Math::StudentT::StudentT 
( const double mass  , 
  const double sigma ,
  const double n     ) 
  : std::unary_function<double,double>  ()
//
  , m_M    (      std::abs ( mass  ) )
  , m_s    (      std::abs ( sigma ) )
  , m_n    ( -1 )
  , m_norm ( -1 ) 
{
  setN ( n ) ;  
}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::StudentT::~StudentT (){}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::StudentT::setM ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_M ) ) { return false ; }
  //
  m_M = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::StudentT::setSigma ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  if ( s_equal ( v , m_s ) ) { return false ; }
  //
  m_s = v ;
  //
  return true ;
}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::StudentT::setN ( const double x )
{
  //
  const double v = 1 + std::abs ( x ) ;
  //
  if ( m_norm < 0 ) 
  {
    m_norm  = gsl_sf_gamma ( 0.5 * ( v + 1 ) ) / gsl_sf_gamma ( 0.5 * v ) ;  
    m_norm /= std::sqrt    ( M_PI * v ) ;
  }
  //
  if ( s_equal ( v , m_n ) ) { return false ; }
  //
  m_n = v ;
  //
  m_norm  = gsl_sf_gamma ( 0.5 * ( v + 1 ) ) / gsl_sf_gamma ( 0.5 * v ) ;  
  m_norm /= std::sqrt    ( M_PI * v ) ;
  //
  return true ;
}
// ==========================================================================
double Gaudi::Math::StudentT::operator () ( const double x ) const
{
  //
  const double y = ( x - M () ) / sigma() ;
  //
  const double f = std::pow (  1 + y * y / n() ,  -0.5 * ( n() + 1 ) ) ;
  //
  return m_norm * f / sigma () ; // sigma comes from dx = dy * sigma 
}
// ============================================================================
// get the integral 
// ============================================================================
double Gaudi::Math::StudentT::integral() const { return 1 ; }
// ============================================================================
// get the integral 
// ============================================================================
double Gaudi::Math::StudentT::integral
( const double low  , 
  const double high ) const 
{
  //
  if ( s_equal ( low , high ) ) { return                 0.0 ; } // RETURN
  if (           low > high   ) { return - integral ( high ,
                                                      low  ) ; } // RETURN
  //
  // split large pieces 
  if ( high - low > 8 * sigma() ) 
  {
    const double split = 0.5 * ( low + high ) ;
    return integral ( low , split ) + integral ( split , high ) ;  // RETURN
  }
  //
  // use GSL to evaluate the integral
  //
  Sentry sentry ;
  //
  gsl_function F                   ;
  F.function             = &studentT_GSL ;
  const StudentT* _ps    = this  ;
  F.params               = const_cast<StudentT*> ( _ps ) ;
  //
  double result   = 1.0 ;
  double error    = 1.0 ;
  //
  const int ierror = gsl_integration_qag
    ( &F                ,            // the function
      low   , high      ,            // low & high edges
      s_PRECISION       ,            // absolute precision
      s_PRECISION       ,            // relative precision
      s_SIZE            ,            // size of workspace
      GSL_INTEG_GAUSS31 ,            // integration rule
      workspace ( m_workspace ) ,    // workspace
      &result           ,            // the result
      &error            ) ;          // the error in result
  //
  if ( ierror )
  {
    gsl_error ( "Gaudi::Math::StudentT::QAG" ,
                __FILE__ , __LINE__ , ierror ) ;
  }
  //
  return result ;
}
// ============================================================================


// ============================================================================
/* constructor form scale & shape parameters
 *  param k      \f$k\f$ parameter (shape)
 *  param theta  \f$\theta\f$ parameter (scale)
 */
// ============================================================================
Gaudi::Math::GammaDist::GammaDist 
( const double k     ,   // shape parameter  
  const double theta )   // scale parameter
  : std::unary_function<double,double> () 
  , m_k     ( std::abs ( k     ) )
  , m_theta ( std::abs ( theta ) ) 
{}
// ============================================================================
// destrructor
// ============================================================================
Gaudi::Math::GammaDist::~GammaDist (){}
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::GammaDist::setK ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_k ) ) { return false ; }
  //
  m_k = v ;
  //
  // evaluate auxillary parameter 
  m_aux =  0 ;
  if ( s_equal ( 1 , m_k ) ) { m_k    = 1 ; }
  else                       { m_aux += -gsl_sf_lngamma ( m_k ) ; }
  m_aux += - m_k * my_log ( m_theta ) ;
  //
  return true ;
}
// ============================================================================
double Gaudi::Math::GammaDist::sigma    () const
{ return std::sqrt ( dispersion ()  ) ; }
// ============================================================================
double Gaudi::Math::GammaDist::skewness () const
{ return 2.0 / std::sqrt ( m_k )      ; }
// ============================================================================
// set the proper parameters
// ============================================================================
bool Gaudi::Math::GammaDist::setTheta ( const double x )
{
  //
  const double v = std::abs ( x ) ;
  //
  if ( s_equal ( v , m_theta ) ) { return false ; }
  //
  m_theta = v ;
  //
  // evaluate auxillary parameter 
  m_aux =  0 ;
  if ( s_equal ( 1 , m_k ) ) {}
  else                       { m_aux += -gsl_sf_lngamma ( m_k ) ; }
  m_aux += - m_k * my_log ( m_theta ) ;
  //
  //
  return true ;
}
// ============================================================================
// calculate gamma distribution shape
// ============================================================================
double Gaudi::Math::GammaDist::pdf ( const double x ) const
{
  // simple cases 
  if ( x < 0 ) { return 0 ; }
  // 
  if ( s_equal ( x , 0 )    ) { return s_equal ( m_k , 1 ) ? 1/m_theta : 0.0 ; }
  //
  double result = m_aux - x / m_theta ;
  if ( !s_equal ( m_k , 1 ) ) { result += ( m_k - 1 ) * my_log ( x ) ; }
  //
  return my_exp ( result ) ;
}
// ============================================================================
// get the integral
// ============================================================================
double Gaudi::Math::GammaDist::integral () const { return 1 ; }
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double Gaudi::Math::GammaDist::integral ( const double low  ,
                                          const double high ) const 
{
  //
  if      ( s_equal ( low  , high ) ) { return 0 ; }
  else if (           low  > high   ) { return -1 * integral ( high , low  ) ; }
  else if (           high <= 0     ) { return 0 ; }
  else if (           low  < 0      ) { return      integral ( 0    , high ) ; }
  //
  return 
    gsl_sf_gamma_inc_P ( m_k , high / m_theta ) - 
    gsl_sf_gamma_inc_P ( m_k , low  / m_theta ) ;
}
// ============================================================================


// ============================================================================
// Generalaize Gamma distribtions 
// ============================================================================
namespace 
{
  // ==========================================================================
  double _calc_aux_ ( const double k     , 
                      const double theta , 
                      const double p     )
  {
    if ( k > 0.2 * GSL_SF_GAMMA_XMAX * p 
         || theta < 1.e-6 
         || p     > 1.e+6 ) 
    {
      double aux  = gsl_sf_log     ( p     ) ;
      aux        -= gsl_sf_log     ( theta ) ;
      aux        -= gsl_sf_lngamma ( k / p ) ;
      return        gsl_sf_exp     ( aux   ) ;
    }
    return p / theta / gsl_sf_gamma ( k / p ) ;
  }
  // ==========================================================================
}
// ============================================================================
/*  constructor
 *  param k     \f$k\f$ parameter      (shape)
 *  param theta \f$\theta\f$ parameter (scale)
 *  param p     \f$p\f$ parameter 
 *  param low   bias       
 */
// ============================================================================
Gaudi::Math::GenGammaDist::GenGammaDist
( const double k     , 
  const double theta , 
  const double p     , // 1 corresponds to gamma distribution 
  const double low   ) 
  : std::unary_function<double,double>() 
  , m_k     ( std::abs ( k     ) ) 
  , m_theta ( std::abs ( theta ) ) 
  , m_p     ( std::abs ( p     ) ) 
  , m_low   ( low ) 
  , m_aux   ( 0   ) 
{
  m_aux = _calc_aux_ ( m_k  , m_theta , m_p ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::GenGammaDist::~GenGammaDist(){}
// ============================================================================
bool Gaudi::Math::GenGammaDist::setK     ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_k ) ) { return false ; }
  m_k   = value_ ;
  m_aux = _calc_aux_ ( k () , theta() , p() ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGammaDist::setTheta ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_theta ) ) { return false ; }
  m_theta = value_ ;
  m_aux   = _calc_aux_ ( k () , theta() , p() ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGammaDist::setP    ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_p ) ) { return false ; }
  m_p     = value_ ;
  m_aux   = _calc_aux_ ( k () , theta() , p () ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::GenGammaDist::setLow ( const double value ) 
{
  if ( s_equal ( value , m_low ) ) { return false ; }
  m_low   = value ;
  return true ;
}
// ============================================================================
double Gaudi::Math::GenGammaDist::pdf ( const double x ) const 
{
  //
  if ( x <= m_low || s_equal ( x , m_low ) ) { return 0 ; }
  //
  const double xc = ( x - m_low ) / theta() ;  
  const double xt = std::pow ( xc , p() ) ;
  //
  if ( xt > 50 
       || k     () > 0.2 * GSL_SF_GAMMA_XMAX * p() 
       || theta () < 1.e-6 
       || p     () > 1.e+6 ) 
  {
    double result   = ( k () - 1 ) * gsl_sf_log ( xc ) ;
    result         -= xt  ;
    result         += gsl_sf_log     ( p     ()    ) ;
    result         -= gsl_sf_log     ( theta ()    ) ;
    result         -= gsl_sf_lngamma ( k () / p () ) ;
    return gsl_sf_exp ( result ) ;
  }
  //
  double result  = m_aux ;
  result        *= std::pow ( xc , k() - 1 ) ;
  result        *= gsl_sf_exp ( -xt  ) ;
  //
  return result ;
}
// ============================================================================
double Gaudi::Math::GenGammaDist::cdf ( const double x ) const 
{
  //
  if ( x <= m_low || s_equal ( x , m_low ) ) { return 0 ; }
  //
  const double xc = ( x - m_low ) / theta() ;  
  const double xt = std::pow ( xc , p () ) ;
  //
  return gsl_sf_gamma_inc_P ( k () / p () , xt ) ;
}
// ============================================================================
double Gaudi::Math::GenGammaDist::integral () const { return 1 ; }
// ============================================================================
double Gaudi::Math::GenGammaDist::integral ( const double low  , 
                                             const double high ) const 
{
  if ( s_equal ( low , high ) ) { return 0 ; }
  return cdf ( high ) - cdf ( low ) ;  
}
// ============================================================================


// ============================================================================
// Amoroso 
// ============================================================================
namespace 
{
  double _calc_aux_2_ ( const double theta ,
                        const double alpha , 
                        const double beta  )
  {
    if ( alpha >= 0.2 * GSL_SF_GAMMA_XMAX 
         || std::abs ( theta ) < 1.e-6 
         || std::abs ( beta  ) > 1.e+6 ) 
    { 
      double aux  = gsl_sf_log     ( std::abs ( beta  ) ) ;
      aux        -= gsl_sf_log     ( std::abs ( theta ) ) ;
      aux        -= gsl_sf_lngamma ( alpha ) ;
      return        gsl_sf_exp     ( aux   ) ;
    }
    return std::abs ( beta / theta ) / gsl_sf_gamma ( alpha ) ;
  }  
}

// ==========================================================================
/*  constructor
 *  param a     a-parameter 
 *  param theta \f$\theta\f$-parameter  
 *  param alpha \f$\alpha\f$-parameter (>0)
 *  param beta  \f$\beta\f$-parameter 
 *  Note that   \f$\alpha\beta\f$ is equal to k-parameter 
 */
// ============================================================================
Gaudi::Math::Amoroso::Amoroso 
( const double theta , 
  const double alpha , 
  const double beta  ,
  const double a     ) 
  : std::unary_function<double,double>() 
  , m_a     (            a       ) 
  , m_theta ( theta              ) 
  , m_alpha ( std::abs ( alpha ) ) 
  , m_beta  (            beta    ) 
  , m_aux   ( 0 ) 
{
  m_aux = _calc_aux_2_ ( m_theta , m_alpha , m_beta ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Amoroso::~Amoroso(){}
// ============================================================================
bool Gaudi::Math::Amoroso::setA      ( const double value ) 
{
  if ( s_equal ( value , m_a ) ) { return false ; }
  m_a = value ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::Amoroso::setTheta ( const double value ) 
{
  if ( s_equal ( value , m_theta ) ) { return false ; }
  m_theta = value ;
  m_aux   = _calc_aux_2_ ( m_theta , m_alpha , m_beta ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::Amoroso::setAlpha ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_alpha ) ) { return false ; }
  m_alpha = value_ ;
  m_aux   = _calc_aux_2_ ( m_theta , m_alpha , m_beta ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::Amoroso::setBeta ( const double value ) 
{
  if ( s_equal ( value , m_beta ) ) { return false ; }
  m_beta  = value ;
  m_aux   = _calc_aux_2_ ( m_theta , m_alpha , m_beta ) ;
  return true ;
}
// ============================================================================
// evaluate Amoroso distribtion
// ============================================================================
double Gaudi::Math::Amoroso::pdf ( const double x ) const 
{
  //
  if      ( theta () > 0 && ( x <= m_a || s_equal ( x , m_a ) ) ) { return 0 ; }
  else if ( theta () < 0 && ( x >= m_a || s_equal ( x , m_a ) ) ) { return 0 ; }
  //
  const double xc = ( x - m_a ) / theta ()    ;
  const double xt = std::pow ( xc , beta() ) ;
  //
  if ( xt > 50 
       || alpha () >= 0.2 * GSL_SF_GAMMA_XMAX 
       || std::abs ( theta () ) < 1.e-6 
       || std::abs ( beta  () ) > 1.e+6 ) 
  {
    //
    double result   = ( alpha() * beta() - 1 ) * xc ;
    result += gsl_sf_log     ( std::abs ( beta  () ) ) ;
    result -= gsl_sf_log     ( std::abs ( theta () ) ) ;
    result -= gsl_sf_lngamma (            alpha ()   ) ;
    //
    return gsl_sf_exp ( result ) ;
  }
  //
  double result  = m_aux ;
  result        *= std::pow   ( xc , alpha() * beta() - 1 ) ;
  result        *= gsl_sf_exp ( -xt  ) ;
  //
  return result ;
}
// ============================================================================
double Gaudi::Math::Amoroso::cdf ( const double x ) const 
{
  //
  if      ( theta () > 0 && ( x <= m_a || s_equal ( x , m_a ) ) ) { return 0 ; }
  else if ( theta () < 0 && ( x >= m_a || s_equal ( x , m_a ) ) ) { return 1 ; }
  //
  const double xc = ( x - m_a ) / theta ()    ;
  const double xt = std::pow ( xc , beta() ) ;
  //
  return 
    beta() * theta() > 0 ? 
    1 - gsl_sf_gamma_inc_Q ( alpha() , xt ) :
    gsl_sf_gamma_inc_Q ( alpha() , xt ) ;
}
// ============================================================================
double Gaudi::Math::Amoroso::integral () const { return 1 ; }
// ============================================================================
double Gaudi::Math::Amoroso::integral ( const double low  , 
                                        const double high ) const 
{
  if ( s_equal ( low ,high ) ) { return 0 ; }
  return  cdf ( high ) - cdf ( low ) ;
}
// ============================================================================
double Gaudi::Math::Amoroso::mode () const 
{
  if ( alpha() * beta() <= 1 ) { return a () ; }
  return a () + theta() * std::pow ( alpha() - 1./beta() , 1./beta () ) ;
}
// ============================================================================
double Gaudi::Math::Amoroso::mean () const 
{
  const double x = alpha() + 1/beta() ;
  if ( x <= 0 || s_equal ( x , 0 )  ) { return -1.e+9 ; } // RETURN 
  //
  if ( x       < 0.2 * GSL_SF_GAMMA_XMAX && 
       alpha() < 0.2 * GSL_SF_GAMMA_XMAX  ) 
  {
    return a () + theta() * gsl_sf_gamma ( x ) / gsl_sf_gamma ( alpha() ) ;
  }
  //
  double aux = gsl_sf_lngamma ( x       ) ;
  aux -= gsl_sf_lngamma       ( alpha() ) ;
  //
  return a() + theta() * gsl_sf_exp ( aux ) ;
}
// ============================================================================
double Gaudi::Math::Amoroso::variance () const 
{
  //
  const double x2 = alpha() + 2/beta() ;
  if ( x2 <= 0 || s_equal ( x2 , 0 )  ) { return -1.e+9 ; } // RETURN 
  //
  const double x1 = alpha() + 1/beta() ;
  if ( x1 <= 0 || s_equal ( x1 , 0 )  ) { return -1.e+9 ; } // RETURN 
  //
  //
  if ( x1      < 0.2 * GSL_SF_GAMMA_XMAX && 
       x2      < 0.2 * GSL_SF_GAMMA_XMAX && 
       alpha() < 0.2 * GSL_SF_GAMMA_XMAX  ) 
  {
    const double ga  = gsl_sf_gamma ( alpha () ) ;
    const double gx1 = gsl_sf_gamma ( x1       ) ;
    const double gx2 = gsl_sf_gamma ( x2       ) ;
    //
    return theta2() * ( gx2 / ga - Gaudi::Math::pow ( gx1 / ga , 2 ) ) ;
  }
  //
  const double lnga = gsl_sf_lngamma ( alpha () ) ;
  //
  double aux1  = gsl_sf_lngamma ( x1   ) ;
  aux1        -= lnga ;
  aux1         = gsl_sf_exp     ( aux1 ) ;
  //
  double aux2  = gsl_sf_lngamma ( x2   ) ;
  aux2        -= lnga ;
  aux2         = gsl_sf_exp     ( aux2 ) ;
  //
  return theta2() * ( aux2 - aux1 * aux1 ) ;
}
// ============================================================================
double Gaudi::Math::Amoroso::sigma () const 
{
  //
  const double x2 = alpha() + 2/beta() ;
  if ( x2 <= 0 || s_equal ( x2 , 0 )  ) { return -1.e+9 ; } // RETURN 
  //
  const double x1 = alpha() + 1/beta() ;
  if ( x1 <= 0 || s_equal ( x1 , 0 )  ) { return -1.e+9 ; } // RETURN 
  //
  return std::sqrt ( variance() ) ;
}
// ============================================================================
/* constructor from scale & shape parameters
 *  param k      \f$k\f$ parameter (shape)
 *  param theta  \f$\theta\f$ parameter (scale)
 */
// ============================================================================
Gaudi::Math::LogGammaDist::LogGammaDist 
( const double k     ,   // shape parameter  
  const double theta )   // scale parameter
  : m_gamma ( k , theta ) 
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::LogGammaDist::~LogGammaDist (){}
// ============================================================================
// calculate log-gamma distribution shape
// ============================================================================
double Gaudi::Math::LogGammaDist::operator() ( const double x ) const
{
  // 
  const double z = my_exp ( x ) ;
  return m_gamma ( z ) * z ;
  //
}
// ============================================================================
// get the integral
// ============================================================================
double Gaudi::Math::LogGammaDist::integral () const { return 1 ; }
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double Gaudi::Math::LogGammaDist::integral ( const double low  ,
                                             const double high ) const 
{
  //
  if      ( s_equal ( low  , high ) ) { return 0 ; }
  else if (           low  > high   ) { return -1 * integral ( high , low  ) ; }
  //
  const double z_low  = my_exp ( low  ) ;
  const double z_high = my_exp ( high ) ;
  //
  return m_gamma.integral ( z_low , z_high ) ;
}
// ============================================================================
/* constructor form scale & shape parameters
 *  param k      \f$k\f$ parameter (shape)
 *  param theta  \f$\theta\f$ parameter (scale)
 */
// ============================================================================
Gaudi::Math::Log10GammaDist::Log10GammaDist 
( const double k     ,   // shape parameter  
  const double theta )   // scale parameter
  : Gaudi::Math::LogGammaDist( k , theta ) 
{}
// ============================================================================
// destructor
// ============================================================================
Gaudi::Math::Log10GammaDist::~Log10GammaDist (){}
// ============================================================================
// calculate log-gamma distribution shape
// ============================================================================
double Gaudi::Math::Log10GammaDist::operator() ( const double x ) const
{ return LogGammaDist::operator() ( x * s_LN10 ) * s_LN10 ; }
// ============================================================================
// get the integral 
// ============================================================================
double Gaudi::Math::Log10GammaDist::integral () const { return 1 ; }
// ============================================================================
// get the integral between low and high limits
// ============================================================================
double Gaudi::Math::Log10GammaDist::integral ( const double low  ,
                                               const double high ) const 
{ return LogGammaDist::integral ( low  * s_LN10 , high * s_LN10 ) ; }
// ============================================================================



// ============================================================================
// Log-Gamma
// ============================================================================
namespace 
{
  // ==========================================================================
  double _calc_aux_3_ ( const double alpha  , 
                        const double lambda )
  {
    if ( alpha >= 0.2 * GSL_SF_GAMMA_XMAX 
         || std::abs ( lambda ) < 1.e-6  )
    { 
      double aux  = 0 ;
      aux        -= gsl_sf_log     ( std::abs ( lambda ) ) ;
      aux        -= gsl_sf_lngamma ( alpha ) ;
      return        gsl_sf_exp     ( aux   ) ;
    }
    return 1 / ( gsl_sf_gamma ( alpha ) * std::abs ( lambda ) ) ;
  }  
  // ==========================================================================
}
// ============================================================================
/*  constructor from scale & shape parameters
 *  param nu      \f$\nu\f$ parameter      (location)
 *  param lambda  \f$\lambda\f$ parameter  
 *  param alpha   \f$\alpha\f$ parameter    (>0)
 */
// ============================================================================
Gaudi::Math::LogGamma::LogGamma
( const double nu     , 
  const double lambda , 
  const double alpha  ) 
  : std::unary_function<double,double>() 
  , m_nu     ( nu     ) 
  , m_lambda ( lambda ) 
  , m_alpha  ( std::abs ( alpha ) ) 
  , m_aux    ( 0 ) 
{
  m_aux = _calc_aux_3_ ( m_alpha , m_lambda ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::LogGamma::~LogGamma(){}
// ============================================================================
bool Gaudi::Math::LogGamma::setNu   ( const double value ) 
{
  if ( s_equal ( value , m_nu ) ) { return false ; }
  m_nu  = value ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::LogGamma::setLambda ( const double value ) 
{
  if ( s_equal ( value , m_lambda ) ) { return false ; }
  m_lambda = value ;
  m_aux    = _calc_aux_3_ ( m_alpha , m_lambda ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::LogGamma::setAlpha ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_alpha ) ) { return false ; }
  m_alpha = value_ ;
  m_aux   = _calc_aux_3_ ( m_alpha , m_lambda ) ;
  return true ;
}
// ============================================================================
// calculate log-gamma shape
// ============================================================================
double Gaudi::Math::LogGamma::pdf ( const double x ) const 
{
  //
  const double xc  = x  -  nu    () ;
  const double xt  = xc / lambda () ;
  //
  const double arg = alpha() * xt - gsl_sf_exp ( xt ) ;
  //
  if ( std::abs ( arg ) > 50 
       || alpha() > 0.2 * GSL_SF_GAMMA_XMAX 
       || std::abs ( lambda() ) < 1.e+6      ) 
  {
    //
    double result  = arg ;
    result        -= gsl_sf_log      ( std::abs ( lambda () ) ) ;
    result        -= gsl_sf_lngamma  (            alpha  ()   ) ;
    //
    return gsl_sf_exp ( result ) ;
  }
  //
  return m_aux * gsl_sf_exp ( arg ) ;
}
// ============================================================================
double Gaudi::Math::LogGamma::cdf ( const double x ) const 
{
  //
  const double xc  = x  -  nu    () ;
  const double xt  = xc / lambda () ;
  //
  const double ext = gsl_sf_exp ( xt ) ;
  //
  return 
    lambda () > 0 ? 
    1 - gsl_sf_gamma_inc_Q ( alpha() , ext ) : 
    gsl_sf_gamma_inc_Q ( alpha() , ext ) ;
}
// ============================================================================
double Gaudi::Math::LogGamma::integral ( const double low  , 
                                         const double high ) const
{
  if ( s_equal ( low , high ) ) { return 0 ; }
  return cdf ( high ) - cdf ( low ) ;
}
// ============================================================================
double Gaudi::Math::LogGamma::integral () const { return 1 ; }
// ============================================================================
double Gaudi::Math::LogGamma::mode     () const 
{ return nu() - lambda() * gsl_sf_log ( alpha () ) ; }
// ============================================================================
double Gaudi::Math::LogGamma::mean     () const 
{ return nu() + lambda() * gsl_sf_psi ( alpha () ) ; }
// ============================================================================
double Gaudi::Math::LogGamma::sigma    () const 
{ return std::sqrt ( variance () ) ; }
// ============================================================================
double Gaudi::Math::LogGamma::variance () const 
{ return lambda() * lambda() * gsl_sf_psi_1 ( alpha () ) ; }
// ============================================================================
double Gaudi::Math::LogGamma::skewness () const 
{ 
  const double l_p2 = gsl_sf_psi_n ( 2 , alpha () ) ; 
  const double l_p1 = gsl_sf_psi_1 (     alpha () ) ; 
  return 
    lambda() > 0 ?
    l_p2 / std::pow ( l_p1 , 1.5 ) : -1 * l_p2 / std::pow ( l_p1 , 1.5 ) ;
}
// ============================================================================
double Gaudi::Math::LogGamma::kurtosis () const 
{ 
  const double l_p3 = gsl_sf_psi_n ( 3 , alpha () ) ; 
  const double l_p1 = gsl_sf_psi_1 (     alpha () ) ; 
  return l_p3 / ( l_p1 * l_p1) ;
}
// ============================================================================



// ============================================================================
// Beta' 
// ============================================================================
/*  constructor with all parameters 
 *  @param alpha \f$\alpha\f$-parameter 
 *  @param beta  \f$\beta\f$-parameter 
 */
// ============================================================================
Gaudi::Math::BetaPrime::BetaPrime 
( const double alpha , 
  const double beta  )
  : std::unary_function<double,double> () 
  , m_alpha ( std::abs ( alpha ) )
  , m_beta  ( std::abs ( beta  ) )
  , m_aux () 
{
  m_aux = gsl_sf_beta ( m_alpha , m_beta ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::BetaPrime::~BetaPrime (){}
// ============================================================================
bool Gaudi::Math::BetaPrime::setAlpha ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_alpha ) ) { return false ; }
  m_alpha = value_ ;
  m_aux   = gsl_sf_beta ( m_alpha , m_beta ) ;
  return true ;
}
// ============================================================================
bool Gaudi::Math::BetaPrime::setBeta  ( const double value ) 
{
  const double value_ = std::abs ( value ) ;
  if ( s_equal ( value_ , m_beta  ) ) { return false ; }
  m_beta  = value_ ;
  m_aux   = gsl_sf_beta ( m_alpha , m_beta ) ;
  return true ;
}
// ============================================================================
// evaluate beta'-distributions 
// ============================================================================
double Gaudi::Math::BetaPrime::pdf ( const double x ) const 
{
  //
  if ( x <= 0 || s_equal ( x , 0 ) ) { return 0 ; }
  //
  return m_aux 
    * std::pow (     x ,   alpha () - 1       ) 
    * std::pow ( 1 + x , - alpha () - beta () ) ;  
}
// ============================================================================
double Gaudi::Math::BetaPrime::cdf ( const double x ) const 
{
  //
  if ( x <= 0 || s_equal ( x , 0 ) ) { return 0 ; }
  //
  const double y = x / ( 1 + x ) ;
  //
  return gsl_sf_beta_inc (  alpha() , beta() , y ) ;
}
// ============================================================================
double Gaudi::Math::BetaPrime::integral ( const double low  , 
                                          const double high ) const 
{
  //
  if ( s_equal ( low , high ) ) { return 0 ; }
  //
  return cdf ( high ) - cdf ( low ) ;
}
// ============================================================================
double Gaudi::Math::BetaPrime::integral () const { return 1 ; }
// ============================================================================
double Gaudi::Math::BetaPrime::mean () const 
{
  if ( beta() <= 1 || s_equal ( beta() , 1 ) ) { return -1.e+9 ; }  
  //
  return alpha() / ( beta() - 1 ) ;
}
// ============================================================================
double Gaudi::Math::BetaPrime::mode () const 
{
  if ( alpha() < 1 ) { return 0 ; }
  return ( alpha() - 1 ) / ( beta() + 1 ) ;
}
// ============================================================================
double Gaudi::Math::BetaPrime::variance () const 
{
  if ( beta() <= 2 || s_equal ( beta() , 2 ) ) { return -1.e+9 ; }  
  //
  const double a = alpha () ;
  const double b = beta  () ;
  //
  return a *  ( a + b + 1 ) / ( b - 2 ) / Gaudi::Math::pow ( b - 1 , 2 ) ;
}
// ===========================================================================
double Gaudi::Math::BetaPrime::sigma () const 
{
  if ( beta() <= 2 || s_equal ( beta() , 2 ) ) { return -1.e+9 ; }  
  return std::sqrt ( variance () ) ;
}
// ===========================================================================
double Gaudi::Math::BetaPrime::skewness  () const 
{
  if ( beta() <= 3 || s_equal ( beta() , 3 ) ) { return -1.e+9 ; }  
  //
  const double a = alpha () ;
  const double b = beta  () ;
  //
  return 2 * ( 2 * a + b - 1 ) / ( b - 3 ) * std::sqrt( ( b - 2 ) / a / ( a + b - 1 ) ) ;
}
// ===========================================================================



// ============================================================================
// The END
// ============================================================================


