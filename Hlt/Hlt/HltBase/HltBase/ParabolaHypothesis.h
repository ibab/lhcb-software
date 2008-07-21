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
  ParabolaHypothesis( float my , float cy , float ax , float bx , float cx,
                      float dx, float dy ) :
	  m_y(my), m_cy(cy), m_ax(ax), m_bx(bx), m_cx(cx), m_dx(dx), m_dy(dy)
  { }

  Tf::XYSearchWindow searchWindow(double zz) const
  {
    float z = zz;
    float y =               m_y * z + m_cy;
    float x = (m_ax * z + m_bx) * z + m_cx;
    return Tf::XYSearchWindow(x - m_dx, x + m_dx, y - m_dy, y + m_dy);
  }

  void getValidity(double& zmin, double& zmax) const
  {
    zmin = double(StateParameters::ZBegT);
    zmax = double(StateParameters::ZEndT);
    return;
  }

private:
  float m_y,  m_cy;
  float m_ax, m_bx, m_cx;
  float m_dx, m_dy;
};
#endif // PARABOLAHYPOTHESIS_H
