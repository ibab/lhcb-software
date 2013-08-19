/*
 * Utilities.h
 *
 *  Created on: Jan 25, 2011
 *      Author: beat
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_
#include <string>
#include <vector>
#include <ctype.h>
extern "C" {
#include "dim.h"
}

#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

namespace
{
  template<class T> inline void releasePtr(T*& p)
  {
    if ( p )
    {
      p->release();
      p = 0;
    }
  }

  template<class T> inline void deallocPtr(T*& p)
  {
    if ( p )
    {
      free( p);
      p = 0;
    }
  }
  template<class T> inline void deletePtr(T*& p)
  {
    if ( p )
    {
      delete p;
      p = 0;
    }
  }
extern "C" {
  void toLowerCase(std::string &s);
}
  template <class T> struct ObjectLock {
    T* m_obj;
    ObjectLock(T* a) : m_obj(a) { m_obj->Lock(); }
    ~ObjectLock() { m_obj->UnLock(); }
  };

}
unsigned long long GetTime();

class dyn_string : public std::vector<std::string >
{
public:
  dyn_string() {};
  virtual ~dyn_string()
  {
  }
};
namespace
{
  class DimLock {
  public:
    DimLock()   { dim_lock();   }
    ~DimLock()  { dim_unlock(); }
  };
}


dyn_string *Strsplit(char *s, char *del);
dyn_string *Strsplit(const char *s, char *del);
dyn_string *Strsplit(char *s, const char *del);
dyn_string *Strsplit(const char *s, const char *del);
void StringReplace(std::string &in, const char *patt, std::string &repl);
void *Memcpy (void *d, void *s,size_t n);
#endif /* UTILITIES_H_ */
