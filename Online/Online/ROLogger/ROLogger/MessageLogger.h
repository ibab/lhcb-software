// $Id: MessageLogger.h,v 1.12 2010-04-15 16:04:59 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/MessageLogger.h,v 1.12 2010-04-15 16:04:59 frankb Exp $
#ifndef ROLOGGER_MESSAGELOGGER_H
#define ROLOGGER_MESSAGELOGGER_H

// Framework include files
#include "CPP/Interactor.h"
#include "ROLogger/Filter.h"

// C++ include files
#include <map>
#include <list>
#include <vector>

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  // Forward declarations

  /**@class MessageLogger MessageLogger.h ROLogger/MessageLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class MessageLogger : public Interactor  {
  protected:
    /**@struct Entry 
      *  Message logger service entry.
      */
    struct Entry {
      time_t       created;
      int          id;
      std::string  name;
      MessageLogger* self;
      Entry() {}
    };
    typedef std::map<std::string,Entry*> Services;
    typedef std::list<std::string>       History;
    typedef std::vector<Filter>          Filters;

    /// DIM service identifier
    int               m_service;
    /// Default message severity to display
    int               m_severity;
    /// Flag to indicate output to xterm using colors
    bool              m_colors;
    /// Flag to indicate output to xterm 
    bool              m_display;
    /// Filter array
    Filters           m_filters;
    /// Connected DIM services
    Services          m_infos;
    /// History buffer
    History           m_history;
    /// Size of history record
    size_t            m_historySize;
    /// Output device
    FILE*             m_output;
    /// Iterator in history circular buffer
    History::iterator m_histIter;
    /// Flag if iterator wrapped
    bool              m_wrapped;
    /// Flag to indicate debugging
    bool              m_debug;
    /// Flag to indicate if alarm messages should be processed rather than standard output
    bool              m_alarm;
    /// Storage slice name
    std::string       m_storage;
    /// Monitoring slice name
    std::string       m_monitoring;
    /// Reconstruction slice name
    std::string       m_reconstruction;
    /// Partition name
    std::string       m_partition;
    /// Message counter by type
    std::vector<int>  m_numMsg;
    /// Currently known run number
    int               m_runNo;
    /// Flag to print the run number (default on)
    int               m_printRunNo;

    /// Print summary of history records from stored memory
    void summarizeHistory();
    /// Print history records from stored memory
    void printHistory(const std::string& pattern);
    /// Update history records and flush obsolete records.
    void updateHistory(const char* msg);
    /// Check filters if this message should be printed....
    bool checkFilters(const char* msg) const;
    /// Get summary line from running messages
    std::string getSummary();
    /// Set new run number in callback
    void newRunNumber(const std::string& run);

    /// Handle DIM message
    virtual void handleMessage(const char* msg);

  public:
    /// Standard constructor with object setup through parameters
    MessageLogger(int argc, char** argv);
    /// Standard constructor, with initialisation
    MessageLogger(int sev, bool display, bool colors);
    /// Standard destructor
    virtual ~MessageLogger();

    /// Interactor callback handler
    virtual void handle(const Event& ev);

    /// Print single message retrieved from error logger
    virtual void printMessage(const char* msg, bool crlf=true);

    /// Print alarm message with special format
    virtual void printAlarm(const char* msg, bool crlf=true);

    /// Execute command from message
    void execute(const std::string& cmd);

    /// Set partition name to filter PVSS names
    void setPartition(const std::string& partition);
    /// Set message severity level for display
    void setMessageSeverity(const std::string& severity);
    /// Print header information before starting output
    void printHeader(const std::string& title, bool with_guards=true);
    /// Print multi-line header information before starting output
    void printHeader(const std::vector<std::string>& titles, bool with_guards=true);
    /// Retrieve message severity from DIM logger message
    int msgSeverity(const char* msg);
    /// Cleanup service stack
    void cleanupServices(const std::string& match="");
    /// Cleanup service entry
    void cleanupService(Entry* e);
    /// Remove unconditionally all connected services
    void removeAllServices();
    /// Load filters from string represntation
    void loadFilters(const std::string& s);

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
#endif /* ROLOGGER_MESSAGELOGGER_H */
