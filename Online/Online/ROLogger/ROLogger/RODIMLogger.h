// $Id: RODIMLogger.h,v 1.2 2008-05-07 16:22:21 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/RODIMLogger.h,v 1.2 2008-05-07 16:22:21 frankb Exp $
#ifndef ROLOGGER_RODIMLOGGER_H
#define ROLOGGER_RODIMLOGGER_H

// Framework include files
#include "CPP/Interactor.h"

// C++ include files
#include <map>
#include <list>
#include <vector>
#include <string>
#include <cstdio>


/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations

  /**@class RODIMLogger RODIMLogger.h ROLogger/RODIMLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class RODIMLogger : public Interactor  {
  protected:
    struct Entry {
      time_t       created;
      int          id;
      std::string  name;
      RODIMLogger* self;
      Entry() {}
    };
    typedef std::map<std::string,Entry*> Services;
    typedef std::list<std::string> History;

    /// DIM service identifier
    int m_service;
    /// Flag to indicate output to xterm using colors
    bool m_colors;
    /// Flag to indicate output to xterm 
    bool m_display;
    /// Connected DIM services
    Services m_infos;
    /// History buffer
    History  m_history;
    /// Size of history record
    size_t   m_historySize;
    /// Output device
    FILE*    m_output;
    /// Flag to indicate the usage of UPI
    bool     m_upi;
    /// Menu id
    int      m_id;
    /// Clear all history content
    void clearHistory();
    /// Print summary of history records from stored memory
    void summarizeHistory();

    /// Create UPI Menu
    void createMenu();
    /// Delete UPI Menu
    void deleteMenu();
    /// Print history records from stored memory
    void printHistory(const std::string& pattern);
    /// Update history content
    void updateHistory(const char* msg);
    /// Log internal message
    void logMessage(const char* fmt, ...);

  public:
    /// Standard constructor with object setup through parameters
    RODIMLogger(int argc, char** argv);
    /// Standard destructor
    virtual ~RODIMLogger();

    /// Interactor callback handler
    virtual void handle(const Event& ev);


    /// Print header information before starting output
    void printHeader(const std::string& title);
    /// Print multi-line header information before starting output
    void printHeader(const std::vector<std::string>& titles);
    /// Print single message retrieved from error logger
    void printMessage(const char* msg, bool crlf=true);
    /// Retrieve message severity from DIM logger message
    int msgSeverity(const char* msg);
    /// Cleanup service stack
    void cleanupServices(const std::string& match="");
    /// Cleanup service entry
    void cleanupService(Entry* e);
    /// Remove unconditionally all connected services
    void removeAllServices();

    /// DIM command service callback
    void handleMessages(const char* nodes, const char* end);
    /// DIM command service callback
    void handleRemoveMessages(const char* nodes, const char* end);

    /// DIM command service callback
    void handleHistory(const std::string& nam);

    /// Help printout for RTL CLI
    static void help_fun();
    /// Dim Info callback overload to process messages
    static void historyInfoHandler(void* tag, void* address, int* size);
    /// Dim Info callback overload to process messages
    static void messageInfoHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void requestHandler(void* tag, void* address, int* size);
  };
}
#endif /* ROLOGGER_RODIMLOGGER_H */
