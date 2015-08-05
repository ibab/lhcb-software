// $Id: FileLogger.h,v 1.4 2010-03-28 06:08:20 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/FileLogger.h,v 1.4 2010-03-28 06:08:20 frankb Exp $
#ifndef ROLOGGER_FILELOGGER_H
#define ROLOGGER_FILELOGGER_H

// Framework include files
#include "ROLogger/MessageLogger.h"

// Forward declarations
namespace UPI { class ReallyClose; }

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations
  class PartitionListener;

  /**@class FileLogger FileLogger.h ROLogger/FileLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class FileLogger : public MessageLogger  {
    typedef std::vector<std::string> Farms;

  protected:
    /// Menu id
    int                m_id;
    /// Reference to partition listener
    PartitionListener* m_listener;
    /// Output directory
    std::string        m_outdir;
    /// Flag to use time-stamp for file name
    bool               m_useTimeForName;
    /// Parameter buffer
    char               m_msgSeverity[16];
    /// Buffer to store HT farm content
    Farms              m_farms;
    /// Prefix for date check of messages
    int                m_date;
    /// Menu to shutdown application
    UPI::ReallyClose*  m_quit;
    /// Flag to dump messages to UPI
    bool               m_connected;
    /// Use RFC3339 https://www.ietf.org/rfc/rfc3339.txt for time-format
    bool               m_timerfc3339;
    /// Open new output file
    FILE* openOutput();
    /// Print single message retrieved from error logger
    virtual void printMessage(const char* msg, bool crlf=true);
    // Format message according to RFC 3339 
    // returns a pointer to a string with the re-formatted message
    // ownership is passed, this string must be free-ed by the caller
    char* formatTimeRFC3339(const char* msg);
  public:
    /// Standard constructor with object setup through parameters
    FileLogger(int argc, char** argv);
    /// Standard destructor
    virtual ~FileLogger();
    /// Interactor callback handler
    virtual void handle(const Event& ev);
    //
    int m_thisYear;
  };
}
#endif /* ROLOGGER_FILELOGGER_H */
