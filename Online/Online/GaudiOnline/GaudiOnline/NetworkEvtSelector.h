// $Id: NetworkEvtSelector.h,v 1.2 2008-10-21 13:53:50 frankb Exp $
//====================================================================
//  NetworkEvtSelector
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The NetworkEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria" with input from MBM buffers.
//
//  Author     : M.Frank
//  Created    : 4/12/2007
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/NetworkEvtSelector.h,v 1.2 2008-10-21 13:53:50 frankb Exp $
#ifndef GAUDIONLINE_NETWORKEVTSELECTOR_H
#define GAUDIONLINE_NETWORKEVTSELECTOR_H 1

// Include files
#include "GaudiOnline/OnlineBaseEvtSelector.h"

// Forward declarations
namespace DataTransfer {  struct NET; }

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class NetworkEvtSelector;
  class NetworkContext;

  /** @class NetworkContext
    *
    * Event selector context for asynchronous network input 
    *
    * @author M.Frank
    */
  class NetworkContext : public OnlineContext  {
  private:
  public:
    /// Pointer to netplug device
    DataTransfer::NET* m_netPlug;
    /// Combined request buffer
    MBM::Requirement   m_request;
    /// Source process
    std::string        m_input;
  public:
    /// Standard constructor
    NetworkContext(const NetworkEvtSelector* pSelector);
    /// Standard destructor 
    virtual ~NetworkContext() {}
    /// Notification callback when event provider dies
    void taskDead(const std::string& who);
    /// Notification callback to handle event data
    void handleData(const std::string& src, size_t siz, char* buff);
    /// Rearm event data request
    virtual StatusCode rearmEvent();
    /// Receive event information
    virtual StatusCode receiveEvent();
    /// Free event resources
    virtual StatusCode freeEvent();
    /// Connect to event data source
    virtual StatusCode connect(const std::string& input);
    /// Close connection to event data source
    void close();
  };

  /** @class NetworkEvtSelector  NetworkEvtSelector.h
    *
    * Event selector implementation for asynchronous event input.
    *
    * @author  M.Frank
    * @vrsion  1.0
    */
  class NetworkEvtSelector : public OnlineBaseEvtSelector  {
  public:
    /// Create a new event loop context
    virtual StatusCode createContext(Context*& refpCtxt) const;
    /// Service Constructor
    NetworkEvtSelector(const std::string& name, ISvcLocator* svcloc);
    /// Standard destructor
    virtual ~NetworkEvtSelector()    {}
    /// Event printout
    void printEvt(const std::string& src);
  };
}      // End namespace LHCb
#endif // GAUDIONLINE_NETWORKEVTSELECTOR_H

