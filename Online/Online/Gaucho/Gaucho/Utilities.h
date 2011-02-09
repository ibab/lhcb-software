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

#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

static inline void toLowerCase(std::string &s)
{
  for (unsigned int i=0;i<s.size();i++)
  {
    s[i] = tolower(s[i]);
  }
}

class dyn_string : public std::vector<std::string >
{
public:
  dyn_string() {};
  ~dyn_string()
  {
  }
};

dyn_string *Strsplit(char *s, char *del);

#endif /* UTILITIES_H_ */
