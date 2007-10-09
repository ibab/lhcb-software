// $Id: PatFwdFitLine.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATFWDFITLINE_H
#define PATFWDFITLINE_H 1

// Include files


  /** @class PatFwdFitLine PatFwdFitLine.h
   *  Simple class to fit a line
   *
   *  @author Olivier Callot
   *  @date   2006-04-20 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   */

  class PatFwdFitLine {
  public:
    /// Standard constructor
    PatFwdFitLine( double z = 0., double x = 0., double w = 0. ) {
      //== First point
      double dz = 1.e-3 * z;  // use small numbers
      m_s0   = w;
      m_sz   = w * dz;
      m_sz2  = w * dz * dz;
      m_sx   = w * x;
      m_sxz  = w * x * dz;
    }

    virtual ~PatFwdFitLine( ) {}; ///< Destructor

    void addPoint( double z, double x, double w ) {
      double dz = 1.e-3*z;
      m_s0  += w;
      m_sz  += w * dz;
      m_sz2 += w * dz * dz;
      m_sx  += w * x;
      m_sxz += w * x * dz;
    }

    void solve()  {
      m_ax = ( m_sx  * m_sz2 - m_sxz * m_sz ) / ( m_s0  * m_sz2 - m_sz * m_sz );
      m_bx = ( m_s0  * m_sxz - m_sx  * m_sz ) / ( m_s0  * m_sz2 - m_sz * m_sz );
      m_bx *= 1.e-3;
    }

    double ax() const { return m_ax; }
    double bx() const { return m_bx; }
    double z0() const { return 1.e3 * m_sz / m_s0; }

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
  };

#endif // PATFWDFITLINE_H
