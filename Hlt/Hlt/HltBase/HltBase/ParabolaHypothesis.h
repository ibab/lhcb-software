#ifndef PARABOLAHYPOTHESIS_H
#define PARABOLAHYPOTHESIS_H 1

// Include files
#include "TfKernel/TStationHitManager.h"
#include "Event/StateParameters.h"

/** @class ParabolaHypothesis ParabolaHypothesis.h
 *
 *  Helper class which gives a parabola track hypothesis
 *  in x and a straight line in y
 *  to be used for T-station decoding in the Tf framework
 *
 *
 *  @author Johannes Albrecht
 *  @date   2008-01-18
 */
class ParabolaHypothesis : virtual public Tf::IStationSelector {
  public:
    /// Standard constructor
    ParabolaHypothesis( double my, double cy, double ax, double bx, double cx,
                        double dx, double dy )
        : m_y( my ),
          m_cy( cy ),
          m_ax( ax ),
          m_bx( bx ),
          m_cx( cx ),
          m_dx( dx ),
          m_dy( dy ) {}

    Tf::XYSearchWindow searchWindow( double zz ) const {
        double z = zz;
        double y = m_y * z + m_cy;
        double x = ( m_ax * z + m_bx ) * z + m_cx;
#ifdef __GCCXML__
        return Tf::XYSearchWindow( x - m_dx, x + m_dx, y - m_dy, y + m_dy );
#else
        return {x - m_dx, x + m_dx, y - m_dy, y + m_dy};
#endif
    }

    void getValidity( double &zmin, double &zmax ) const {
        zmin = StateParameters::ZBegT;
        zmax = StateParameters::ZEndT;
    }

  private:
    double m_y, m_cy;
    double m_ax, m_bx, m_cx;
    double m_dx, m_dy;
};
#endif // PARABOLAHYPOTHESIS_H
