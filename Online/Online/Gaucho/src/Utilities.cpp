/*
 * Utilities.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: beat
 */
#include "Gaucho/Utilities.h"
#include <string>
#include <string.h>
extern "C"
{
  void toLowerCase(std::string &s)
  {
    for (unsigned int i=0;i<s.size();i++)
    {
      s[i] = tolower(s[i]);
    }
  }
}
dyn_string *Strsplit(const char *s, const char *del)
{
  return Strsplit((char*)s, (char*)del);
}
dyn_string *Strsplit(const char *s, char *del)
{
  return Strsplit((char*)s,del);
}
dyn_string *Strsplit(char *s, const char *del)
{
  return Strsplit(s,(char*)del);
}
dyn_string *Strsplit(char *s, char *del)
{
  char *pch;
  pch = strtok(s,del);
  dyn_string *v=new dyn_string();
  int nel=0;
  while (pch != 0)
  {
    v->push_back(std::string(pch));
    if (pch != s) *(pch-1) = *del;
    nel++;
    pch = strtok(0,del);
  }
  return v;
}
void StringReplace(std::string &in, const char *patt, std::string &repl)
{
  if (in.find(patt) != std::string::npos)
  {
    in.replace(in.find(patt),strlen(patt),repl);
  }

}
unsigned long long GetTime()
{
#define onesec_nano (unsigned long long)(1000000000)
  unsigned long long timstamp;
  struct timeval tv;
  struct timezone *tz;
  tz = 0;
  gettimeofday(&tv, tz);
  timstamp  = tv.tv_sec;
  timstamp *= onesec_nano;
  timstamp += tv.tv_usec*1000;
  return timstamp;
}
void *Memcpy(void *d, void *s,size_t n)
{
  return memcpy(d,s,n);
}
