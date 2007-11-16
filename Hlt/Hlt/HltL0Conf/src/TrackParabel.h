// $Id: TrackParabel.h,v 1.1 2007-11-16 11:36:17 albrecht Exp $
#ifndef TRACKPARABEL_H 
#define TRACKPARABEL_H 1

// Include files
#include "Event/State.h"
#include "TfKernel/TStationHitManager.h"

/** @class TrackParabel TrackParabel.h
 *
 *  Helper class which parameterizes the track hypothesis as
 *  parabel through the T-stations using the momentum to determine 
 *  curvature
 *
 *  Assumes a state at T3 as input
 *
 *  @author Johannes Albrecht
 *  @date   2007-10-30
 */
class TrackParabel : virtual public Tf::IStationSelector {
public: 
  /// Standard constructor
  TrackParabel(const LHCb::State& aState, double nSigma)
  {
    m_y = aState.ty();
    c_y = aState.y() - m_y*aState.z();
    
    a_x = aState.qOverP()/42.;
    b_x = aState.tx() -2*aState.z()*a_x;
    c_x = aState.x() - aState.z()*(b_x + a_x*aState.z());

    m_dy = nSigma*sqrt(aState.errY2());
    m_dx = nSigma*sqrt(aState.errX2());
  }

  Tf::XYSearchWindow searchWindow(double z) const{
    double x = z*((a_x*z)+b_x)+c_x;
    double y = m_y*z+c_y;
    Tf::XYSearchWindow win(x-m_dx, x+m_dx, y-m_dy, y+m_dy);
    return win;
  }

protected:

private:
  double m_y;
  double c_y;
  double a_x;
  double b_x;
  double c_x;
  double m_dy;
  double m_dx;

};
#endif // TRACKPARABEL_H
