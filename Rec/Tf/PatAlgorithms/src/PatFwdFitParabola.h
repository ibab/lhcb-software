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

  class PatFwdFitParabola {
  public:
    /// Standard constructor
    PatFwdFitParabola( double z = 0., double x = 0., double w = 0. ) {
      //== First point, used to constraint the tangent at z=0 to go to this point.
      const double dz = 1.e-3 * z;  // use small numbers
      m_mat[0] = w;
      m_mat[1] = w * dz;
      m_mat[2] = w * dz * dz;
      m_mat[3] = 0.;
      m_mat[4] = 0.;
      m_mat[5] = 0.;
      m_rhs[0] = w * x;
      m_rhs[1] = w * x * dz;
      m_rhs[2] = 0.;
      m_sol[0] = 0.;
      m_sol[1] = 0.;
      m_sol[2] = 0.;
    }

    virtual ~PatFwdFitParabola( ) {}; ///< Destructor

    void addPoint( double z, double x, double w ) {
      const double dz = 1.e-3*z;
      m_mat[0] += w;
      m_mat[1] += w * dz;
      m_mat[2] += w * dz * dz;
      m_mat[3] += w * dz * dz;
      m_mat[4] += w * dz * dz * dz;
      m_mat[5] += w * dz * dz * dz * dz;
      m_rhs[0] += w * x;
      m_rhs[1] += w * x * dz;
      m_rhs[2] += w * x * dz * dz;
    }

    /// return false if matrix is singular
    bool solve() {
      CholeskyDecomp<double, 3> decomp(m_mat);
      if (!decomp) return false;
      m_sol[0] = m_rhs[0];
      m_sol[1] = m_rhs[1];
      m_sol[2] = m_rhs[2];
      decomp.Solve(m_sol);
      m_sol[1] *= 1.e-3;
      m_sol[2] *= 1.e-6;
      return true;
    }

    double ax() const { return m_sol[0]; }
    double bx() const { return m_sol[1]; }
    double cx() const { return m_sol[2]; }

  protected:

  private:
    double m_mat[6]; /// matrix M in Mx = b
    double m_rhs[3]; /// (right hand side) vector b in Mx = b
    double m_sol[3]; /// (solution) vector x in Mx = b
  };

#endif // PATFWDFITPARABOLA_H
