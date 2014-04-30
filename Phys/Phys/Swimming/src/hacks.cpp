// stdlib
#include <string>
#include <map>

// LHCb
#include <Event/TurningPoint.h>
namespace SwimmingHacks {

  LHCb::TurningPoint* createTurningPoint(const double raw, const double tau, const double ip,
					 const double dec,
					 const std::map<std::string, bool>& decisions,
					 const std::map<std::string, std::map<size_t, bool> >& info)
  {
     return new LHCb::TurningPoint(raw, tau, ip, dec, decisions, info);
  }
}
