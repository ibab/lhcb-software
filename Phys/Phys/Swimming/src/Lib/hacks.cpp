// stdlib
#include <string>
#include <map>
#include <vector>

// LHCb
#include <Event/TurningPoint.h>
namespace SwimmingHacks {

  LHCb::TurningPoint* createTurningPoint(const double raw, const double tau, const double ip,
					 const double dec,
					 const std::map<std::string, bool>& decisions,
					 const std::map<std::string, std::map<size_t, bool> >& info,
                                         const std::vector<std::pair<std::string, double> > &extra)
  {
     std::map<std::string, double> extra_fudged;
     for(std::vector<std::pair<std::string, double> >::const_iterator i = extra.begin(); i != extra.end(); i++)
       extra_fudged[i->first] = i->second;
     return new LHCb::TurningPoint(raw, tau, ip, dec, decisions, info, extra_fudged);
  }
}
