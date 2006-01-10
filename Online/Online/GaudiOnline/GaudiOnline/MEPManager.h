// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MEPManager.h,v 1.1 2006-01-10 13:45:02 frankb Exp $
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
    int                       m_partitionID;
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IMEPManager; }

    /// Standard service constructor
    MEPManager(const std::string& nam, ISvcLocator* loc);

    /// Standard destructor
    virtual ~MEPManager();

    MEPID  mepID() const  {  return m_mepID;   }
    /** Query interfaces of Interface
        @param riid       ID of Interface to be retrieved
        @param ppvUnknown Pointer to Location for interface pointer
    */
    virtual StatusCode queryInterface(const InterfaceID& riid,
                                      void** ppvInterface);

    /// Initialize MEP manager service
    virtual StatusCode initialize();

    StatusCode error(const std::string& msg)   const;
    int partitionID()  const                 {   return m_partitionID;  }
    const std::string& processName()  const  {  return m_procName;      }
  };
  typedef MEPManager IMEPManager;
}      // End namespace LHCb
#endif // GAUDIONLINE_MEPManager_H
