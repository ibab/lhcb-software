/*
 * Utilities.h
 *
 *  Created on: Jan 25, 2011
 *      Author: beat
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_
#include <string>
#include <ctype.h>

#define AddPtr(ptr,offs) (void*)((char *)ptr +offs)

static inline void toLowerCase(std::string& s)
{
  for(size_t i=0;i<s.size();i++)
  {
    s[i] = tolower(s[i]);
  }
}

#endif /* UTILITIES_H_ */
