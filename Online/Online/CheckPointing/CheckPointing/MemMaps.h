#ifndef CHECKPOINTING_MEMMAPS_H
#define CHECKPOINTING_MEMMAPS_H

#include <climits>
#define _ALIGN(x) x __attribute__((__packed__))


/*
 * CheckPointing namespace declaration
 */
namespace CheckPointing {

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
  _ALIGN(struct) Area {
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
    int  read(const void* address);
    /// Write memory area descriptor and data to file given by the file handle
    int  write(void* address) const;
    /// Print area descriptor to standard output
    void print(const char* opt="") const;
    /// Access protection flags for this memory area
    int  protection() const;
    /// Access mmap flags for this memory area
    int  mapFlags() const;
    /// Simple check if the memory area is mapped to a file
    int  isFile() const;
    /// Returns the full spze requirement to save this memory area
    int  length() const;
  };

  /** @class AreaHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaHandler {
  public:
    virtual int handle(int which, const Area& a) = 0;
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
    long m_space;
    int  updateCounts(const Area& a);
  public:
    AreaBaseHandler();
    long bytes() const {  return m_bytes; }
    long space() const {  return m_space; }
    long count() const {  return m_count; }
    virtual int handle(int which, const Area& a);
  };

  /** @class AreaPrintHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaPrintHandler : public AreaBaseHandler {
  public:
    AreaPrintHandler() {}
    virtual int handle(int which, const Area& a);
  };

  /** @class AreaInfoHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaInfoHandler : public AreaBaseHandler {
  public:
    unsigned long stack[2];
    unsigned long vdso[2];
    unsigned long vsyscall[2];
    unsigned long highAddr;

  public:
    AreaInfoHandler();
    virtual int handle(int which, const Area& a);
  };

  /** @class AreaWriteHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaWriteHandler : public AreaBaseHandler {
    mutable char *m_addr, *m_ptr;
  public:
    AreaWriteHandler(void* add) : m_addr((char*)add), m_ptr((char*)add) {}
    virtual int handle(int which, const Area& a);
    long bytesWritten() const { return m_ptr-m_addr; }
  };


  /** @class AreaMapper
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaMapper : public AreaHandler   {
  public:
    AreaMapper()  {}
    virtual int handle(int which, const Area& a);
  };

  /** @class MemMaps
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MemMaps {
    friend class AreaCollector;
    int   m_numArea;
    Area* m_areas;
  public:
    /// Default constructor
    MemMaps();
    /// Default destructor
    ~MemMaps();
    /// Number of memory mapped areas/files
    int numLines()  const;
    /// Collect in process information about the memory mappings
    int scan(AreaHandler& handler);
    /// Dump descriptor information about the memory mappings
    void dump();

    /// Write descriptor information and data from the memory mappings to address
    long write(void* address);
    /// Read descriptor information and data from the memory mappings
    long read(const void* address,AreaHandler& hdlr);

    /// Write descriptor information and data from the memory mappings to file
    long write(const char* file_name);
    /// Read descriptor information and data from the memory mappings from file
    long read(const char* file_name);
  };

}  // End namespace CheckPointing

#undef _ALIGN

#endif // CHECKPOINTING_MEMMAPS_H
