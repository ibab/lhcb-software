
#ifndef RICHMARKOV_CACHE_HANDLER_H
#define RICHMARKOV_CACHE_HANDLER_H

#include <string>

namespace RichMarkov {

  class CacheHandler {
  public:
    static const std::string approxCoPointSepCacheFileName() {
      return ( static_cast<std::string>( getenv("PARAMFILESROOT") ) +
               "/data/Rich2_MarkovRingFinder_approxCoPointSep.cache" );
    };
  };

}

#endif
