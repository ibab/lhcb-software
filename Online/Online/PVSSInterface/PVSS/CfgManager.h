// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/CfgManager.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  CfgManager.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CFGMANAGER_H
#define ONLINE_PVSS_CFGMANAGER_H

// Framework include files
#include "PVSS/NamedIdentified.h"

// C++ include files
#include <vector>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class CfgType;

  /** @class CfgManager   CfgManager.h  PVSS/CfgManager.h
    *
    *  PVSS configuration attribute.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class CfgManager  : public NamedIdentified {
  public:
    friend struct CfgManip<CfgManager>;

    /// Internal type definitions: configurations container
    typedef std::vector<CfgType*> Configs;

  protected:
    /// Access to manager
    ControlsManager *m_manager;
    /// Configurations container
    Configs          m_configs;

    /// Initializing constructor
    CfgManager(ControlsManager* mgr, int i, const std::string& nam);
    /// Standard destructor
    virtual ~CfgManager();

    /// Add new configuration
    void addConfig(CfgType* cfg);

  public:

    /// Access to controls manager type
    ControlsManager* manager() const       {  return m_manager;   }
    /// Client access to attributes
    const Configs& configurations()  const {  return m_configs;   }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CFGMANAGER_H
