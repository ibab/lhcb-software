#ifndef ONLINEKERNEL_FS_FILESYSTEM_H
#define ONLINEKERNEL_FS_FILESYSTEM_H

// $ID:
// C++ include files
#include <vector>
#include <string>

/*
 *   FS namespace declaration
 */
namespace RTL {
  /** @class FileSystem FileSystem.h FS/FileSystem.h
   *
   *  @author  N. Neufeld
   *  @version 1.0
   */
  class FileSystem {
  public: // Niko doesn't believe in hiding
    /// Filesystem name 
    std::string m_name;
    /// Standard constructor
    FileSystem();
    /// Constructor to attach to a filesystem
    FileSystem(const std::string& name);
    /// Constructor to start new process
    virtual ~FileSystem();
    /// set filesystem name
    const std::string& name() const {  return m_name; }
    /// Access process name
    void setName(const std::string& val) {  m_name = val; }
    /// number of bytes usable for normal users. -1 if call fails
    virtual long freeBytes();
    /// format the filesystem
    virtual int format();
  };
} // End namespace RTL
#endif  // ONLINEKERNEL_RTL_PROCESS_H
