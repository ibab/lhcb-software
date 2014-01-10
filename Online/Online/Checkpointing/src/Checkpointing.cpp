#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>

using namespace Checkpointing;

#ifdef CHECKPOINTING_HAS_NAMESPACE
namespace CHECKPOINTING_NAMESPACE {
#endif

HIDDEN(int)    m_intcheck(const void* a, const void* b)   {
  const int* ia = (const int*)a;
  const int* ib = (const int*)b;
  return *ia == *ib ? 1 : 0;
}

HIDDEN(int)    m_longcheck(const void* a, const void* b)   {
  const long* ia = (const long*)a;
  const long* ib = (const long*)b;
  return *ia == *ib ? 1 : 0;
}

HIDDEN(int)    m_isspace(char s) {
  return s==' '||s=='\n'||s=='\t'||s=='\r'||s=='\v'||s=='\f' ? 1 : 0;
}

HIDDEN(int) m_strcmp(const char* t, const char* s) {
  if ( t && t == s ) {
    return 0;
  }
  else if ( t && s ) {
    for(; *t && *s && *t==*s; ++s, ++t) ;
    if ( *t == *s ) return 0;
    return 1;
  }
  return 0;
}

HIDDEN(int) m_strncmp(const char* t, const char* s, size_t len) {
  if ( t && s ) {
    for(size_t i=1; i<len && *t && *s && *t==*s; ++s, ++t, ++i) ;
    if ( *t == *s ) return 0;
  }
  return 1;
}

HIDDEN(size_t) m_strncpy(char* t, const char* s, size_t len) {
  const char* q=s;
  for(; *q && --len; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

HIDDEN(size_t) m_strcpy(char* t, const char* s) {
  const char* q=s;
  for(; *q; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

HIDDEN(size_t) m_strcat(char* t, const char* s) {
  const char* q=s;
  while( *t )++t;
  for(; *q; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

HIDDEN(const char*) m_strfind(const char* s, const char* pattern) {
  return ::strstr((char*)s,pattern);
}

HIDDEN(const char*) m_chrfind(const char* s, const char pattern) {
  for(; *s; ++s)  {
    if (*s==pattern) return s;
  }
  return 0;
}

HIDDEN(char*) m_chrfind(char* s, const char pattern) {
  for(; *s; ++s)  {
    if (*s==pattern) return s;
  }
  return 0;
}

HIDDEN(size_t) m_strlen(const char* s) {
  size_t l = 0;
  for(; *s; ++s) ++l;
  return l;
}

HIDDEN(size_t) m_memset(void* target, unsigned char pattern, size_t len) {
  unsigned char* t=((unsigned char*)target)+len-1;
  for(; t>=target;--t) *t = pattern;
  return len;
}

HIDDEN(size_t) m_memcpy(void* target, const void* source, size_t len) {
  const char* s=(const char*)source;
  char* t=(char*)target;
  for(; len>0; ++s, ++t, --len) *t = *s;
  return s-(const char*)source;
}

HIDDEN(int) saveMarker(void* p, Marker flag)   {
  *(Marker*)p = flag;
  return sizeof(flag);
}

HIDDEN(int) saveInt(void* addr, int val) {
  *(int*)addr = val;
  return sizeof(int);
}

HIDDEN(int) saveLong(void* addr, long val) {
  *(long*)addr = val;
  return sizeof(long);
}

HIDDEN(int) getInt(const void* addr, void* value) {
  *(int*)value = *(int*)addr;
  return sizeof(int);  
}

HIDDEN(int) getLong(const void* addr, void* value) {
  *(long*)value = *(long*)addr;
  return sizeof(long);  
}

HIDDEN(int) writeMarker(int fd, Marker flag)   {
  return mtcp_sys_write(fd,&flag,sizeof(flag));
}

HIDDEN(int) writeInt(int fd, int val)   {
  return mtcp_sys_write(fd,&val,sizeof(val));
}

HIDDEN(int) writeLong(int fd, long val)   {
  return mtcp_sys_write(fd,&val,sizeof(val));
}

HIDDEN(size_t) m_writemem(int fd, const void* ptr, size_t size) {
  static char const zeroes[4096] = { 0 };
  const char* buff = (const char*)ptr;
  char const *bf = buff;
  int num_zero = 0;

  for(size_t sz = size; sz > 0; ) {
    ssize_t rc = 0;
    size_t wt;
    for (wt = sz; wt > 0; wt /= 2) {
      rc = write (fd, bf, wt);
      if ((rc >= 0) || (errno != EFAULT)) break;
    }
    // Sometimes image page alignment will leave a hole in the middle of an image
    // ... but the idiot proc/self/maps will include it anyway
    if (wt == 0) {
      rc = (sz > sizeof zeroes ? sizeof zeroes : sz);
      m_writemem(fd, zeroes, rc);
      num_zero += rc;
    }
    // Otherwise, check for real error
    else {
      if (rc == 0) errno = EPIPE;
      if (rc <= 0) {
        mtcp_output(MTCP_FATAL,"writefile: error writing from address %p %s\n",
		    bf, ::strerror (errno));
      }
    }
    // It's ok, we're on to next part
    sz -= rc;
    bf += rc;
  }
  if ( num_zero > 0 ) {
    mtcp_output(MTCP_INFO,"writefile: wrote %d of %d NULL bytes...\n",num_zero,size);
  }
  return size;
}

HIDDEN(size_t) m_fcopy(int to_fd, int from_fd, size_t len) {
  char buff[1024];
  long rem = len%sizeof(buff);
  for(size_t i=0, n=len/sizeof(buff); i<n; ++i) {
    mtcp_sys_read(from_fd,buff,sizeof(buff));
    mtcp_sys_write(to_fd,buff,sizeof(buff));
  }
  if ( 0 != rem ) {
    char p;
    for(size_t i=0, n=rem; i<n;++i) {
      mtcp_sys_read(from_fd,&p,sizeof(p));
      mtcp_sys_write(to_fd,&p,sizeof(p));
    }
  }
  return len;
}

HIDDEN(size_t) m_fskip(int fd, size_t len) {
  long c, rem=len%sizeof(c);
  for(size_t i=0, n=len/sizeof(c); i<n;++i)
    mtcp_sys_read(fd,&c,sizeof(c));
  if ( 0 != rem ) {
    char p;
    for(size_t i=0, n=rem; i<n;++i)
      mtcp_sys_read(fd,&p,sizeof(p));
  }
  return len;
}

HIDDEN(size_t) m_fread(int fd, void* t, size_t len) {
  return mtcp_sys_read(fd,t,len);
}

HIDDEN(size_t) m_writeset(int fd, unsigned char pattern, size_t siz) {
  for(size_t len=0; len<siz; ++len)
    mtcp_sys_write(fd,&pattern,1);
  return siz;
}

HIDDEN(int) readMarker(int fd, Marker pattern) {
  Marker f(0);
  int rc = mtcp_sys_read(fd,&f,sizeof(Marker));
  if ( rc != sizeof(Marker) ) {
    mtcp_output(MTCP_ERROR,"fd:%d, FAILED to read marker..... rc=%d\n",fd,rc);
  }
  return checkMarker(&f,pattern);
}

HIDDEN(int) readInt(int fd, void* value) {
  mtcp_sys_read(fd,value,sizeof(int));
  return sizeof(int);
}

HIDDEN(int) readLong(int fd, void* value) {
  mtcp_sys_read(fd,value,sizeof(long));
  return sizeof(long);
}

HIDDEN(int) checkMarker(const void* addr, Marker pattern) {
  Marker f(*(Marker*)addr);
  if ( f != pattern ) {
    char *a=(char*)&pattern, *b=(char*)&f;
    mtcp_output(MTCP_ERROR,"ERROR Inconsistent item marker: Seen:%X '%c%c%c%c' Expected:%X '%c%c%c%c'!\n",
		f,b[0],b[1],b[2],b[3],pattern,a[0],a[1],a[2],a[3]);
    return -0xFEED;
  }
  return sizeof(Marker);
}
#ifdef CHECKPOINTING_HAS_NAMESPACE
}
#endif
