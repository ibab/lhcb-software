// $Id: FwdHypothesis.h,v 1.1 2008-03-20 11:34:51 albrecht Exp $
#ifndef FWDHYPOTHESIS_H 
#define FWDHYPOTHESIS_H 1

// Include files
#include "TfKernel/TStationHitManager.h"

/** @class FwdHypothesis FwdHypothesis.h
 *  
 *  Helper class which gives a third order polynomial track 
 *  hypothesis in x and a straight line in y 
 *  to be used for T-station decoding in the Tf framework
 *
 *
 *  @author Johannes Albrecht
 *  @date   2008-01-21
 */
class FwdHypothesis : virtual public Tf::IStationSelector {
public: 
  /// Standard constructor
  FwdHypothesis( float ax , float bx , float cx , float dx, 
                 float ay , float by, 
                 float errx , float erry, 
                 float zRef ) :
	  m_ax(ax), m_bx(bx), m_cx(cx), m_dx(dx), m_ay(ay), m_by(by),
	  m_errx(errx), m_erry(erry), m_zRef(zRef)
  { }
  
  Tf::XYSearchWindow searchWindow(double z) const
  {
    float dz = float(z) - m_zRef;
    float x = m_ax + (m_bx + dz * (m_cx + dz * m_dx)) * dz;
    float y = m_ay + dz * m_by;

    return Tf::XYSearchWindow(x - m_errx, x + m_errx, y - m_erry, y + m_erry);
  }

private:
  float m_ax, m_bx, m_cx, m_dx;
  float m_ay, m_by;
  float m_errx, m_erry;
  float m_zRef;
};
#endif // FWDHYPOTHESIS_H
