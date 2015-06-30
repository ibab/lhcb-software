#ifndef CHECKPOINTING_CHECKPOINTING_H
#define CHECKPOINTING_CHECKPOINTING_H

#include <cstdlib>
#include "Checkpointing/Namespace.h"

#ifdef CHECKPOINTING_HAS_NAMESPACE
namespace CHECKPOINTING_NAMESPACE  {

  typedef unsigned char* Pointer;
  typedef const unsigned char* const_Pointer;

  HIDDEN(int)    m_intcheck(const void* a, const void* b);
  HIDDEN(int)    m_longcheck(const void* a, const void* b);
  HIDDEN(int)    m_isspace(char s);
  HIDDEN(int)    m_strcmp(const char* t, const char* s);
  HIDDEN(int)    m_strncmp(const char* t, const char* s, size_t len);
  HIDDEN(size_t) m_strcpy(char* t, const char* s);
  HIDDEN(size_t) m_strncpy(char* t, const char* s, size_t len);
  HIDDEN(size_t) m_strcat(char* t, const char* s);

  HIDDEN(size_t) m_memcpy(void* t, const void* s, size_t len);
  HIDDEN(size_t) m_fread(int fd, void* t, size_t len);
  HIDDEN(size_t) m_fcopy(int to_fd, int from_fd, size_t len);
  HIDDEN(size_t) m_fskip(int fd, size_t len);

  HIDDEN(size_t)      m_memset(void* t, unsigned char pattern, size_t len);
  HIDDEN(const char*) m_strfind(const char* s, const char* pattern);
  HIDDEN(const char*) m_chrfind(const char* s, const char pattern);
  HIDDEN(char*)       m_chrfind(char* s, const char pattern);
  HIDDEN(size_t)      m_strlen(const char* s);

  HIDDEN(size_t) m_writemem(int fd, const void* s, size_t len);
  HIDDEN(size_t) m_writeset(int fd, unsigned char pattern, size_t len);

  HIDDEN(int) readMarker(int fd, ::Checkpointing::Marker pattern);
  HIDDEN(int) writeMarker(int fd, ::Checkpointing::Marker pattern);
  HIDDEN(int) saveMarker(void* addr, ::Checkpointing::Marker pattern);
  HIDDEN(int) checkMarker(const void* addr, ::Checkpointing::Marker pattern);


  HIDDEN(int) writeInt(int fd,int value);
  HIDDEN(int) saveInt(void* addr, int val);
  HIDDEN(int) readInt(int fd, void* value);
  HIDDEN(int) getInt(const void* addr, void* value);

  HIDDEN(int) writeLong(int fd,long value);
  HIDDEN(int) saveLong(void* addr, long val);
  HIDDEN(int) readLong(int fd, void* value);
  HIDDEN(int) getLong(const void* addr, void* value);

  template <class A, class B> static inline 
    size_t addr_diff(const A* a, const B* b) { return (((const char*)a)-((const char*)b)); }

}
#else
  typedef unsigned char* Pointer;
  typedef const unsigned char* const_Pointer;
  template <class A, class B> static inline 
    size_t addr_diff(const A* a, const B* b) { return (((const char*)a)-((const char*)b)); }
#endif

#ifndef CHECKPOINTING_OUTPUT
#define CHECKPOINTING_OUTPUT
void mtcp_output(int lvl, const char* fmt,...);
void mtcp_set_debug_level(int lvl);
int  mtcp_get_debug_level();
#ifndef MTCP_ERRNO
//#define MTCP_ERRNO
//extern int* __mtcp_sys_errno();
//#define mtcp_sys_errno (__mtcp_sys_errno())
#endif

#endif // CHECKPOINTING_OUTPUT

#define DefineMarker(x,y) static const ::Checkpointing::Marker x = *(::Checkpointing::Marker*)y

#endif  // CHECKPOINTING_CHECKPOINTING_H
