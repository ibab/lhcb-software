#include "RTL/strdef.h"
#include <cstring>
#include <cstdlib>

enum { STR_NOMATCH, STR_MATCH };

int str_upcase(const char* src, char* dst, size_t dest_len)  {
  if ( dst )  {
    for(char* s1=dst, *p=dst+dest_len; *src && s1<p; s1++) {
      *s1 = ::toupper(*src++);
    }
    *s1 = 0;
    return 1;
  }
  return 0;
}

int str_lowcase(const char* src, char* dst, size_t dest_len)  {
  if ( dst )  {
    for(char* s1=dst, *p=dst+dest_len; *src && s1<p; s1++) {
      *s1 = ::tolower(*src++);
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

int str_match_wild (const char *candidate_string, const char *pattern_string)   {
  int result = STR_NOMATCH;
  if ( candidate_string && pattern_string )   {
	  size_t	s1_len = strlen(candidate_string);
    size_t  s2_len = strlen(pattern_string);	
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
			  s1_pos = s1_len;
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
