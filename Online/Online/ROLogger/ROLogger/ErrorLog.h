// $Id: ErrorLog.h,v 1.2 2008-05-13 07:55:40 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/ErrorLog.h,v 1.2 2008-05-13 07:55:40 frankb Exp $
#ifndef ROMON_RODIMLOGGER_H
#define ROMON_RODIMLOGGER_H

// Framework include files
#include "CPP/Interactor.h"

// Forward declarations
class Event;

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
    Interactor* m_messageLog;
    Interactor* m_historyLog;
    Interactor* m_partDisplay;
    Interactor* m_partListener;

  public:
    /// Standard constructor with object setup through parameters
    ErrorLog(int argc, char** argv);
    /// Standard destructor
    virtual ~ErrorLog();

    /// Shutdown client windows
    void shutdown();
    /// Display callback handler
    virtual void handle(const Event& ev);
    /// Help printout for RTL CLI
    static void help_fun();
  };
}      /* End namespace ROLogger */
#endif /* ROMON_RODIMLOGGER_H */
