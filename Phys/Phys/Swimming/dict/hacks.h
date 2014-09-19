// $Id: $
#ifndef SWIMMING_HACKS_H 
#define SWIMMING_HACKS_H 1

#include <map>
#include <string>
#include <vector>

namespace LHCb {
  class TurningPoint;
}

namespace SwimmingHacks {
  LHCb::TurningPoint* createTurningPoint(const double raw, const double tau, const double ip,
					 const double dec,
					 const std::map<std::string, bool>& decisions,
					 const std::map<std::string, std::map<size_t, bool> >& info,
                                         const std::vector<std::pair<std::string, double> >& extra);
}

#endif // SWIMMING_HACKS_H
