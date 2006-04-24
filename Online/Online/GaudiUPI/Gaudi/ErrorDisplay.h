#ifndef GAUDIUPI_ERRORDISPLAY_H
#define GAUDIUPI_ERRORDISPLAY_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/Gaudi/ErrorDisplay.h,v 1.1 2006-04-24 13:45:55 frankb Exp $

#include "GaudiOnline/DimErrorLogger.h"
#include "CPP/Interactor.h"
#include <iostream>

class DialogMenu;
class DialogItem;
class Event;

/*
 *  Gaudi namespace
 */
namespace LHCb  {

    /** @class ErrorDisplay ErrorDisplay.h GaudiUPI/ErrorDisplay.h
      *
      * Dialog class to interact with a Gaudi task
      * 
      * @author  M.Frank
      * @version 1.0
      */
  class ErrorDisplay : public DimErrorLogger, public Interactor  {
  protected:
    /** @class Setup ErrorDisplay.h GaudiUPI/ErrorDisplay.h
      *
      * Dialog class to setup error logger display
      * 
      * @author  M.Frank
      * @version 1.0
      */
    class Logger : public std::streambuf     { 
    public:
      /// Standard Constructor
      Logger (LHCb::ErrorDisplay& iface); 
      /// Standard destructor
      ~Logger (); 
      /// Callback to fill stream buffer
      int overflow (int c); 
      /// Callback indicating EOF
      int underflow (); 
    private:  
      /// Save old stream buffer
      std::streambuf* _old; 
      /// String currently beeing assembled
      std::string _buf; 
      /// Connection to java environment
      ErrorDisplay& m_logger;
    };

    /// Main dialog window
    DialogMenu* m_window;
    /// Main menu ommand item
    DialogItem* m_cmd;
    // Pointer to child window (setup)
    Interactor* m_child;
    /// Pointer to cout catcher
    Logger*     m_log;
    /// Set new child window and delete existing one
    void setNewChild(Interactor* c);
    /// Build top level menu
    void showTopMenu();
    /// Interactor overload: handle menu interupts
    void handle (const Event& event);
    /// Show message in the message window
    void print(const char* fmt, ...);
    /// Internal report of messages to error display
    virtual void reportMessage(int typ, const std::string& src, const std::string& msg);

  public:
    /// Standard Constructor.
    ErrorDisplay( const std::string& nam, ISvcLocator* svc);
    /// Destructor.
    virtual ~ErrorDisplay();
    /// Initialize the service.
    virtual StatusCode initialize();
    /// Finalise the service.
    virtual StatusCode finalize();
    /// Report Error to logger
    virtual void reportMessage(int typ, const std::string& src, const std::string& msg);
  };
}      // Enad namespace Gaudi
#endif // GAUDIUPI_ERRORDISPLAY_H
