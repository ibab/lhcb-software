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
  template <typename C,typename F> void loopFunction(C& c, F pmf) 
  {  for(typename C::iterator i=c.begin(); i != c.end(); ++i) pmf(*i); }
  template <typename C> void deleteContainer(C& c) 
  {  for(typename C::iterator i=c.begin(); i != c.end(); ++i) delete *i; c.clear(); }

  template <typename C,typename F> void loop(C& c, F pmf) 
  {  for(typename C::iterator i=c.begin(); i != c.end(); ++i) ((*i)->*pmf)();  }
  template <typename C,typename F> void loop(const C& c, F pmf) 
  {  for(typename C::const_iterator i=c.begin(); i != c.end(); ++i) ((*i)->*pmf)();  }
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
  inline const char* _T(const std::string& s) { return s.c_str(); }
}
#endif // FRAMEWORK_ITEMREGISTRY_H

