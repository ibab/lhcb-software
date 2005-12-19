
#include "GSLSpline.h"

GSLSpline::GSLSpline(const std::vector<double>& x, 
                     const std::vector<double>& y,
                     const gsl_interp_type* interType):
  m_spline(0),
  m_accel(0)
{

 // constructer
  /*
 double ax[x.size()];
 double ay[x.size()];

 for (unsigned int i = 0; i< x.size(); ++i){
    ax[i] = x[i];
    ay[i] = y[i];
 } // i
  */
 // implicit news 
 m_accel = gsl_interp_accel_alloc();
 m_spline  = gsl_spline_alloc(interType, x.size());
 gsl_spline_init(m_spline, &x[0], &y[0], x.size());
}


GSLSpline::~GSLSpline(){
  // destructer - deletes 
  if (0 != m_spline) gsl_spline_free (m_spline);
  if (0 != m_accel) gsl_interp_accel_free(m_accel);
}
