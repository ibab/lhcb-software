// $Id:$ 
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToStream.h"
// =============================================================================
#include "HltBase/Cache.h"
// =============================================================================
namespace Hlt {
// =============================================================================
using Gaudi::StringKey;
// =============================================================================
const std::vector<const StringKey*> Cache::keys() const{
  std::vector<const StringKey*> result;
  for (auto& value : m_boolean_store) result.push_back(&value.first);
  for (auto& value : m_integer_store) result.push_back(&value.first);
  for (auto& value : m_double_store)  result.push_back(&value.first);
  for (auto& value : m_string_store)  result.push_back(&value.first);
  return result;
}

std::ostream& Cache::fillStream(std::ostream& s) const
{
  std::string delim {} ;
  s << "Cache {";
  for (auto& value: m_boolean_store){
    s << delim;
    Gaudi::Utils::toStream(value.first, s);
    s << ": ";
    Gaudi::Utils::toStream(value.second,s);
    delim = ", ";
  }
  for (auto& value: m_integer_store){
    s << delim;
    Gaudi::Utils::toStream(value.first, s);
    s<< ": ";
    Gaudi::Utils::toStream(value.second, s);
    delim = ", ";
  }
  for (auto& value: m_double_store){
      s << delim;
      Gaudi::Utils::toStream(value.first, s);
      s << ": ";
      Gaudi::Utils::toStream(value.second, s);
      delim = ", ";
  }

  for (auto& value: m_string_store){
      s << delim;
      Gaudi::Utils::toStream(value.first, s);
      s << ": ";
      Gaudi::Utils::toStream(value.second, s);
      delim = ", ";
  }
  s << "}";
  s << " CacheKeys [";
  delim = "";
  for(auto key: keys()) {
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
