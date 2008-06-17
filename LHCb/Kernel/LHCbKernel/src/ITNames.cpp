#include "Kernel/STChannelID.h"
#include "Kernel/ITNames.h"
#include "boost/lexical_cast.hpp"

std::string LHCb::ITNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  std::string theString = UniqueLayerToString(chan) + "Sector" + boost::lexical_cast<std::string>(chan.sector());
  return theString;
}
