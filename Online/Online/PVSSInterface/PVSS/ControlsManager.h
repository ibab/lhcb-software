// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/ControlsManager.h,v 1.4 2008-03-19 08:07:47 frankm Exp $
//  ====================================================================
//  ControlsManager.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_CONTROLSMANAGER_H
#define ONLINE_PVSS_CONTROLSMANAGER_H

// Framework include files
#include "PVSS/Printer.h"
#include "PVSS/NamedIdentified.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/ReadTransaction.h"
#include "PVSS/WriteTransaction.h"

// C++ include files
#include <memory>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class Printer;
  class CfgManager;
  class DeviceIO;
  class DeviceManager;
  class DevTypeManager;
  class ReadTransaction;
  class WriteTransaction;

  /** @class ControlsManager   ControlsManager.h  PVSS/ControlsManager.h
    *
    *  PVSS manager interface.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class ControlsManager  : public NamedIdentified {
  public:
    friend struct CfgManip<ControlsManager>;

  protected:
    /// Access to configuration manager
    CfgManager            *m_cfgMgr;
    /// Access to device manager
    DeviceManager         *m_devMgr;
    /// Device type manager
    DevTypeManager        *m_devTypeMgr;
    /// Error logger instance
    std::auto_ptr<Printer> m_logger;

  public:
    /// Initializing constructor
    ControlsManager(int i, const std::string nam);
    /// Standard destructor
    virtual ~ControlsManager();
    /// Return the host name of the event manager of this system
    std::string hostName() const;
    /// Access to error logger
    Printer* logger() const                {  return m_logger.get();}
    /// Access to controls manager
    CfgManager* configMgr() const          {  return m_cfgMgr;      }
    /// Access to device manager 
    DeviceManager* deviceMgr() const       {  return m_devMgr;      }
    /// Access to device manager 
    DevTypeManager* typeMgr() const        {  return m_devTypeMgr;  }
    /// Create device reader to read device data from PVSS
    std::auto_ptr<DeviceIO> devReader();
    /// Create device writer to write device data to PVSS
    std::auto_ptr<DeviceIO> devWriter();
    /// Create transaction object to read device data from PVSS
    std::auto_ptr<ReadTransaction> readTransaction();
    /// Create transaction object to write device data to PVSS
    std::auto_ptr<WriteTransaction> writeTransaction();


    /// Full data point name with system name
    std::string dpFullName(const std::string& nam) const;
    /// Extract name of datapoint from online/original name
    static std::string dpName(const std::string& dp);
    /// Extract system name of datapoint from online/original name
    static std::string dpSystemName(const std::string& dp);
    /// Extract system name of datapoint from online/original name
    static std::string dpElementName(const std::string& dp);
    /// Extract system and element name of datapoint from online/original name
    static std::string dpSysElementName(const std::string& dp);

  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_CONTROLSMANAGER_H
