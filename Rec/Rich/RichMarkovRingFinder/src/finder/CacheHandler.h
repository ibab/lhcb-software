
#ifndef LESTER_CACHE_HANDLER_H
#define LESTER_CACHE_HANDLER_H

#include <string>

namespace Lester {

  class CacheHandler {
  public:
    static const std::string approxCoPointSepCacheFileName() {
      return ( static_cast<std::string>( getenv("PARAMFILESROOT") ) +
               "/data/Rich2_MarkovRingFinder_approxCoPointSep.cache" );
    };
  };

}

#endif
