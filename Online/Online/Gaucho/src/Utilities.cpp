/*
 * Utilities.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: beat
 */
#include "Gaucho/Utilities.h"
#include <string>
#include <string.h>
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
};
void StringReplace(std::string &in, const char *patt, std::string &repl)
{
  if (in.find(patt) != std::string::npos)
  {
    in.replace(in.find(patt),strlen(patt),repl);
  }

}
