// $Id: ErrorShow.h,v 1.1 2008-05-27 06:53:13 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/ErrorShow.h,v 1.1 2008-05-27 06:53:13 frankb Exp $
#ifndef ROLOGGER_ERRORSHOW_H
#define ROLOGGER_ERRORSHOW_H

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {
  // Forward declarations

  class Logger;

  /**@class ErrorShow ErrorShow.h ROLogger/ErrorShow.h
   *
   *   Error history viewer.
   *
   *   @author M.Frank
   */
  class ErrorShow : public CPP::Interactor {
  protected:
    /// Reference to message logger
    Interactor* m_msg;
    /// Reference to history display
    Interactor* m_display;

  public:
    /// Standard constructor with object setup through parameters
    ErrorShow(int argc, char** argv);
    /// Standard destructor
    virtual ~ErrorShow();

    /// Shutdown client windows
    void shutdown();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Help printout for RTL CLI
    static void help_fun();
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_ERRORSHOW_H */
