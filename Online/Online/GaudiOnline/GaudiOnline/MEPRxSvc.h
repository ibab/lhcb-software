//	============================================================
//
//	MEPRxSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_MEPRXSVC_H
#define GAUDIONLINE_MEPRXSVC_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "RTL/rtl.h"
// Forward declarations
class ISvcLocator;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MEPRxSvc MEPRxSvc.h GaudiOnline/MEPRxSvc.h
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class MEPRxSvc : public Service, virtual public IRunable    {
    lib_rtl_thread_t m_handle;
    bool   m_receiveEvents;

    /// Thread execution routine.
    static int exec(void* arg);
    /// Worker routine
    int execute();
  public:
    /// Standard Constructor
    MEPRxSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPRxSvc();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// Service overload: initialize()
    virtual StatusCode initialize();
    /// Service overload: finalize()
    virtual StatusCode finalize();
    /// IRunable implementation: Run the object
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_MEPRXSVC_H
