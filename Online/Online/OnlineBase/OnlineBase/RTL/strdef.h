#ifndef _RTL_STRDEF_H
#define _RTL_STRDEF_H

#ifdef __cplusplus
#include <cstdlib>
#define __CXX_CONSTANT const
extern "C" {
#else
#include <stdlib.h>
#define __CXX_CONSTANT 
#endif

/* Case sensitive wild card matching  */
int str_match_wild (__CXX_CONSTANT char *candidate_string, __CXX_CONSTANT char *pattern_string);
/* Case insensitive wild card matching  */
int strcase_match_wild (__CXX_CONSTANT char *candidate_string, __CXX_CONSTANT char *pattern_string);
int str_trim(__CXX_CONSTANT char* src, char* dst, size_t* resultant_length);
int str_upcase(__CXX_CONSTANT char* src, char* dst, size_t dest_len);
int str_lowcase(__CXX_CONSTANT char* src, char* dst, size_t dest_len);

#undef __CXX_CONSTANT
#ifdef __cplusplus
}
#endif

#endif // _RTL_STRDEF_H
