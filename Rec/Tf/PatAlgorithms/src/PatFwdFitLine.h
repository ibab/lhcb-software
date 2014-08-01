// $Id: PatFwdFitLine.h,v 1.4 2009-11-26 18:00:48 mschille Exp $
#ifndef PATFWDFITLINE_H
#define PATFWDFITLINE_H 1

// Include files
#include <Math/CholeskyDecomp.h>
using ROOT::Math::CholeskyDecomp;

  /** @class PatFwdFitLine PatFwdFitLine.h
   *  Simple class to fit a line
   *
   *  @author Olivier Callot
   *  @date   2006-04-20 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   *  @date   2009-22-04 Update to use Cholesky decomposition
   */

  class PatFwdFitLine final {
  public:
    /// Standard constructor
    PatFwdFitLine( double z = 0., double x = 0., double w = 0. ) {
      //== First point
      const double dz = 1.e-3*z;
      m_data = {{ w, w*dz, w*dz*dz, w*x, w*x*dz, 0. , 0. }};
    }

    void addPoint( double z, double x, double w ) {
      const double dz = 1.e-3*z;
      m_data[0] += w;
      m_data[1] += w * dz;
      m_data[2] += w * dz * dz;
      m_data[3] += w * x;
      m_data[4] += w * x * dz;
    }

    /// return false if matrix is singular
    bool solve() {
      CholeskyDecomp<double, 2> decomp(m_data.data());
      if (!decomp) return false;
      m_data[5] = m_data[3];
      m_data[6] = m_data[4];
      auto badInterfaceWorkaround = m_data.data()+5;
      decomp.Solve(badInterfaceWorkaround);
      m_data[6] *= 1.e-3;
      return true;
    }

    double ax() const { return m_data[5]; }
    double bx() const { return m_data[6]; }
    double cx() const { return 0.; }
    double z0() const { return 1.e3 * m_data[1] / m_data[0]; }

  private:
    // [0,1,2] = packed 2x2 symmetric matrix M in Mx = b
    // [3,4]   = vector b  in Mx = b
    // [5,6]   = solution x in Mx = b
    /*alignas(16)*/ std::array<double,7> m_data; 
  };

#endif // PATFWDFITLINE_H
