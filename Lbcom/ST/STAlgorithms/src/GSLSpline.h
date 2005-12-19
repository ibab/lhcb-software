// $Id: GSLSpline.h,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
#ifndef _GSLSpline_H
#define _GSLSpline_H

#include <vector>
#include <gsl/gsl_spline.h>

class GSLSpline{

public:

  /// constructer
  GSLSpline(const std::vector<double>& x, 
            const std::vector<double>& y, 
            const gsl_interp_type* interType = gsl_interp_cspline) ;

  /// destructer
  virtual ~GSLSpline();

  /// evaluate method ie calc y from x
  double value(const double x);

  /// first derivative
  double firstDerivative(const double x ) const;

  /// second derivative
  double secondDerivative(const double x ) const;

  /// integral
  double integral (const double from, const double to) const;

private:

  // spline
  gsl_spline* m_spline;

  // accelerator
  gsl_interp_accel* m_accel;
   
}; 

inline double GSLSpline::value(const double x){
  return gsl_spline_eval(m_spline, x, m_accel);
} 

inline double GSLSpline::firstDerivative( const double x ) const
{
  return gsl_spline_eval_deriv( m_spline, x, m_accel );
}

inline double GSLSpline::secondDerivative( const double x ) const
{
  return gsl_spline_eval_deriv2( m_spline, x, m_accel );
}

inline double GSLSpline::integral (const double from, const double to ) const
{
  return gsl_spline_eval_integ( m_spline, from, to, m_accel );
}


#endif
