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
#include "dim.h"

#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

namespace {
  template<class T> inline void releasePtr(T*& p)
  {
    if ( p )
    {
      p->release();
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

  inline void toLowerCase(std::string &s)
  {
    for (unsigned int i=0;i<s.size();i++)
    {
      s[i] = tolower(s[i]);
    }
  }
}

class dyn_string : public std::vector<std::string >
{
public:
  dyn_string() {};
  virtual ~dyn_string()
  {
  }
};
namespace {
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

#endif /* UTILITIES_H_ */
