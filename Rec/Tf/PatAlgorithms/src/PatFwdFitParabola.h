// $Id: PatFwdFitParabola.h,v 1.2 2009-11-26 18:00:48 mschille Exp $
#ifndef PATFWDFITPARABOLA_H
#define PATFWDFITPARABOLA_H 1

// Include files
#include <Math/CholeskyDecomp.h>
using ROOT::Math::CholeskyDecomp;

  /** @class PatFwdFitParabola PatFwdFitParabola.h
   *  Simple class to fit a parabola
   *
   *  @author Olivier Callot
   *  @date   2006-04-20 Initial version
   *  @date   2007-08-20 Update for A-Team framework 
   *  @date   2009-11-04 Update to use Cholesky decomposition
   */

  class PatFwdFitParabola final {
  public:
    /// Standard constructor
    PatFwdFitParabola( double z = 0., double x = 0., double w = 0. ) {
      //== First point, used to constraint the tangent at z=0 to go to this point.
      const double dz = 1.e-3 * z;  // use small numbers
      m_data = {{ w , w*dz, w*dz*dz, 0., 0., 0., w*x, w*x*dz, 0., 0. , 0. , 0. }};
    }

    void addPoint( double z, double x, double w ) {
      const double dz = 1.e-3*z;
      const double dz2 = dz * dz;
      m_data[0] += w;
      m_data[1] += w * dz;
      m_data[2] += w * dz2;
      m_data[3] += w * dz2;
      m_data[4] += w * dz * dz2 ;
      m_data[5] += w * dz2 * dz2;
      m_data[6] += w * x;
      m_data[7] += w * x * dz;
      m_data[8] += w * x * dz2 ;
    }

    /// return false if matrix is singular
    bool solve() {
      CholeskyDecomp<double, 3> decomp(m_data.data());
      if (!decomp) return false;
      m_data[ 9] = m_data[6];
      m_data[10] = m_data[7];
      m_data[11] = m_data[8];
      auto badInterfaceWorkaround = m_data.data()+9;
      decomp.Solve(badInterfaceWorkaround);
      m_data[10] *= 1.e-3;
      m_data[11] *= 1.e-6;
      return true;
    }

    double ax() const { return m_data[9]; }
    double bx() const { return m_data[10]; }
    double cx() const { return m_data[11]; }

  private:
    /// [0:5] : packed symetrix 3x3 matrix M in Mx = b 
    /// [6,7,8] : 3-vector b in Mx = b; 
    /// [9,10,11] solution x in Mx = b
    /* alignas(16) */ std::array<double,12> m_data; 
  };

#endif // PATFWDFITPARABOLA_H
