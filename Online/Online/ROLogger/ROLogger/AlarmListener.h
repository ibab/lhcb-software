// $Id: AlarmListener.h,v 1.2 2009-01-09 10:31:20 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/AlarmListener.h,v 1.2 2009-01-09 10:31:20 frankb Exp $
#ifndef ROLOGGER_ALARMLISTENER_H
#define ROLOGGER_ALARMLISTENER_H

// C++ include files
#include <string>
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class AlarmListener AlarmListener.h ROLogger/AlarmListener.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class AlarmListener : public Interactor {
  protected:
    /// Reference to parent for communication
    Interactor* m_parent;
    /// Listener service name
    std::string m_name;
    /// Listener facility
    std::string m_facility;
    /// Dim service ID for subfarms
    int         m_subFarmDP;
    
  public:
    /// Standard constructor with object setup through parameters
    AlarmListener(Interactor* parent,const std::string& name, const std::string& fac="alarms");
    /// Standard destructor
    virtual ~AlarmListener();
    /// Access name
    const std::string& name() const {   return m_name; }
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_ALARMLISTENER_H */
