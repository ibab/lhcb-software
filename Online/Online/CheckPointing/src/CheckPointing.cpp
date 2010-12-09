#include "CheckPointing/MemMaps.h"
#include "CheckPointing.h"
#include <unistd.h>

#define MARKER 0xFEEDDADA
extern void mtcp_printf(const char* fmt,...);

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

int CheckPointing::writeMarker(int fd)   { return writeMarker(fd,MARKER); }
int CheckPointing::checkMarker(int fd)   { return checkMarker(fd,MARKER); }

int CheckPointing::writeMarker(int fd, unsigned int flag)   {
  return ::write(fd,&flag,sizeof(flag));
}

int CheckPointing::checkMarker(int fd, unsigned int flag) {
  unsigned int f = 0;
  ::read(fd,&f,sizeof(f));
  if ( f != flag ) {
    mtcp_printf("ERROR Inconsistent item marker: Seen:%X Expected:%X!\n",f,flag);
    return -0xFEED;
  }
  return 4;
}
