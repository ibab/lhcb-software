// $Id: PowercutRecovery.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
//====================================================================
//  OnlineTools
//--------------------------------------------------------------------
//
//  Package    : OnlineTools
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/OnlineTools/PowercutRecovery.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
#ifndef ONLINETOOLS_POWERCUTRECOVERY_H
#define ONLINETOOLS_POWERCUTRECOVERY_H

// Framework include files
#include "CPP/Interactor.h"

/*
 *   OnlineTools namespace declaration
 */
namespace OnlineTools {
  // Forward declarations

  class Logger;
  class ProcessorGroup;

  /**@class PowercutRecovery PowercutRecovery.h OnlineTools/PowercutRecovery.h
   *
   *   Tool to recover processors after a power cut
   *
   *   @author M.Frank
   */
  class PowercutRecovery : public CPP::Interactor {
  protected:
    /// Reference to message logger
    Interactor*     m_messageLog;
    /// Reference to processor group
    ProcessorGroup* m_processors;
    /// Reference to processor display of D1
    Interactor* m_display;

  public:
    /// Standard constructor with object setup through parameters
    PowercutRecovery(int argc, char** argv);
    /// Standard destructor
    virtual ~PowercutRecovery();

    /// Shutdown client windows
    void shutdown();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Help printout for RTL CLI
    static void help_fun();
  };
}      /* End namespace OnlineTools */
#endif /* ONLINETOOLS_POWERCUTRECOVERY_H */
