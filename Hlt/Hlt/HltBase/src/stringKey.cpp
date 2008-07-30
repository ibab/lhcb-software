#include "HltBase/stringKey.h"
#include <boost/functional/hash.hpp>

size_t stringKey::hash(const std::string& s) {
    static boost::hash<std::string> hasher;
    return hasher(s);
}

void stringKey::updateHandler(Property& prop) {
    m_str = prop.toString();
    m_hash = hash(str());
}
