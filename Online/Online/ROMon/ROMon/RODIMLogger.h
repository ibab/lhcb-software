// $Id: RODIMLogger.h,v 1.1 2008-04-28 15:16:48 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RODIMLogger.h,v 1.1 2008-04-28 15:16:48 frankb Exp $
#ifndef ROMON_RODIMLOGGER_H
#define ROMON_RODIMLOGGER_H

// C++ include files
#include <map>
#include <vector>
#include <string>
#include <cstdio>


/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  // Forward declarations

  /**@class RODIMLogger RODIMLogger.h ROMon/RODIMLogger.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class RODIMLogger {
  protected:
    int m_service;
    /// Flag to indicate output to xterm using colors
    bool m_colors;
    struct Entry {
      time_t       created;
      int          id;
      std::string  name;
      RODIMLogger* self;
      Entry() {}
    };
    typedef std::map<std::string,Entry*> Services;
    Services m_infos;

  public:
    /// Standard constructor with object setup through parameters
    RODIMLogger(int argc, char** argv);
    /// Standard destructor
    virtual ~RODIMLogger();
    /// Print header information before starting output
    void printHeader(FILE* fp, const std::string& title);
    /// Print multi-line header information before starting output
    void printHeader(FILE* fp, const std::vector<std::string>& titles);
    /// Print single message retrieved from error logger
    void printMessage(FILE* fp, const char* msg, bool crlf=true);
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
#endif /* ROMON_RODIMLOGGER_H */
