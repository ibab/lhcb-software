// $Id: PVSSLogListener.h,v 1.1 2008-08-05 21:25:11 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/PVSSLogListener.h,v 1.1 2008-08-05 21:25:11 frankb Exp $
#ifndef ROLOGGER_PVSSLOGLISTENER_H
#define ROLOGGER_PVSSLOGLISTENER_H

// C++ include files
#include <string>
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class PVSSLogListener PVSSLogListener.h ROLogger/PVSSLogListener.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class PVSSLogListener : public Interactor {
  protected:
    Interactor* m_parent;
    std::string m_name;
    int         m_subFarmDP;
    
  public:
    /// Standard constructor with object setup through parameters
    PVSSLogListener(Interactor* parent,const std::string& name);
    /// Standard destructor
    virtual ~PVSSLogListener();
    /// Access name
    const std::string& name() const {   return m_name; }
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_PVSSLOGLISTENER_H */
