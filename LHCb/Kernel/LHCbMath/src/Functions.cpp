// $Id$ 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <climits>
#include <iostream>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Functions.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_errno.h"
#include "gsl/gsl_sf_exp.h"
#include "gsl/gsl_sf_log.h"
#include "gsl/gsl_sf_erf.h"
#include "gsl/gsl_integration.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
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
namespace 
{
  // ==========================================================================
  // GSL 
  // ==========================================================================
  void GSL_local_error
  ( const char * reason    ,
    const char * file      ,
    int          line      ,
    int          gsl_errno ) 
  {
    std::cerr 
      << " GSL_ERROR : "   << gsl_errno << "/'" << gsl_strerror( gsl_errno ) << "'"
      << "\t reason '"     << reason    << "' "
      << "\t file/line '"  << file      << "'/" << line 
      << std::endl ;  
  }
  // ==========================================================================
  class GSL_Handler_Sentry
  {
  public :
    // ========================================================================
    /// constructor: loc
    GSL_Handler_Sentry () 
      : m_old ( 0 ) 
    { 
      // m_old = gsl_set_error_handler ( &GSL_local_error ) ; 
      m_old = gsl_set_error_handler_off () ; 
    }
    //
    ~GSL_Handler_Sentry () { gsl_set_error_handler ( m_old ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the "old" error handler 
    gsl_error_handler_t * m_old ; // the "old" error handler 
    // ========================================================================
  } ;  
  // ==========================================================================
  BOOST_STATIC_ASSERT( std::numeric_limits<float>::is_specialized  ) ;
  // ==========================================================================
  /** @var s_INFINITY 
   *  representation of positive INFINITY 
   */
  const double s_INFINITY = 0.5 * std::numeric_limits<float>::max()  ;
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
   *  trucnkating parameter for CrystalBall-functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-05-23      
   */
  const double      s_TRUNC = 15.0 ;
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
  const double s_SQRT2PI    =       std::sqrt( 2 * M_PI ) ;
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
    GSL_Handler_Sentry sentry ;
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
        // GSL_Handler_Sentry sentry ;
        GSL_local_error ( "Error from erf_e function" ,
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
    GSL_Handler_Sentry sentry ;
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
        // GSL_Handler_Sentry sentry ;
        GSL_local_error ( "Error from exp_e function" ,
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
    if ( 0 >= arg ) { return -s_INFINITY ; } // REUTRN
    //
    GSL_Handler_Sentry sentry ;
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
        // GSL_Handler_Sentry sentry ;
        GSL_local_error ( "Error from exp_e function" ,
                          __FILE__ , __LINE__ , ierror ) ; 
      }
      //
      if      (  1.e-100 > arg  ) { return -s_INFINITY ; }
      else if (  1.e+100 < arg  ) { return  s_INFINITY ; }
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
    const double arg    = kappa * x * x + xi * x ;
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
    if      ( s_equal ( alpha , 0 ) )      // the most trivial trivial case 
    {
      /// the most trivial trivial case 
      if     ( s_equal ( beta , 0 ) ) { return b - a ; }  // RETURN 
      /// get the simple exponential integral
      return ( my_exp ( beta * b ) - my_exp ( beta * a ) ) / beta ;
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
    GSL_Handler_Sentry sentry ;
    //
    gsl_function F            ;
    F.function = &gauss_GSL   ;
    double params[2]          ;
    params[0]  = -alpha       ;    // ATTENTION: minus sign here!
    params[1]  =  beta        ;
    F.params   = params       ;
    //
    gsl_integration_workspace* ws =
      gsl_integration_workspace_alloc ( s_SIZE ) ; 
    //
    double result   = 1.0 ;
    double error    = 1.0 ;
    //
    const int ierror = gsl_integration_qag 
      ( &F                ,            // the function 
        a     ,   b       ,            // low & high edges 
        s_PRECISION       ,            // absolute precision            
        s_PRECISION       ,            // relative precision 
        s_SIZE            ,            // size of workspace 
        GSL_INTEG_GAUSS31 ,            // integration rule  
        ws                ,            // workspace  
        &result           ,            // the result 
        &error            ) ;          // the error in result 
    //
    gsl_integration_workspace_free ( ws );
    //
    if ( ierror ) 
    { 
      // GSL_Handler_Sentry sentry ;      
      GSL_local_error ( "Gaudi::Math::gaussian_int " ,
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
} //                                                 end of anonymous namespace 
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
// evaluate Hrrmite polynomial 
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
( const double peak  , 
  const double sigma , 
  const double asym  ) 
  : std::unary_function<double,double> () 
  , m_peak    ( peak ) 
  , m_sigma   ( std::fabs ( sigma ) ) 
  , m_asym    ( std::tanh ( asym  ) )
//
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::BifurcatedGauss::~BifurcatedGauss (){}
// ============================================================================
// evaluate Bifurcated Gaussian 
// ============================================================================
double Gaudi::Math::BifurcatedGauss::operator() ( const double x ) const 
{
  const double dx = x - m_peak ;
  //
  const double sigma_L = m_sigma * ( 1 + m_asym ) ;
  const double sigma_R = m_sigma * ( 1 - m_asym ) ;
  //
  const double norm = s_SQRTPIHALF * ( sigma_L + sigma_R ) ;
  //
  return 
    dx < 0 ?
    my_exp ( -0.5 * dx * dx / sigma_L / sigma_L ) / norm :
    my_exp ( -0.5 * dx * dx / sigma_R / sigma_R ) / norm ;
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
  const double sigma_L = m_sigma * ( 1 + m_asym ) ;
  const double sigma_R = m_sigma * ( 1 - m_asym ) ;
  //
  const double norm = s_SQRTPIHALF * ( sigma_L + sigma_R ) ;
  //
  // left half-gaussian 
  if       ( high <= m_peak ) 
  {
    return gaussian_int ( 0.5 / sigma_L / sigma_L , 
                          0                           , 
                          low  - m_peak , 
                          high - m_peak ) / norm ;
    
  }
  // right half-gaussian 
  else if ( low >= m_peak )  
  {
    return gaussian_int ( 0.5 / sigma_R / sigma_R , 
                          0                           , 
                          low  - m_peak , 
                          high - m_peak ) / norm ;
    
  }  
  //
  return
    integral ( low    , m_peak ) + 
    integral ( m_peak , high   ) ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setSigma ( const double value ) 
{ 
  const double value_ = std::fabs ( value ) ; 
  if ( s_equal ( m_sigma , value_ ) ) { return false ; }
  m_sigma = value_ ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::BifurcatedGauss::setAsym ( const double value ) 
{
  //
  const double value_ = std::tanh ( value ) ;
  if ( s_equal ( m_asym , value_ ) ) { return false ; }
  m_asym = value_ ;
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
  , m_workspace ( 0     ) // NB! 
{
  //
  setXi    ( xi    ) ; // must be the first 
  //
  setPeak  ( peak  ) ;
  //
  setSigma ( sigma ) ;
  //
  setRho_L ( rho_L ) ;
  //
  setRho_R ( rho_R ) ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Gaudi::Math::Bukin::Bukin 
( const Gaudi::Math::Bukin& right ) 
  : std::unary_function<double,double> ( right )
//
  , m_peak      ( right.m_peak     )  
  , m_sigma     ( right.m_sigma    ) 
  , m_xi        ( right.m_xi       )  
  , m_rho_L     ( right.m_rho_L    ) 
  , m_rho_R     ( right.m_rho_R    ) 
//
  , m_x1        ( right.m_x1   ) 
  , m_x2        ( right.m_x2   ) 
  , m_A         ( right.m_A    ) 
  , m_B2        ( right.m_B2   ) 
  , m_L         ( right.m_L    ) 
  , m_R         ( right.m_R    )
//
  , m_integral  ( right.m_integral ) 
//
  , m_workspace ( 0     ) // NB! 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Bukin::~Bukin ()
{
  //
  if ( 0 != m_workspace ) 
  {
    gsl_integration_workspace * _ws = (gsl_integration_workspace*) m_workspace ;
    m_workspace = 0 ;
    gsl_integration_workspace_free ( _ws );
  }
}
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
bool Gaudi::Math::Bukin::setRho_L  ( const double value ) 
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
bool Gaudi::Math::Bukin::setRho_R  ( const double value ) 
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
double Gaudi::Math::Bukin::operator() ( const double x ) const 
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
  GSL_Handler_Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &bukin_GSL ;
  F.params   = const_cast<Bukin*> ( this ) ;
  //
  gsl_integration_workspace* ws = 0 ;
  if ( 0 == m_workspace )  
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  ws = (gsl_integration_workspace*) m_workspace ;
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
      ws                ,            // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    //
    // GSL_Handler_Sentry sentry ;
    GSL_local_error ( "Gaudi::Math::Bukin::QAG" ,
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
  , m_workspace ( 0     ) 
{
  //
  m_lambda = x_sinh ( m_tau * s_Novosibirsk ) ;
  //
}
// ============================================================================
// copy constructor 
// ============================================================================
Gaudi::Math::Novosibirsk::Novosibirsk
( const Gaudi::Math::Novosibirsk& right ) 
  : std::unary_function<double,double> ( right )
//
  , m_m0        ( right.m_m0       ) 
  , m_sigma     ( right.m_sigma    ) 
  , m_tau       ( right.m_tau      ) 
//
  , m_lambda    ( right.m_lambda   ) 
//
  , m_integral  ( right.m_integral ) 
  , m_workspace ( 0                ) 
{}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::Novosibirsk::~Novosibirsk()
{
  if ( 0 != m_workspace ) 
  {
    gsl_integration_workspace * _ws = (gsl_integration_workspace*) m_workspace ;
    m_workspace = 0 ;
    gsl_integration_workspace_free ( _ws );
  }
}
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
double Gaudi::Math::Novosibirsk::operator() ( const double x ) const 
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
  //
  // use GSL to evaluate the integral 
  //
  GSL_Handler_Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  gsl_integration_workspace* ws = 0 ;
  if ( 0 == m_workspace )  
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  ws = (gsl_integration_workspace*) m_workspace ;
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
      ws                ,            // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    GSL_Handler_Sentry sentry ;
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
  GSL_Handler_Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &novosibirsk_GSL ;
  F.params   = const_cast<Novosibirsk*> ( this ) ;
  //
  gsl_integration_workspace* ws = 0 ;
  if ( 0 == m_workspace )  
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  ws = (gsl_integration_workspace*) m_workspace ;
  //
  // left tail:
  //
  double tail_l   =  0.0 ;
  double error_l  = -1.0 ;
  //
  const int ierror_l = gsl_integration_qagil
    ( &F                ,   // the function 
      x_low             ,   // "high" edge
      s_PRECISION       ,   // absolute precision  
      s_PRECISION_TAIL  ,   // relative precision 
      s_SIZE            ,   // size of workspace 
      ws                ,   // workspace  
      &tail_l           ,   // the result 
      &error_l          ) ; // the error in result 
  //
  if ( ierror_l ) 
  { 
    GSL_Handler_Sentry sentry ;
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
    ( &F                ,   // the function 
      x_high            ,   // "low" edge
      s_PRECISION       ,   // absolute precision  
      s_PRECISION_TAIL  ,   // relative precision 
      s_SIZE            ,   // size of workspace 
      ws                ,   // workspace  
      &tail_r           ,   // the result 
      &error_r          ) ; // the error in result 
  //
  if ( ierror_r ) 
  { 
    GSL_Handler_Sentry sentry ;
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
  const double N     ) 
  : std::unary_function<double,double> () 
  , m_m0       ( m0                  )
  , m_sigma    (     std::fabs ( sigma ) )
  , m_alpha    ( 1 + std::fabs ( alpha ) )
  , m_N        ( 1 + std::fabs ( N     ) )
//
  , m_const    ( 0.0 )  
  , m_integral ( -1000 ) 
{
  m_const = my_exp ( -0.5 * m_alpha * m_alpha ) ;
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::CrystalBall::~CrystalBall (){}
// ============================================================================
bool  Gaudi::Math::CrystalBall::setM0 ( const double value ) 
{
  if ( s_equal ( value , m_m0 ) ) { return false ; } 
  //
  m_m0       = value ;
  m_integral = -1000 ;
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
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setAlpha  ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha ) ) { return false ; } 
  //
  m_alpha    = value_ ;
  m_const    = my_exp ( -0.5 * m_alpha * m_alpha ) ;  
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBall::setN      ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N     ) ) { return false ; } 
  //
  m_N        = value_ ;
  m_integral = -1000  ;
  //
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function 
// ============================================================================
double Gaudi::Math::CrystalBall::operator() ( const double x ) const 
{
  //
  const double dx = ( x - m_m0 ) / m_sigma ;
  //
  // tail 
  //
  if  ( dx < -m_alpha ) 
  {
    const double f1 = m_N / m_alpha ;
    const double f2 = m_N / m_alpha - m_alpha - dx ;
    //
    return m_const * std::pow ( f1 / f2 , m_N ) ;  
  }
  //
  // peak 
  // 
  return my_exp ( -0.5 * dx * dx ) ;
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
  if      ( low < x0 && x0 < high ) 
  { return integral ( low , x0 ) + integral ( x0 , high ) ; }
  //
  // peak 
  //
  if ( x0 <= low  ) 
  {
    return gaussian_int ( 0.5 / m_sigma / m_sigma , 
                          0                       , 
                          low  - m_m0             , 
                          high - m_m0             ) ;
  }
  //
  // tail
  //
  const double factor = m_const * std::pow ( m_N / m_alpha * m_sigma , m_N ) ;
  //
  const double c  =  ( m_N / m_alpha - m_alpha ) * m_sigma + m_m0;
  //
  const double a  = -  low + c ;
  const double b  = - high + c ;
  //
  if ( std::fabs( m_N - 1 ) < 1.e-5 ) { return my_log ( b / a ) * factor ; }
  //
  return factor / ( 1 - m_N ) * ( std::pow ( a , 1 - m_N ) - 
                                  std::pow ( b , 1 - m_N ) ) ;
}
// ============================================================================
// get the (trunkated)  integral
// ============================================================================
void Gaudi::Math::CrystalBall::integrate () 
{
  //
  const double x0  = m_m0 - m_alpha * m_sigma ;
  //
  const double low = m_m0 - s_TRUNC * m_sigma ;
  //
  // integrate the tail:
  m_integral  = integral       ( low , x0 ) ;
  // integrate the peak:
  m_integral += gaussian_int_R ( 0.5 / m_sigma / m_sigma ,
                                 0  ,
                                 x0 ) ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBall::integral () const
{
  if ( m_integral <= 0 ) 
  {
    CrystalBall* cb = const_cast<CrystalBall*> ( this ) ;
    cb -> integrate() ;
  }
  //
  return m_integral ;
}
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
  const double N_L     ,   
  const double alpha_R , 
  const double N_R     ) 
  : std::unary_function<double,double> () 
  , m_m0       (  m0                        )
  , m_sigma    (      std::fabs ( sigma   ) )
  , m_alpha_L  (  1 + std::fabs ( alpha_L ) )
  , m_N_L      (  1 + std::fabs ( N_L     ) )
  , m_alpha_R  (  1 + std::fabs ( alpha_R ) )
  , m_N_R      (  1 + std::fabs ( N_R     ) )
//
  , m_const_L  (  1 ) 
  , m_const_R  (  1 )
  , m_integral ( -1000 ) 
{
  //
  m_const_L = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;  
  m_const_R = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;  
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
  m_integral = -1000 ;
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
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_L ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha_L ) ) { return false ; } 
  //
  m_alpha_L  = value_ ;
  m_const_L  = my_exp ( -0.5 * m_alpha_L * m_alpha_L ) ;  
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setAlpha_R ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_alpha_R ) ) { return false ; } 
  //
  m_alpha_R  = value_ ;
  m_const_R  = my_exp ( -0.5 * m_alpha_R * m_alpha_R ) ;  
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_L     ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N_L    ) ) { return false ; } 
  //
  m_N_L      = value_ ;
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
bool Gaudi::Math::CrystalBallDoubleSided::setN_R     ( const double value ) 
{
  const double value_ = 1 + std::fabs ( value );
  if ( s_equal ( value_ , m_N_R    ) ) { return false ; } 
  //
  m_N_R      = value_ ;
  m_integral = -1000  ;
  // 
  return true ;
}
// ============================================================================
//  evaluate CrystalBall's function 
// ============================================================================
double Gaudi::Math::CrystalBallDoubleSided::operator() ( const double x ) const 
{
  //
  const double dx = ( x - m_m0 ) / m_sigma ;
  //
  // left tail 
  if      ( dx  < -m_alpha_L )  // left tail 
  {
    const double f1 = m_N_L / m_alpha_L ;
    const double f2 = m_N_L / m_alpha_L - m_alpha_L - dx ;
    //
    return m_const_L * std::pow ( f1 / f2 , m_N_L ) ;  
  }
  // right tail
  else if  ( dx >  m_alpha_R )  // right tail
  {
    const double f1 = m_N_R / m_alpha_R ;
    const double f2 = m_N_R / m_alpha_R - m_alpha_R + dx ;
    //
    return m_const_R * std::pow ( f1 / f2 , m_N_R ) ;  
  }
  //
  // peak 
  //
  return my_exp ( -0.5 * dx * dx ) ;
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
  if ( low < x_low  && x_low  < high ) 
  { return integral ( low , x_low  ) + integral ( x_low  , high ) ; }
  if ( low < x_high && x_high < high ) 
  { return integral ( low , x_high ) + integral ( x_high , high ) ; }
  //
  //  "left tail"
  if     ( high <= x_low ) 
  {
    //
    const double factor = m_const_L * std::pow ( m_N_L / m_alpha_L * m_sigma , m_N_L ) ;
    //
    const double c  =  ( m_N_L / m_alpha_L - m_alpha_L ) * m_sigma + m_m0;
    //
    const double a  = -  low + c ;
    const double b  = - high + c ;
    //
    if ( std::fabs( m_N_L - 1 ) < 1.e-5 ) { return my_log ( b / a ) * factor ; }
    //
    return factor / ( 1 - m_N_L ) * ( std::pow ( a , 1 - m_N_L ) - 
                                      std::pow ( b , 1 - m_N_L ) ) ;
  }
  //  "right tail"
  else if ( low  >= x_high ) 
  {
    //
    const double factor = m_const_R * std::pow ( m_N_R / m_alpha_R * m_sigma , m_N_R ) ;
    //
    const double c  = ( m_N_R / m_alpha_R - m_alpha_R ) * m_sigma - m_m0;
    //
    const double a  = low  + c ;
    const double b  = high + c ;
    //
    if ( std::fabs ( m_N_R - 1 ) < 1.e-5 ) { return my_log ( b / a ) * factor ; }
    //
    return factor / ( 1 - m_N_R ) * ( std::pow ( b , 1 - m_N_R ) - 
                                      std::pow ( a , 1 - m_N_R ) ) ;
  }
  //
  // the peak 
  //
  return gaussian_int ( 0.5 / ( m_sigma * m_sigma ) , 
                        0            , 
                        low   - m_m0 ,
                        high  - m_m0 ) ;
}
// ============================================================================
// get the (trunkated)  integral
// ============================================================================
void Gaudi::Math::CrystalBallDoubleSided::integrate () 
{
  //
  const double x_low  = m_m0 - m_alpha_L * m_sigma ;
  const double x_high = m_m0 + m_alpha_R * m_sigma ;
  //
  const double low  = x_low  - std::max ( s_TRUNC , m_alpha_L ) * m_sigma ;
  const double high = x_high + std::max ( s_TRUNC , m_alpha_R ) * m_sigma ;
  //
  // left tail 
  m_integral  = integral ( low    , x_low ) ;
  // rigth tail
  m_integral += integral ( x_high , high  ) ;
  // integrate the peak:
  m_integral += gaussian_int ( 0.5 / ( m_sigma * m_sigma ) , 
                               0.0                         , 
                               x_low  , 
                               x_high ) ;
}
// =========================================================================
// get the integral
// =========================================================================
double Gaudi::Math::CrystalBallDoubleSided::integral () const
{
  if ( m_integral <= 0 ) 
  {
    CrystalBallDoubleSided* cb = const_cast<CrystalBallDoubleSided*> ( this ) ;
    cb -> integrate() ;
  }
  //
  return m_integral ;
}
// ============================================================================


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
  , m_workspace ( 0 ) 
//
{}
// ============================================================================
// cpoy constructor
// ============================================================================
Gaudi::Math::GramCharlierA::GramCharlierA 
( const Gaudi::Math::GramCharlierA& right ) 
  : std::unary_function<double,double> ( right ) 
//
  , m_mean   ( right.m_mean   )
  , m_sigma  ( right.m_sigma  ) 
  , m_kappa3 ( right.m_kappa3 ) 
  , m_kappa4 ( right.m_kappa4 ) 
//
  , m_workspace ( 0 ) 
//
{}
// ============================================================================
// destrructor
// ============================================================================
Gaudi::Math::GramCharlierA::~GramCharlierA()
{
  if ( 0 != m_workspace ) 
  {
    gsl_integration_workspace * _ws = (gsl_integration_workspace*) m_workspace ;
    m_workspace = 0 ;
    gsl_integration_workspace_free ( _ws );
  }
}
// ============================================================================
// evaluate Gram-Charlier type A approximation 
// ============================================================================
double Gaudi::Math::GramCharlierA::operator() ( const double x ) const 
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
  // use GSL to evaluate the integral 
  //
  GSL_Handler_Sentry sentry ;
  //
  gsl_function F                ;
  F.function = &gram_charlier_A_GSL ;
  F.params   = const_cast<GramCharlierA*> ( this ) ;
  //
  gsl_integration_workspace* ws = 0 ;
  if ( 0 == m_workspace )  
  { m_workspace = gsl_integration_workspace_alloc ( s_SIZE ) ; }
  //
  ws = (gsl_integration_workspace*) m_workspace ;
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
      ws                ,            // workspace  
      &result           ,            // the result 
      &error            ) ;          // the error in result 
  //
  if ( ierror ) 
  { 
    GSL_Handler_Sentry sentry ;
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
// The END 
// ============================================================================

