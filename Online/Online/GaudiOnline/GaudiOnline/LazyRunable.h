#ifndef GAUDIONLINE_LAZYRUNABLE_H
#define GAUDIONLINE_LAZYRUNABLE_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IEventProcessor.h"

// Forward declarations
class ISvcLocator;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class LazyRunable LazyRunable.h GaudiOnline/LazyRunable.h
    * A runable unwilling to work or use energy 
    *  @author Giacomo Graziani
    */
  class LazyRunable : public Service, 
                        virtual public IRunable,
                        virtual public IEventProcessor
  {

  public:
    /// Standard Constructor
    LazyRunable(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~LazyRunable();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IRunable implementation: Run the object
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
#endif //  GAUDIONLINE_LAZYRUNABLE_H
