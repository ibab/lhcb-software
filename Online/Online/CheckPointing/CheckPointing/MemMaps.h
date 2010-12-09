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
    long    low;
    /// High address of the memory map
    long    high;
    /// Section size
    long    size;
    /// Offset for file mapping
    long    offset;
    /// File name for this memory area
    char    name[PATH_MAX];
    /// Read memory area descriptor and data from file given by the file handle
    int  read(int fd);
    /// Write memory area descriptor and data to file given by the file handle
    int  write(int fd);
    /// Print area descriptor to standard output
    void print() const;
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
    virtual void handle(int which, const Area& a) const = 0;
  };

  /** @class AreaCollector
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaCollector : public AreaHandler   {
  protected:
    MemMaps* m_maps;
  public:
    AreaCollector(MemMaps* maps) : m_maps(maps) {}
    virtual void handle(int which, const Area& a) const;
  };

  /** @class AreaMapper
   *
   * @author  M.Frank
   * @version 1.0
   */
  class AreaMapper : public AreaHandler   {
  public:
    AreaMapper()  {}
    virtual void handle(int which, const Area& a) const;
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
    int collect();
    /// Dump descriptor information about the memory mappings
    void dump();
    /// Write descriptor information about the memory mappings to file
    int writeDescriptors(int fd);

    /// Write descriptor information and data from the memory mappings to file
    long write(int fd);
    /// Read descriptor information and data from the memory mappings from file
    long read(int fd);
    /// Read descriptor information and data from the memory mappings from file
    long read(int fd, const AreaHandler& hdlr);
  };

}  // End namespace CheckPointing

#undef _ALIGN

#endif // CHECKPOINTING_MEMMAPS_H
