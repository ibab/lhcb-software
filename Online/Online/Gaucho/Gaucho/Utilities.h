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

#define toLowerCase(s) (\
{\
  for (unsigned int i=0;i<s.size();i++)\
  {\
    s[i] = tolower(s[i]);\
  }\
}\
)
#endif /* UTILITIES_H_ */
