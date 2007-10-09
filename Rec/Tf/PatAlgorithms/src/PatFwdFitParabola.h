// $Id: PatFwdFitParabola.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATFWDFITPARABOLA_H
#define PATFWDFITPARABOLA_H 1

// Include files

  /** @class PatFwdFitParabola PatFwdFitParabola.h
   *  Simple class to fit a parabola
   *
   *  @author Olivier Callot
   *  @date   2006-04-20 Initial version
   *  @date   2007-08-20 Update for A-Team framework 
   */

  class PatFwdFitParabola {
  public:
    /// Standard constructor
    PatFwdFitParabola( double z = 0., double x = 0., double w = 0. ) {
      //== First point, used to constraint the tangent at z=0 to go to this point.
      double dz = 1.e-3 * z;  // use small numbers
      m_s0   = w;
      m_sz   = w * dz;
      m_sz2m = w * dz * dz;
      m_sz2  = 0.;
      m_sz3  = 0.;
      m_sz4  = 0.;
      m_sx   = w * x;
      m_sxz  = w * x * dz;
      m_sxz2 = 0.;
    }

    virtual ~PatFwdFitParabola( ) {}; ///< Destructor

    void addPoint( double z, double x, double w ) {
      double dz = 1.e-3*z;
      m_s0  += w;
      m_sz  += w * dz;
      m_sz2 += w * dz * dz;
      m_sz2m+= w * dz * dz;
      m_sz3 += w * dz * dz * dz;
      m_sz4 += w * dz * dz * dz * dz;
      m_sx  += w * x;
      m_sxz += w * x * dz;
      m_sxz2+= w * x * dz * dz;
    }

    void solve()  {
      double A1 = m_s0 * m_sz3 - m_sz   * m_sz2;
      double B1 = m_sz * m_sz3 - m_sz2  * m_sz2m;
      double C1 = m_sx * m_sz3 - m_sxz  * m_sz2;
      double A2 = m_s0 * m_sz4 - m_sz2  * m_sz2;
      double B2 = m_sz * m_sz4 - m_sz3  * m_sz2;
      double C2 = m_sx * m_sz4 - m_sxz2 * m_sz2;

      m_ax = ( B2 * C1 - B1 * C2 ) / (A1 * B2 - A2 * B1 );
      m_bx = ( A1 * C2 - A2 * C1 ) / (A1 * B2 - A2 * B1 );
      m_cx = ( m_sx  - m_ax * m_s0 - m_bx * m_sz ) / m_sz2;
      m_bx *= 1.e-3;
      m_cx *= 1.e-6;
    }

    double ax() const { return m_ax; }
    double bx() const { return m_bx; }
    double cx() const { return m_cx; }

  protected:

  private:
    double m_s0;
    double m_sz;
    double m_sz2;
    double m_sz2m;
    double m_sz3;
    double m_sz4;
    double m_sx;
    double m_sxz;
    double m_sxz2;

    double m_ax;
    double m_bx;
    double m_cx;
  };

#endif // PATFWDFITPARABOLA_H
