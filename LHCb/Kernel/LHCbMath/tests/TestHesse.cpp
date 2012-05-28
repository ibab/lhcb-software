// $Id: TestEigen.cpp 124901 2011-06-16 12:46:28Z ibelyaev $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Hesse.h"
#include "LHCbMath/GSL_utils.h"
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_multimin.h"
// ============================================================================
/** @file
 *  @date 2010-02-09
 */
// ============================================================================
namespace 
{
  const size_t N = 24 ;
  // ==========================================================================
  /// copy the function example from GSL 
  double my_f ( const gsl_vector *v, void *params)
  {
    double x, y;
    double *p = (double *)params;
    
    x = gsl_vector_get(v, 0);
    y = gsl_vector_get(v, 1);    
    
    return
      p[2] *  ( x - p[0]) * ( x - p[0] ) +
      p[3] *  ( y - p[1]) * ( y - p[1] ) + p[4] 
      + pow ( ( x - p[0] ) , N ) 
      + pow ( ( y - p[1] ) , N ) ; 
    ;
  }
  /* The gradient of f, df = (df/dx, df/dy). */
  void my_df ( const gsl_vector *v      , 
               void             *params , 
               gsl_vector       *df     )
  {
    double x, y;
    double *p = (double *)params;
    
    x = gsl_vector_get(v, 0);
    y = gsl_vector_get(v, 1);
    
    // gsl_vector_set(df, 0, 2.0 * p[2] * (x - p[0]));
    // gsl_vector_set(df, 1, 2.0 * p[3] * (y - p[1]));
    
    gsl_vector_set(df, 0, 2.0 * p[2] * (x - p[0]) + N * pow ( x-p[0] , N-1 ) ) ;
    gsl_vector_set(df, 1, 2.0 * p[3] * (y - p[1]) + N * pow ( y-p[1] , N-1 ) ) ;
    
    // gsl_vector_set(df, 0, N * pow ( x-p[0] , N-1 ) ) ;
    // gsl_vector_set(df, 1, N * pow ( y-p[1] , N-1 ) ) ;

  }
  /* Compute both f and df together. */
  void my_fdf ( const gsl_vector *x      , 
                void             *params , 
                double           *f      , 
                gsl_vector       *df     ) 
  {
    *f = my_f(x, params); 
    my_df(x, params, df);
  }
  // ==========================================================================
}
// ============================================================================
// ============================================================================
int main()
{
 
  // ==========================================================================
  // function example form GSL 
  // ==========================================================================
  gsl_multimin_function_fdf my_func;
  
  /* Paraboloid center at (1,2), scale factors (10, 20), minimum value 30 */
  double p[5] = { 1.0, 2.0, 10.0, 20.0, 30.0 };
  // double p[5] = { 0.0, 0.0, 10.0, 20.0, 30.0 };
  // double p[5] = { 1.0, 2.0, 10.0, 0.00000001 , 30.0 };
  // double p[5] = { 1.0, 2.0, 10.0, 2000000.0, 30.0 };
  // double p[5] = { 0.0, 0.0, 1.0, 1.0, 1.0 };
  
  my_func.n      = 2         ;  /* number of function components */
  my_func.f      = &my_f     ;
  my_func.df     = &my_df    ;
  my_func.fdf    = &my_fdf   ;
  my_func.params = (void *)p ;
  
  gsl_vector* x = gsl_vector_calloc ( 2 ) ;
  
  /* Starting point, x = (5,7) */
  x = gsl_vector_alloc (  2);
  gsl_vector_set (x, 0, 5.0);
  gsl_vector_set (x, 1, 7.0);
  
  size_t iter = 0;
  int status;
  
  const gsl_multimin_fdfminimizer_type *T;
  gsl_multimin_fdfminimizer *s;
  
  T = gsl_multimin_fdfminimizer_conjugate_fr;
  s = gsl_multimin_fdfminimizer_alloc (T, 2);
  
  gsl_multimin_fdfminimizer_set (s, &my_func, x, 0.01, 1e-4);
  
  do
  {
    iter++;
    status = gsl_multimin_fdfminimizer_iterate (s);
    
    if (status)
      break;
    
    status = gsl_multimin_test_gradient (s->gradient, 1e-4);
    
    if (status == GSL_SUCCESS)
      printf ("Minimum found at:\n");
    
    int ii = iter ;
    printf ("%5d %.5f %.5f %10.5f\n", 
            ii  ,
            gsl_vector_get (s->x, 0), 
            gsl_vector_get (s->x, 1), 
            s->f);
    
  }
  while ( status == GSL_CONTINUE && iter < 100);
  //
  gsl_vector_set ( x , 0 , p[0] + 0 * 0.001 ) ;
  gsl_vector_set ( x , 1 , p[1] - 0 * 0.001 ) ;  
  //
  Gaudi::Math::GSL::Hesse hesse  ( &my_f    , 
                                   x        , 
                                   (void*)p , 
                                   0.5      ) ;
  
  if ( 0 == hesse.hesse () ) { hesse.calcHesse () ; }
  const gsl_matrix* h = hesse.hesse  () ;
  if ( 0 == hesse.cov2  () ) { hesse.calcCov2  () ; }
  const gsl_matrix* c = hesse.cov2   () ;

  std::cout << " HESSE \n" << (*h) << std::endl ;
  std::cout << " COV2  \n" << (*c) << std::endl ;
  
  //
  gsl_multimin_fdfminimizer_free (s);
  gsl_vector_free (x);
  //
  return 0 ;
} 
// ============================================================================
//                                                                      The END 
// ============================================================================
