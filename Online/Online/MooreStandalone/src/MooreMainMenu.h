// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: GUI to run Moore standalone on a single node.

//  Author     : M.Frank
//====================================================================
// $Header: $
#ifndef MOORESTANDALONE_MOOREMAINMENU_H
#define MOORESTANDALONE_MOOREMAINMENU_H

// Framework include files
#include "CPP/Interactor.h"
#include "MBM/Monitor.h"

// C/C++ include files
#include <cstring>

// Forward declarations
class ISvcLocator;
class IocSensor;
class Event;


/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class UpiMooreMonitor;
  class MessageLogger;

  /** @class MooreMainMenu MooreMainMenu.h MooresStandalone/MooreMainMenu....h
   *
   *   Class definition of MooreMainMenu.
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class MooreMainMenu : public Interactor  {
  public:
    /// Thread descriptor of the message pump
    lib_rtl_thread_t m_exec;
    char  m_logFile[1024];
    char  m_resFile[1024];
    char  m_myOpts[1024];
    int   m_id;
    FILE* m_log;
    MBM::Monitor*    m_mbmMon;
    UpiMooreMonitor* m_evtMon;
    UpiMooreMonitor* m_rateMon;
    UpiMooreMonitor* m_allMon;

    IocSensor*       m_ioc;
    ISvcLocator*     m_svcLocator;
    MessageLogger*   m_logger;
    bool             m_terminate;

    /// Default constructor
    MooreMainMenu(const std::string& opts);
    /// Default destructor
    virtual ~MooreMainMenu();
    void handleUPI(int cmd, int par, int index);

    /// Interactor callback to handle interrupts
    virtual void handle (const Event& event);
    /// Destroy windows, but do not destruct object
    void destroy();
    /// Close all monitor menues in one go....
    void closeMenus();
    /// Handle termination sequence
    void terminate();
    /// Run Moore application in thread
    int runMoore(const std::string& opts);
    /// Start test execution
    bool startExecution();
    /// End test execution
    bool finishExecution();
    /// Enable commands after test finished/after test got interrupted.
    bool enableTesting();
    /// Send interrupt to testing service
    bool sendInterrupt(const std::string& service, int code, void* param=0);
    /// Handle ON/OFF of monitoring displays
    bool handleMonitor(UpiMooreMonitor*& monitor, int type);
    /// Reconnect monitor updates after test finished
    bool reconnectMonitor(UpiMooreMonitor* monitor);
    /// Write asynchronous output message
    void write_message(const char* text);
    /// Output and printing helper function
    static size_t write_message(void* param, int level, const char* fmt, va_list args);
    /// Static thread entry point
    static int  run(void* param);
    /// Backspace callback
    static void backSpaceCallBack (int menu,int /* cmd */,int par,const void* param);
  };
}
#endif // MOORESTANDALONE_MOOREMAINMENU_H
