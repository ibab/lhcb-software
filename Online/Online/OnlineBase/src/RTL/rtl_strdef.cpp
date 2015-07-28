#include "RTL/strdef.h"
#include <cctype>
#include <cstring>
#include <cstdlib>

enum { STR_NOMATCH, STR_MATCH };

int str_upcase(const char* src, char* dst, size_t dest_len)  {
  if ( dst )  {
    char* s1=dst, *p=dst+dest_len;
    while(*src && s1<p) {
      *s1++ = char(::toupper(*src++));
    }
    *s1 = 0;
    return 1;
  }
  return 0;
}

int str_lowcase(const char* src, char* dst, size_t dest_len)  {
  if ( dst )  {
    char* s1=dst, *p=dst+dest_len;
    while(*src && s1<p) {
      *s1++ = char(::tolower(*src++));
    }
    *s1 = 0;
    return 1;
  }
  return 0;
}

int str_trim(const char* src, char* dst, size_t* resultant_length)    {
  if ( dst )  {
    if ( src )  {
      size_t s2_length = strlen(src);		// Length of string
      // Determine the end of the string
      while ((s2_length > 0) && ((src[s2_length-1] == ' ') || (src[s2_length-1] == '\t')))	{
        s2_length--;
      }
      *resultant_length = *resultant_length < s2_length ? *resultant_length - 1 : s2_length;
      dst[*resultant_length] = 0;
      // Now, copy that much to the destination
      memcpy(dst, src, *resultant_length);
      return *resultant_length;
    }
    memset(dst,0,*resultant_length);
  }
  return 0;
}

static bool match1(const char* pat, const char* str, bool case_sensitive) {
  //
  // Credits: Code from Alessandro Felice Cantatore.
  // 
  static char mapCaseTable[256];
  static char mapNoCaseTable[256];
  static bool first = true;

  const char *s, *p, *table;
  bool star = false;
  if ( first ) {
    for (int i = 0; i < 256; ++i) {
      mapCaseTable[i] = char(i);
      mapNoCaseTable[i] = char(( i >= 'a' && i <='z' ) ? i+'A'-'a' : i);
    }
    first = false;
  }
  table = case_sensitive ? mapCaseTable : mapNoCaseTable;
  
 loopStart:
  for (s = str, p = pat; *s; ++s, ++p) {
    switch (*p) {
    case '?':
      if (*s == '.') goto starCheck;
      break;
    case '*':
      star = true;
      str = s, pat = p;
      do { ++pat; } while (*pat == '*');
      if (!*pat) return true;
      goto loopStart;
    default:
      if ( *(table+*s) != *(table+*p) )
        goto starCheck;
      break;
    } /* endswitch */
  } /* endfor */
  while (*p == '*') ++p;
  return (!*p);
  
 starCheck:
  if (!star) return false;
  str++;
  goto loopStart;
}

#if 0
static bool match0(const char *pattern, const char *candidate)  {
  switch (*pattern)   {
  case '\0':
    return !*candidate;
  case '*':
    return match0(pattern+1, candidate) || (*candidate && match0(pattern, candidate+1));
  case '?':
    return *candidate && match0(pattern+1, candidate+1);
  default:
    return (*pattern == *candidate) && match0(pattern+1, candidate+1);   
  }
}
#endif

int str_match_wild (const char *candidate_string, const char *pattern_string)   {
  return match1(pattern_string,candidate_string,true) ? STR_MATCH : STR_NOMATCH;
}

int strcase_match_wild (const char *candidate_string, const char *pattern_string)   {
  return match1(pattern_string,candidate_string,false) ? STR_MATCH : STR_NOMATCH;
}

/// C++ version to convert a string to lower case
std::string RTL::str_lower(const std::string& str) {
  std::string res = str.c_str();
  for(char* p=(char*)res.c_str(); *p; ++p) *p = ::tolower(*p);
  return res;
}

/// C++ version to convert a string to upper case
std::string RTL::str_upper(const std::string& str) {
  std::string res = str.c_str();
  for(char* p=(char*)res.c_str(); *p; ++p) *p = ::toupper(*p);
  return res;
}

/// C++ version: replace all occurrences of a string
std::string RTL::str_replace(const std::string& str, const std::string& pattern, const std::string& replacement) {
  std::string res = str;
  for(size_t id=res.find(pattern); id != std::string::npos; id = res.find(pattern) )
    res.replace(id,pattern.length(),replacement);
  return res;
}

#if 0
int str_match_wild (const char *candidate_string, const char *pattern_string)   {
  int result = STR_NOMATCH;
  if ( candidate_string && pattern_string )   {
    size_t s1_len = strlen(candidate_string);
    size_t s2_len = strlen(pattern_string);	
    size_t s1_pos = 0;
    size_t s2_pos = 0;
    
    result = STR_MATCH;
    for ( s1_pos = 0; s1_pos < s1_len; s1_pos++  )	  {
      if ( candidate_string[s1_pos] == pattern_string[s2_pos] )  {
        result = STR_MATCH;
        s2_pos++;
      }
      else if ( pattern_string[s2_pos] == '%' )	 {
        result = STR_MATCH;
        s2_pos++;
      }
      else if ( pattern_string[s2_pos] == '*' )   {
        if (s2_pos+1 >= s2_len )	{ // wild card last char 
          s1_pos = s1_len;
          s2_pos++;
          result = STR_MATCH;
        }
        else	 { // wild card not last character
          if ( candidate_string[s1_pos] == pattern_string[s2_pos+1] )  {
            s2_pos += 2;
          } 
        }
      }
      else  {
        result = STR_NOMATCH;
        // s1_pos = s1_len;
        s2_pos = 0;
      }
    } // end for
    
    
    // Special case last char is * which can represent nothing
    if (( pattern_string[s2_pos] == '*' ) && ( s2_pos+1 == s2_len ))
      s2_pos++;
    
    // we left uncheck characters in the candidate string
    if ( s2_pos != s2_len ) 	{
      result = STR_NOMATCH;
    }
  }
  return result;
}
#endif
