// $Id: ErrorLog.h,v 1.3 2008-05-27 06:52:49 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/ErrorLog.h,v 1.3 2008-05-27 06:52:49 frankb Exp $
#ifndef ROLOGGER_ERRORLOG_H
#define ROLOGGER_ERRORLOG_H

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations

  class Logger;

  /**@class ErrorLog ErrorLog.h ROLogger/ErrorLog.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class ErrorLog : public Interactor {
  protected:
    /// Reference to message logger
    Interactor* m_messageLog;
    /// Reference to history logger
    Interactor* m_historyLog;
    /// Reference to partition display
    Interactor* m_partDisplay;
    /// Reference to partition listener
    Interactor* m_partListener;

  public:
    /// Standard constructor with object setup through parameters
    ErrorLog(int argc, char** argv);
    /// Standard destructor
    virtual ~ErrorLog();

    /// Shutdown client windows
    void shutdown();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Help printout for RTL CLI
    static void help_fun();
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_ERRORLOG_H */
