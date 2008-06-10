// $Id: DimRPCFileReader.h,v 1.6 2008-06-10 12:36:13 apuignav Exp $
#ifndef GAUDIONLINE_DIMRPCFILEREADER_H
#define GAUDIONLINE_DIMRPCFILEREADER_H

// Framework includes
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiOnline/OnlineService.h"
#include "OnlineKernel/RTL/rtl.h"
#include <GaudiKernel/IEventProcessor.h>
#include "DimRPCFileReaderCommand.h"
# include <time.h>

// Forward declarations
class IIncidentSvc;
class IDataProvider;
class IEvtSelector;
class IEventProcessor;
class Command;

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  class MEPManager;

  /** Class definition of DimRPCFileReader.
   *
   * This is the online extension of the runnable of the application manager.
   * The runable handles the actual run() implementation of the
   * ApplicationMgr object.
   *
   *  @author Markus Frank
   *  @modified Albert Puig
   *  
   *  @version 1.5
   */
  class DimRPCFileReader : public OnlineService, virtual public IRunable  {

    typedef std::pair<int,int> DIMRPC;
  protected:
    /// Reference to MEP manager service
    MEPManager*       m_mepMgr;
    /// IDataProvider pointer to event data service
    IDataProviderSvc* m_dataSvc;
    /// IEventProcessor pointer to the event selector
    IService*         m_evtSelector;
    IService*         m_evtLoopMgr;
    /// Flag indicating that MBM event retrieval is active
    bool              m_receiveEvts;
    /// Monitoring quantity: Number of events processed
    int               m_evtCount;
    /// Lock handle
    void*             m_lock;
    /// DIM RPC handles
    DIMRPC            m_rpc;
    /// Command class
    Command*          m_command;
    /// Buffer with reply data after successful event processing
    std::string       m_reply;
    /// File ID info
    int             m_fileID;
    /// Static DIM callback for command input
    static void cmndCallback(void* tag, void* address, int* size);
    /// Service for publishing info on read events
    static void publishEvents(void* tag, void** buf, int* size, int* first);
    void handleCommand(const char* address, int size);
    /// Timer for testing purposes
    time_t m_timerStartPrep;
    time_t m_timerStartProc;
    time_t m_timerStopPrep;
    time_t m_timerStopProc;
    
  public:
    /// Standard Constructor
    DimRPCFileReader(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~DimRPCFileReader();
    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif // GAUDIONLINE_DIMRPCFILEREADER_H
