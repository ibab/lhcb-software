// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MEPManager.h,v 1.3 2006-04-18 08:11:54 frankb Exp $
//	====================================================================
//  MEPManager.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef GAUDIONLINE_MEPManager_H
#define GAUDIONLINE_MEPManager_H

#include "GaudiKernel/Service.h"
#include "MBM/mepdef.h"
#include "MBM/bmdef.h"

// Declaration of the interface ID. 
static const InterfaceID IID_IMEPManager("IMEPManager", 0, 0); 

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /** @class MEPManager MEPManager.cpp  GaudiOnline/MEPManager.cpp
    *
    * Conversion service for the online data.
    * 
    * @author  M.Frank
    * @version 1.0
    * @date    01/01/2005
    */
  class MEPManager : public Service  {
    MEPID                     m_mepID;
    std::vector<std::string>  m_buffers;
    std::string               m_procName;
    std::string               m_initFlags;
    int                       m_partitionID;
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IMEPManager; }

    /// Standard service constructor
    MEPManager(const std::string& nam, ISvcLocator* loc);

    /// Standard destructor
    virtual ~MEPManager();

    /// Access to MEP identifier structure
    MEPID  mepID() const  {  return m_mepID;   }

    /// Access to partition ID as specified in the job options
    int partitionID()  const                 {   return m_partitionID;  }
    
    /// Access to process name
    const std::string& processName()  const  {  return m_procName;      }

    /** Query interfaces of Interface
        @param riid       ID of Interface to be retrieved
        @param ppvUnknown Pointer to Location for interface pointer
    */
    virtual StatusCode queryInterface(const InterfaceID& riid,
                                      void** ppvInterface);

    /// IService overload: Initialize MEP manager service
    virtual StatusCode initialize();

    /// IService overload: Finalize MEP manager service
    virtual StatusCode finalize();

    /// Initialize buffers for MEP usage
    StatusCode initializeBuffers();

    /// Connect to specified buffers
    StatusCode connectBuffers();

    /// Cancel MBM connections
    StatusCode cancel();

    /// Issues error message and return error
    StatusCode error(const std::string& msg)   const;
  };
  typedef MEPManager IMEPManager;
}      // End namespace LHCb
#endif // GAUDIONLINE_MEPManager_H
