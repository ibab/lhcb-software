#ifndef CHECKPOINTING_CHECKPOINTING_H
#define CHECKPOINTING_CHECKPOINTING_H

#include <cstdlib>
namespace CheckPointing {
  size_t m_strcpy(char* t, const char* s);
  size_t m_strcat(char* t, const char* s);

  int writeMarker(int fd);
  int checkMarker(int fd);
  int writeMarker(int fd, unsigned int pattern);
  int checkMarker(int fd, unsigned int pattern);
}

#endif  // CHECKPOINTING_CHECKPOINTING_H
