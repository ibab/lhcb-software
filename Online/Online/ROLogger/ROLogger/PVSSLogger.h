// $Id: PVSSLogger.h,v 1.1 2008-09-26 16:06:09 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/PVSSLogger.h,v 1.1 2008-09-26 16:06:09 frankb Exp $
#ifndef ROLOGGER_PVSSLOGGER_H
#define ROLOGGER_PVSSLOGGER_H

// C++ include files
#include <map>
#include <string>

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  class LogFileEntry;

  /**@class PVSSLogger PVSSLogger.h ROLogger/PVSSLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class PVSSLogger {
    /// Definition of file map
    typedef std::map<std::string,LogFileEntry*> Files;
    /// Directory path
    std::string m_name;
    /// Node name
    std::string m_node;
    /// Process name
    std::string m_process;
    /// Maximum time difference to changes to log files in order to be taken into account
    time_t      m_maxFileChange;
    /// Map of known log files
    Files       m_files;
    /// FIFO file descriptor
    int m_fifo;
    /// Max. number of seconds before checking for new files
    int m_checkLimit;
    /// Time stamp of last file existence check.
    time_t m_lastChecked;

    std::string _prefix(const std::string& tag="INFO") const;
  public:

    /// Initializing constructor
    PVSSLogger(const std::string& nam);
    /// Default destructor
    virtual ~PVSSLogger(); 

    /// Connect output fifo
    int connect(const std::string& fifo);
    /// Open all log files
    int open(bool begin,bool prt);
    /// Close all log files
    int close();
    /// Read directory content
    int read(bool prt);
    /// Print content of log files as they come....
    int publish();
    /// Check directory for new log files
    void checkFiles();
    /// Print message to output device
    virtual int print(const std::string& msg);
    /// Dump directory content
    void dump();
    /// Access directory name
    const std::string& name() const { return m_name;  }
    /// Access file map
    const Files& files() const      { return m_files; }
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_PVSSLOGGER_H */
