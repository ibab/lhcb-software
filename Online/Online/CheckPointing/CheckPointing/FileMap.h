#ifndef CHECKPOINTING_FILEDSC_H
#define CHECKPOINTING_FILEDSC_H

#define _ALIGN(x) x __attribute__((__packed__))

#include <climits>
#include <sys/stat.h>

/*
 * CheckPointing namespace declaration
 */
namespace CheckPointing {

  /** @class FileDesc
   *
   * @author  M.Frank
   * @version 1.0
   */
  _ALIGN(struct) FileDesc {
    struct stat statbuf;
    int    name_len;
    int    fd;
    long   offset;
    char   name[PATH_MAX];
    char   istmp;
    char   isdel;
    char   hasData;
    char   _pad;
    void*  tmpbuf;

    /// Load the proper data structures and initialize the properties from the fs
    int setup(int fdnum);
    /// Get the data block behind this file. Assumes proper statbuf and fd!
    int getData(void* ptr, int len);
    /// Copy data block to output file descriptor. Assumes proper statbuf and fd!
    int copyData(int out);
    /// Print the data to standard output
    void print();

    /// Reopen file and attach it to the proper file descriptor
    int reopen();
    /// Write descriptor and possibly data to file identified by fileno
    int write(int out);
    /// Read descriptor and possibly data from file identified by fileno
    int read(int in, bool restore);
  };

  /** @class FileDesc
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileHandler   {  
  public: 
    /// Handle single file entry
    virtual void handle(int fdnum)  const = 0;  
  };


  /** @class FilePrintHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FilePrintHandler : public FileHandler {
  public: 
    /// Handle single file entry
    virtual void handle(int fdnum)  const;   
  };

  
  /** @class FileWriteHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileWriteHandler : public FileHandler {
    int m_fd;
    mutable int  m_count;
    mutable long m_bytes;
    mutable long m_offset;
  public:
    /// Standard constructor
    FileWriteHandler(int fd) : m_fd(fd), m_count(0), m_bytes(0), m_offset(0) {}
      /// Default destructor
      ~FileWriteHandler() { }
      /// Access number of bytes written
      long bytes() const { return m_bytes; }
      /// Start the write cycle
      int start();
      /// Stop and finish the write cycle
      int stop();
      /// Handle single file entry
      virtual void handle(int fdnum)  const;
  };
  
  /** @class FileReadHandler
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileReadHandler : public FileHandler {
    int m_fd;
    mutable int  m_count;
    mutable long m_bytes;
    mutable long m_offset;
  public: 
    /// Standard constructor
    FileReadHandler(int fd) : m_fd(fd), m_count(0), m_bytes(0), m_offset(0) {}
      /// Default destructor
      ~FileReadHandler() {}
      /// Start the read cycle
      int start();
      /// Stop and finish the read cycle
      int stop();
      /// Handle single file entry
      virtual void handle(int fdnum)  const;
  };

  /** @class FileMap
   *
   * @author  M.Frank
   * @version 1.0
   */
  class FileMap {
    /// The number of open files (only valid AFTER count or scan was called)
    int m_numFiles;
  public:
    /// Default constructor
    FileMap();
    /// Count the number of files in the proc file system known to this process
    int count();
    /// Scan the files in the proc file system and invoke the handler for each file.
    int scan(const FileHandler& handler);
    /// Read the dump file and invoke the handler for each file item read.
    int restore(int fd_in, const FileHandler& handler);
  };

}
#undef _ALIGN

#endif  // CHECKPOINTING_FILEDSC_H
