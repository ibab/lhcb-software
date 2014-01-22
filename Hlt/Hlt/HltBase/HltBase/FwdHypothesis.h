// $Id: FwdHypothesis.h,v 1.4 2009-10-08 21:05:04 graven Exp $
#ifndef FWDHYPOTHESIS_H 
#define FWDHYPOTHESIS_H 1

// Include files
#include "TfKernel/TStationHitManager.h"
#include "Event/StateParameters.h"

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
  FwdHypothesis( double ax , double bx , double cx , double dx, 
                 double ay , double by, 
                 double errx , double erry, 
                 double zRef ) :
	  m_ax{ax}, m_bx{bx}, m_cx{cx}, m_dx{dx}, m_ay{ay}, m_by{by},
	  m_errx{errx}, m_erry{erry}, m_zRef{zRef}
  { }
  
  Tf::XYSearchWindow searchWindow(double z) const
  {
    double dz = z - m_zRef;
    double x = m_ax + (m_bx + dz * (m_cx + dz * m_dx)) * dz;
    double y = m_ay + dz * m_by;
    return { x - m_errx, x + m_errx, y - m_erry, y + m_erry };
  }

  void getValidity(double& zmin, double& zmax) const
  {
    zmin = StateParameters::ZBegT;
    zmax = StateParameters::ZEndT;
  }
  

private:
  double m_ax, m_bx, m_cx, m_dx;
  double m_ay, m_by;
  double m_errx, m_erry;
  double m_zRef;
};
#endif // FWDHYPOTHESIS_H
