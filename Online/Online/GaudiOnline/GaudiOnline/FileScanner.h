// $Id: HltBufferedIOReader.h,v 1.12 2009-12-02 10:32:35 apuignav Exp $
//====================================================================
//  FileScanner.h
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Author     : M.Frank
//====================================================================
#ifndef GAUDIONLINE_FILESCANNER_H
#define GAUDIONLINE_FILESCANNER_H

// Framework includes
#include "MBM/Client.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>
#include <set>

/*
 *   LHCb namespace declaration
 */
namespace LHCb   {

  /** Class definition of FileScanner.
   *
   *  @author Markus Frank
   *
   *  @version 1.0
   */
  class FileScanner  {
  protected:
    /// Property: Data directory name
    std::string              m_directory;
    /// Property: Required file prefix used to select files
    std::string              m_filePrefix;
    /// Property: Path to the file containing broken nodes, where no reading should happen
    std::string              m_brokenHostsFile;
    /// Property: List of runs to be processed (as strings!)
    std::vector<std::string> m_allowedRuns;
    /// List of files to process
    std::set<std::string>    m_files;
    /// Current file name
    std::string              m_current;
    /// Error string message
    mutable std::string      m_comment;

    /// Flag to indicate if files should be deleted
    bool                     m_deleteFiles;

  public:
    enum {
      EVENT_OK = 1,
      NO_FILES_LEFT = 2,
      END_OF_DATA = 4,
      BAD_FILE_DESC = 6
    };
    /// Standard Constructor
    FileScanner();
    /// Standard Destructor
    virtual ~FileScanner();

    /// Allocate space when reading events
    virtual MBM::EventDesc& allocateSpace(size_t buffer_size) = 0;
    /// Scan directory for matching files
    size_t scanFiles();
    /// Open a new data file
    int openFile();
    /// Save remainder of currently read file
    int safeRestOfFile(int file_handle);
    /// Read broken hosts file and check if this node should process files
    int isBrokenHost()  const;
    /// Read one record and filel the proper structures
    int readEvent(int file_handle, void* baseAddr);

  };
} // End namespace LHCb
#endif // GAUDIONLINE_FILESCANNER_H
