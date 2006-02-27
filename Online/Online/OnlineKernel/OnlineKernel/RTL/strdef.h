#ifndef _RTL_STRDEF_H
#define _RTL_STRDEF_H


#ifdef __cplusplus
#define __CXX_CONSTANT const
extern "C" {
#else
#define __CXX_CONSTANT 
#endif

int str_match_wild (const char *candidate_string, const char *pattern_string);
int str_trim(char* dst, const char* src, size_t* resultant_length);
int str_upcase(const char* src, char* dst, size_t dest_len);
int str_lowcase(const char* src, char* dst, size_t dest_len);

#undef __CXX_CONSTANT
#ifdef __cplusplus
}
#endif

#endif // _RTL_STRDEF_H
