#ifndef CHECKPOINTING_MMAP_H
#define CHECKPOINTING_MMAP_H

#include "Checkpointing/Namespace.h"
/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE {

  /** @class MMap
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MMap {
  protected:
    int m_fd;
    char* m_addr;
    long  m_length;
  public:
    MMap() : m_fd(-1), m_addr(0), m_length(0) {     }
    void* address() const { return m_addr;          }
    void* low()  const    { return m_addr;          }
    void* high() const    { return m_addr+m_length; }
    long  length() const  { return m_length;        }
    int   fd() const      { return m_fd;            }
    int create(const char* file_name, long len=-1);
    int open(const char* file_name);
    int close();
  };
}

#endif // CHECKPOINTING_MMAP_H
