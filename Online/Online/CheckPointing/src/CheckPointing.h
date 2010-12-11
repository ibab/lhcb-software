#ifndef CHECKPOINTING_CHECKPOINTING_H
#define CHECKPOINTING_CHECKPOINTING_H

#include <cstdlib>
namespace CheckPointing {
  typedef unsigned char* Pointer;
  typedef const unsigned char* const_Pointer;
  typedef unsigned int Marker;
  typedef unsigned long VA; /* VA = virtual address */


  size_t m_strcpy(char* t, const char* s);
  size_t m_strcat(char* t, const char* s);
  size_t m_memcpy(void* t, const void* s, size_t len);
  const char* m_strfind(const char* s, const char* pattern);
  const char* m_chrfind(const char* s, const char pattern);
  char*       m_chrfind(char* s, const char pattern);
  size_t      m_strlen(const char* s);

  //int writeMarker(int fd);
  //int checkMarker(int fd);
  //int writeMarker(int fd, Marker pattern);
  //int checkMarker(int fd, Marker pattern);

  int saveMarker(void* addr, Marker pattern);
  int checkMarker(const void* addr, Marker pattern);


  int saveInt(void* addr, int val);
  int getInt(const void* addr, void* value);

  int saveLong(void* addr, long val);
  int getLong(const void* addr, void* value);

  template <class A, class B> size_t addr_diff(const A* a, const B* b) { return (((const char*)a)-((const char*)b)); }
}

#ifndef CHECKPOINTING_OUTPUT
#define CHECKPOINTING_OUTPUT
enum MTCP_OUTPUT_LEVEL  { 
  MTCP_DEBUG   = 1,
  MTCP_INFO    = 2,
  MTCP_WARNING = 3,
  MTCP_ERROR   = 4,
  MTCP_FATAL   = 5
};
void mtcp_output(int lvl, const char* fmt,...);
void mtcp_set_debug_level(int lvl);
int mtcp_get_debug_level();

#endif // CHECKPOINTING_OUTPUT

#endif  // CHECKPOINTING_CHECKPOINTING_H
