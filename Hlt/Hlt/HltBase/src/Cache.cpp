// $Id:$ 
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToStream.h"
// =============================================================================
#include "HltBase/Cache.h"
// =============================================================================
#include <boost/foreach.hpp>
// =============================================================================
namespace Hlt {
// =============================================================================
using Gaudi::StringKey;
// =============================================================================
const std::vector<const StringKey*> Cache::keys() const{
  std::vector<const StringKey*> result;
  BOOST_FOREACH (BooleanMap::reference value, m_boolean_store){
    result.push_back(&value.first);
  }
  BOOST_FOREACH (IntegerMap::reference value, m_integer_store){
      result.push_back(&value.first);
    }
  BOOST_FOREACH (DoubleMap::reference value, m_double_store){
      result.push_back(&value.first);
    }

  BOOST_FOREACH (StringMap::reference value, m_string_store){
      result.push_back(&value.first);
    }
  return result;
}

std::ostream& Cache::fillStream(std::ostream& s) const
{
  std::string delim = "";
  s << "Cache {";
  BOOST_FOREACH (BooleanMap::reference value, m_boolean_store){
    s << delim;
    Gaudi::Utils::toStream(value.first, s);
    s << ": ";
    Gaudi::Utils::toStream(value.second,s);
    delim = ", ";
  }
  BOOST_FOREACH (IntegerMap::reference value, m_integer_store){
    s << delim;
    Gaudi::Utils::toStream(value.first, s);
    s<< ": ";
    Gaudi::Utils::toStream(value.second, s);
    delim = ", ";
  }
  BOOST_FOREACH (DoubleMap::reference value, m_double_store){
      s << delim;
      Gaudi::Utils::toStream(value.first, s);
      s << ": ";
      Gaudi::Utils::toStream(value.second, s);
      delim = ", ";
  }

  BOOST_FOREACH (StringMap::reference value, m_string_store){
      s << delim;
      Gaudi::Utils::toStream(value.first, s);
      s << ": ";
      Gaudi::Utils::toStream(value.second, s);
      delim = ", ";
  }
  s << "}";
  s << " CacheKeys [";
  delim = "";
  BOOST_FOREACH(const StringKey* key, keys()) {
    s << delim;
    Gaudi::Utils::toStream(*key, s);
    delim = ", ";
  }
  s << "]";
  return s;
}
// =============================================================================
} //  Hlt
// =============================================================================
