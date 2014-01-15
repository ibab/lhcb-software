#ifndef CHECKPOINTING_MEMMAPS_H
#define CHECKPOINTING_MEMMAPS_H

#include "Checkpointing/Namespace.h"
#include <climits>
#define _ALIGN(x) x __attribute__((__packed__))


/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE {

  // Forward declarations
  class Area;
  class AreaHandler;
  class AreaCollector;
  class AreaMapper;
  class MemMaps;

  /** @class Area
   *
   * @author  M.Frank
   * @version 1.0
   */
  _ALIGN(class) Area {
  public:
    /// Length of the file name
    int     name_len;
    /// File protection bytes
    char    prot[4];
    /// Low address of the memory map
    unsigned long low;
    /// High address of the memory map
    unsigned long high;
    /// Section size
    long    size;
    /// Offset for file mapping
    long    offset;
    /// File name for this memory area
    char    name[PATH_MAX];

    /// Read memory area descriptor and data from file given by the file handle
    int  read(const void* address, const AreaHandler& handler);
    /// Write memory area descriptor and data to file given by the file handle
    int  write(int fd, bool force_nulls=false) const;
    /// Stream out memory area descriptor and to memory
    int  streamOut(void* address, bool force_nulls=false) const;
    /// Access protection flags for this memory area
    int  protection() const;
    /// Access mmap flags for this memory area
    int  mapFlags() const;
    /// Simple check if the memory area is mapped to a file
    int  isFile() const;
  };

  /** @class AreaHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaHandler {
  public:
    int (*f_map)(const AreaHandler* self,const Area& a, const unsigned char* data, int data_len);
    int (*f_handle)(const AreaHandler* self,int which, const Area& a);
    /// Initializing constructor
    AreaHandler() : f_map(0), f_handle(0) {}
  };

  /** @class AreaBaseHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaBaseHandler : public AreaHandler {
  protected:
    long m_bytes;
    long m_count;
    int  updateCounts(const Area& a);
  public:
    /// Initializing constructor
    AreaBaseHandler();
    long bytes() const {  return m_bytes; }
    long count() const {  return m_count; }
    static int do_map(const Area&, const unsigned char*, int data_len) 
    {      return data_len;    }
    static int mapArea(const Area& a, const unsigned char* data, int data_len);
    /// Handler callback
    int handle(int which, const Area& a);
  };

  /** @class AreaPrintHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaPrintHandler : public AreaBaseHandler {
  public:
    /// Initializing constructor
    AreaPrintHandler();
    /// Handler callback
    int handle(int which, const Area& a);
  };

  /** @class AreaLibHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaLibHandler : public AreaBaseHandler {
    int    m_fd;
    int    m_len;
    int    m_numLibs;
    char** m_libs;
  public:
    /// Initializing constructor
    AreaLibHandler(int fd);
    /// Default destructor
    ~AreaLibHandler();
    /// Handler callback
    int handle(int which, const Area& a);
    int numLibs() const {  return m_numLibs; }
    void release();
  };

  /** @class AreaInfoHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaInfoHandler : public AreaBaseHandler {
  public:
    bool m_prev;
    char image[PATH_MAX];
    unsigned long stack[2];
    unsigned long vdso[2];
    unsigned long vsyscall[2];
    unsigned long imageAddr[2];
    unsigned long checkpointAddr[2];
    unsigned long highAddr;

  public:
    /// Initializing constructor
    AreaInfoHandler();
    /// Handler callback
    int handle(int which, const Area& a);
  };

  /** @class AreaWriteHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaWriteHandler : public AreaBaseHandler {
  protected:
    int m_fd;
  public:
    /// Initializing constructor
    AreaWriteHandler(int fd);
    /// Handler callback
    int  handle(int which, const Area& a);
    long bytesWritten() const { return m_bytes; }
  };

  /** @class AreaChkptWriteHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaChkptWriteHandler : public AreaWriteHandler {
  public:
    bool m_prev;
    /// Initializing constructor
    AreaChkptWriteHandler(int fd);
    /// Handler callback
    int handle(int which, const Area& a);
  };

  /** @class AreaMapper
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaMapper : public AreaBaseHandler   {
  public:
    /// Initializing constructor
    AreaMapper();
    /// Handler callback
    int handle(int, const Area& ) { return 1; }
    static int do_map(const Area& a, const unsigned char* data, int data_len) {
      return mapArea(a,data,data_len);
    }
  };

  /** @class MemMaps
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MemMaps {
    friend class AreaCollector;
  public:
    /// Default constructor
    MemMaps() {}
    /// Default destructor
    ~MemMaps() {}
    /// Number of memory mapped areas/files
    int numLines()  const;
    /// Collect in process information about the memory mappings
    int scan(AreaHandler& handler);
    /// Dump descriptor information about the memory mappings
    void dump();

    /// Write descriptor information and data from the memory mappings to address
    long write(int fd);
    /// Read descriptor information and data from the memory mappings
    long read(const void* address,AreaHandler& hdlr);
  };

}  // End namespace Checkpointing

#undef _ALIGN

#endif // CHECKPOINTING_MEMMAPS_H
