#include "CheckPointing/MemMaps.h"
#include "CheckPointing.h"
#include <unistd.h>
#include <cstring>

#define MARKER 0xFEEDDADA
extern void mtcp_output(int lvl,const char* fmt,...);

size_t CheckPointing::m_strcpy(char* t, const char* s) {
  const char* q=s;
  for(; *q; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

size_t CheckPointing::m_strcat(char* t, const char* s) {
  const char* q=s;
  while( *t )++t;
  for(; *q; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

const char* CheckPointing::m_strfind(const char* s, const char* pattern) {
  return ::strstr((char*)s,pattern);
}

const char* CheckPointing::m_chrfind(const char* s, const char pattern) {
  for(; *s; ++s)  {
    if (*s==pattern) return s;
  }
  return 0;
}

char* CheckPointing::m_chrfind(char* s, const char pattern) {
  for(; *s; ++s)  {
    if (*s==pattern) return s;
  }
  return 0;
}

size_t CheckPointing::m_strlen(const char* s) {
  size_t l = 0;
  for(; *s; ++s) ++l;
  return l;
}

size_t CheckPointing::m_memcpy(void* target, const void* s, size_t len) {
  const char* q=(const char*)s;
  char* t=(char*)target;
  for(; len>0; ++q, ++t, --len) *t = *q;
  return q-(const char*)s;
}

int CheckPointing::saveInt(void* addr, int val) {
  *(int*)addr = val;
  return sizeof(int);
}

int CheckPointing::getInt(const void* addr, void* value) {
  *(int*)value = *(int*)addr;
  return sizeof(int);  
}

int CheckPointing::saveLong(void* addr, long val) {
  *(long*)addr = val;
  return sizeof(long);
}

int CheckPointing::getLong(const void* addr, void* value) {
  *(long*)value = *(long*)addr;
  return sizeof(long);  
}

#if 0
int CheckPointing::writeMarker(int fd)   { return writeMarker(fd,MARKER); }
int CheckPointing::checkMarker(int fd)   { return checkMarker(fd,MARKER); }

int CheckPointing::writeMarker(int fd, Marker flag)   {
  return ::write(fd,&flag,sizeof(flag));
}
int CheckPointing::checkMarker(int fd, Marker pattern) {
  Marker f(0);
  ::read(fd,&f,sizeof(f));
  return checkMarker(&f,pattern);
}
#endif

int CheckPointing::saveMarker(void* p, Marker flag)   {
  *(Marker*)p = flag;
  return sizeof(flag);
}

int CheckPointing::checkMarker(const void* addr, Marker pattern) {
  Marker f(*(Marker*)addr);
  if ( f != pattern ) {
    char *a=(char*)&pattern, *b=(char*)&f;
    mtcp_output(MTCP_ERROR,"ERROR Inconsistent item marker: Seen:%X '%c%c%c%c' Expected:%X '%c%c%c%c'!\n",
		f,b[0],b[1],b[2],b[3],pattern,a[0],a[1],a[2],a[3]);
    return -0xFEED;
  }
  return sizeof(Marker);
}

