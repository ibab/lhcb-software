// $Id: ProcessorDisplay.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
//====================================================================
//  OnlineTools
//--------------------------------------------------------------------
//
//  Package    : OnlineTools
//
//  Description: 
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineTools/OnlineTools/ProcessorDisplay.h,v 1.1 2008-09-18 13:08:22 frankb Exp $
#ifndef ONLINETOOLS_PROCESSORDISPLAY_H
#define ONLINETOOLS_PROCESSORDISPLAY_H

// Framework include files
#include "CPP/Interactor.h"

/*
 *   OnlineTools namespace declaration
 */
namespace OnlineTools {

  // Forward declarations
  class NodeManipulator;

  /**@class ProcessorDisplay ProcessorDisplay.h OnlineTools/ProcessorDisplay.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class ProcessorDisplay : public CPP::Interactor {

  protected:
    int              m_id;
    /// Parent object interactor
    Interactor*      m_parent;
    /// Message logger interactor
    Interactor*      m_msg;
    /// Mode list
    NodeManipulator* m_nodes;
    /// Current cursor position
    int              m_cursor;
  public:
    /// Standard constructor with object setup through parameters
    ProcessorDisplay(CPP::Interactor* parent, CPP::Interactor* msg, NodeManipulator* manip);
    /// Standard destructor
    virtual ~ProcessorDisplay();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
  };

}      /* End namespace OnlineTools */
#endif /* ONLINETOOLS_PROCESSORDISPLAY_H */
