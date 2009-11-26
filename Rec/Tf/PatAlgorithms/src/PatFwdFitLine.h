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

  class PatFwdFitLine {
  public:
    /// Standard constructor
    PatFwdFitLine( double z = 0., double x = 0., double w = 0. ) {
      //== First point
      const double dz = 1.e-3 * z;  // use small numbers
      m_mat[0] = w;
      m_mat[1] = w * dz;
      m_mat[2] = w * dz * dz;
      m_rhs[0] = w * x;
      m_rhs[1] = w * x * dz;
      m_sol[0] = 0.;
      m_sol[1] = 0.;
    }

    virtual ~PatFwdFitLine( ) {}; ///< Destructor

    void addPoint( double z, double x, double w ) {
      const double dz = 1.e-3*z;
      m_mat[0] += w;
      m_mat[1] += w * dz;
      m_mat[2] += w * dz * dz;
      m_rhs[0] += w * x;
      m_rhs[1] += w * x * dz;
    }

    /// return false if matrix is singular
    bool solve() {
      CholeskyDecomp<double, 2> decomp(m_mat);
      if (!decomp) return false;
      m_sol[0] = m_rhs[0];
      m_sol[1] = m_rhs[1];
      decomp.Solve(m_sol);
      m_sol[1] *= 1.e-3;
      return true;
    }

    double ax() const { return m_sol[0]; }
    double bx() const { return m_sol[1]; }
    double z0() const { return 1.e3 * m_mat[1] / m_mat[0]; }

  protected:

  private:
    double m_mat[3]; /// matrix M in Mx = b
    double m_rhs[2]; /// vector b in Mx = b
    double m_sol[2]; /// (solution) vector x in Mx = b
  };

#endif // PATFWDFITLINE_H
