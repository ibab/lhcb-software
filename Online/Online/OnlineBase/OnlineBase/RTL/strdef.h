#ifndef _RTL_STRDEF_H
#define _RTL_STRDEF_H

#ifdef __cplusplus
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define __CXX_CONSTANT const
extern "C" {
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __CXX_CONSTANT 
#endif /* __cplusplus      */

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
#endif /* __cplusplus      */

#ifdef __cplusplus
#include <string>
namespace RTL {
  std::string str_lower(const std::string& str);
  std::string str_upper(const std::string& str);
  std::string str_replace(const std::string& str, const std::string& pattern, const std::string& replacement);
}

inline const char* strstr_safe(const char* p1, const char* p2)  { return ::strstr((char*)p1,p2);   }
inline const char* strchr_safe(const char* p1, char p2)         { return ::strchr((char*)p1,p2);   }

#ifdef _WIN32
#define vsnprintf _vsnprintf
#define snprintf  _snprintf
inline int str_casecmp(const char *s1, const char *s2)          { return ::_stricmp(s1, s2);     }
inline int str_ncasecmp(const char *s1,const char *s2, size_t n){ return ::_strnicmp(s1, s2, n); }
#else  // linux & gcc
inline int str_casecmp(const char *s1, const char *s2)          { return ::strcasecmp(s1, s2);     }
inline int str_ncasecmp(const char *s1,const char *s2, size_t n){ return ::strncasecmp(s1, s2, n); }
#endif   /* _WIN32         */

#endif   /* __cplusplus    */

#endif   /* _RTL_STRDEF_H  */
