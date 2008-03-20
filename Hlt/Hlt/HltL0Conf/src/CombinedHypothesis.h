#ifndef COMBINEDHYPOTHESIS_H 
#define COMBINEDHYPOTHESIS_H 1

#include <vector>

// Include files
#include "TfKernel/TStationHitManager.h"

/** @class CombinedHypothesis CombinedHypothesis.h
 *
 *  Helper class which combines several search windows so that decoding
 *  can be done in a single step
 *  to be used for T-station decoding in the Tf framework
 *  
 *  @author Manuel Schiller
 *  @date   2008-02-06
 */
class CombinedHypothesis : virtual public Tf::IStationSelector {
public: 
  /// Standard constructor
  CombinedHypothesis(const std::vector<Tf::IStationSelector*>& windows) :
	  m_windows(windows)
  { }

  Tf::XYSearchWindow searchWindow(double z) const {
    float xmin = 1e42, xmax = -1e42, ymin = 1e42, ymax = -1e42;
    for (std::vector<Tf::IStationSelector*>::const_iterator
           it = m_windows.begin(); m_windows.end() != it; ++it) {
      Tf::XYSearchWindow win = (*it)->searchWindow(z);
      if (xmin > win.minX()) xmin = win.minX();
      if (xmax < win.maxX()) xmax = win.maxX();
      if (ymin > win.minY()) ymin = win.minY();
      if (ymax < win.maxY()) ymax = win.maxY();
    }
    return Tf::XYSearchWindow(xmin, xmax, ymin, ymax);
  }

protected:

private:
  const std::vector<Tf::IStationSelector*>& m_windows;
};
#endif // PARABOLAHYPOTHESIS_H
