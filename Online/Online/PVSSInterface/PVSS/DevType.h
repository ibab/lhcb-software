// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DevType.h,v 1.2 2007-03-02 12:19:02 frankb Exp $
//  ====================================================================
//  DevType.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVTYPE_H
#define ONLINE_PVSS_DEVTYPE_H

// Framework include files
#include "PVSS/DevTypeElement.h"

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class DevTypeManager;

  /** @class DevType   DevType.h  PVSS/DevType.h
    *
    *  PVSS device type.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DevType : public DevTypeElement {
  public:
    friend struct DevManip<DevType>;

  protected:
    /// Access to manager
    DevTypeManager *m_manager;

    /// Initializing constructor
    DevType(DevTypeManager* mgr, int i, const std::string& nam, int typ);

    /// Standard destructor
    virtual ~DevType();

  public:

    /// Access device type manager
    DevTypeManager* manager() const    {  return m_manager;   }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVTYPE_H
