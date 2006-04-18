//	============================================================
//
//	DaemonRunable.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_DAEMONRUNABLE_H
#define GAUDIONLINE_DAEMONRUNABLE_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IEventProcessor.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class DaemonRunable DaemonRunable.h GaudiOnline/DaemonRunable.h
    *
    *  Dummy runable class for gaudi Daemons.
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class DaemonRunable : public Service, 
                        virtual public IRunable,
                        virtual public IEventProcessor
{
  public:
    /// Standard Constructor
    DaemonRunable(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~DaemonRunable();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IRunable overload: Process events
    virtual StatusCode run();
    /// IRunable overload: Process single event
    virtual StatusCode executeEvent(void* par=0);
    /// IEventProcessor overload: Process the maxevt events as a Run (beginRun() and endRun() called)
    virtual StatusCode executeRun(int maxevt);
    /// IEventProcessor overload: Process the next maxevt events
    virtual StatusCode nextEvent(int maxevt);
    /// IEventProcessor overload: Schedule a stop of the current event processing
    virtual StatusCode stopRun();
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_DAEMONRUNABLE_H
