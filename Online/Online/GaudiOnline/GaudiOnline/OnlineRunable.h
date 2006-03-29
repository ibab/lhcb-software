//	============================================================
//
//	OnlineRunable.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_ONLINERUNABLE_H
#define  GAUDIONLINE_ONLINERUNABLE_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"

// Forward declarations
class ISvcLocator;


/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class OnlineRunable OnlineRunable.h GaudiOnline/OnlineRunable.h
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class OnlineRunable : public Service, virtual public IRunable    {
    /// Debug flag to start debugger online
    int m_debug;
  public:
    /// Standard Constructor
    OnlineRunable(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~OnlineRunable();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: Initialize the service
    virtual StatusCode initialize();
    /// IService implementation: Finalize the service
    virtual StatusCode finalize();
    /// IRunable implementation: Run the object
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_ONLINERUNABLE_H
