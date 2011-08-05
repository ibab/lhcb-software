#ifndef FRAMEWORK_HELPERS_H
#define FRAMEWORK_HELPERS_H

#include <string>
/*
*    Framework namespace declaration
*/
namespace Framework {
  template <class T> inline void releasePtr(T*& p)  { if ( p ) { p->release(); p = 0; } }
  template <class T> inline void deletePtr(T*& p)   { if ( p ) { delete p; p = 0; } }
  template <class T> inline void deleteArray(T*& p) { if ( p ) { delete [] p; p = 0; } }

  inline unsigned int hash32(const char* key) {
    unsigned int hash = 0;
    for (; *key; key++)
      hash += *key; hash += (hash << 10); hash ^= (hash >> 6);
    hash += (hash << 3); 
    hash ^= (hash >> 11); 
    hash += (hash << 15);
    return hash;
  }
  inline unsigned int hash32(const std::string& key) {    return hash32(key.c_str());  }
}
#endif // FRAMEWORK_ITEMREGISTRY_H

