#include "HltBase/stringKey.h"

size_t stringKey::hash(const std::string& s) {
    static boost::hash<std::string> hasher;
    return hasher(s);
}
