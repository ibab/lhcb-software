#ifndef CHECKPOINTING_FILEDSC_H
#define CHECKPOINTING_FILEDSC_H

#define _ALIGN(x) x __attribute__((__packed__))

#include "Checkpointing/Namespace.h"
#include <climits>
#include <cstdlib>
#include <sys/stat.h>

/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE  {

  /** @class FileDesc
   *
   * @author  M.Frank
   * @version 1.0
   */
  _ALIGN(class) FileDesc {
  public:
    struct stat statbuf;
    int    name_len;
    int    fd;
    char   istmp;
    char   isdel;
    char   hasData;
    char   _pad;
    long   offset;
    char   name[PATH_MAX];

    /// Length of the data structure
    long length() const      { return sizeof(FileDesc)-sizeof(name)+name_len+1; }
    /// Length of the file data
    long dataLength() const  { return hasData ? statbuf.st_size : 0;            }
    /// Address to the data section of thsi file (if any)
    const char* data() const { return name+name_len+1;                          }
    /// Load the proper data structures and initialize the properties from the fs
    int setup(int fdnum);
    /// Print the data to standard output
    void print(int lvl) const;

    /// Reopen file and attach it to the proper file descriptor
    int reopen();
    /// Write descriptor and possibly data to file identified by fileno
    int write(int fd)  const;
    /// StreamOut descriptor and possibly data to file identified by fileno
    int streamOut(void* address)  const;
    /// Read descriptor and possibly data from file identified by fileno
    int read(const void* addr, bool restore);
  };

  /** @class FileDesc
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileHandler   {  
  protected:
    /// Default destructor
    virtual ~FileHandler() {}
  public: 
    /// Handle single file entry
    virtual int handle(int fdnum)  const = 0;  
  };


  /** @class FilePrintHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FilePrintHandler : public FileHandler {
  public: 
    /// Standard constructor
    FilePrintHandler() {}
    /// Handle single file entry
    virtual int handle(int fdnum)  const;   
  };

  
  /** @class FileCountHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileCountHandler : public FileHandler {
  protected:
    mutable int  m_count;
  public: 
    /// Standard constructor
    FileCountHandler() : m_count(0) {}
    /// Handle single file entry
    virtual int handle(int /* fdnum */)  const {  ++m_count; return 1;}
    /// Access number of files counted
    int count() const { return m_count; }
  };
  
  /** @class MemCountFileHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class MemCountFileHandler : public FileHandler {
  protected:
    mutable long m_count;
  public: 
    /// Standard constructor
    MemCountFileHandler() : m_count(2*sizeof(Marker)) {}
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
    /// Access number of bytes necessary
    long count() const { return m_count; }
  };
  
  
  /** @class FileWriteHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileWriteHandler : public FileHandler {
  protected:
    int  m_fd;
    long m_offset;
    mutable long  m_bytes;
    mutable long  m_count;
    
  public:
    /// Standard constructor
    FileWriteHandler(int fd) : m_fd(fd), m_bytes(0), m_count(0) {}
    /// Access number of bytes written
    long bytes() const { return m_bytes; }
    /// Access number of bytes necessary
    long count() const { return m_count; }
    /// Start the write cycle
    FileWriteHandler& start();
    /// Stop and finish the write cycle
    int stop();
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
  };
  
  /** @class FileStreamOutHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileStreamOutHandler : public FileHandler {
  protected:
    mutable char *m_addr, *m_ptr;
    mutable long  m_count;
  public:
    /// Standard constructor
    FileStreamOutHandler(void* add);
    /// Access number of bytes written
    long bytes() const { return m_ptr-m_addr; }
    /// Access number of bytes necessary
    long count() const { return m_count; }
    /// Start the streamOut cycle
    FileStreamOutHandler& start();
    /// Stop and finish the streamOut cycle
    int stop();
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
  };

  /** @class FileReadHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileReadHandler : public FileHandler {
  protected:
    mutable char *m_addr, *m_ptr;
    mutable long  m_now;
    long          m_count;
    bool          m_restore;
  public: 
    /// Standard constructor
    FileReadHandler(const void* add, bool restore=false) 
    : m_addr((char*)add), m_ptr((char*)add), m_now(0), m_count(0), m_restore(restore) {}
    /// Access number of bytes necessary
    long count() const { return m_count; }
    /// Access number of bytes written
    long bytes() const { return m_ptr-m_addr; }
    /// Start the read cycle
    FileReadHandler& start();
    /// Stop and finish the read cycle
    int stop();
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
  };

  /** @class FileMemPrintHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileMemPrintHandler : public FileReadHandler {
  public: 
    /// Standard constructor
    FileMemPrintHandler(const void* add) :  FileReadHandler(add) {}
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
  };

  /** @class FileMapper
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileMapper : public FileReadHandler {
    bool m_print;
  public: 
    /// Standard constructor
    FileMapper(const void* add, bool prt, bool restore) 
    :  FileReadHandler(add, restore), m_print(prt) {}
    /// Handle single file entry
    virtual int handle(int fdnum)  const;
  };

  /** @class FileMap
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileMap {
  public:

    class MapInfo {
    public:
      void*       addr;
      std::size_t len;
      MapInfo(void* a, std::size_t s) : addr(a), len(s) {}
    };

    /// Default constructor
    FileMap() {}
    /// Default destructor
    ~FileMap() {}
    
    /// Count the number of files in the proc file system known to this process
    int count();
    /// Count the necessary amount of contiguous memory to save all data 
    long memoryCount();

    /// Map anonymous section to store information of size "len"
    MapInfo map(std::size_t len);
    /// Unmap anonymous global section identified by the MapInfo data
    int unmap(MapInfo& info);

    /// Scan the files in the proc file system and invoke the handler for each file.
    int scan(const FileHandler& handler)   const;
    /// Collect the file information to a contiguous memory block

    /// Read the dump file and invoke the handler for each file item read.
    int analyze(const FileHandler& handler);
  };
}

#undef _ALIGN

#endif  // CHECKPOINTING_FILEDSC_H
