// $Id: OnlineEvtSelector.h,v 1.15 2007-01-26 20:23:36 frankb Exp $
//====================================================================
//  OnlineEvtSelector.h
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The OnlineEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria".
//
//  Author     : M.Frank
//  Created    : 4/01/99
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/OnlineEvtSelector.h,v 1.15 2007-01-26 20:23:36 frankb Exp $

#ifndef GAUDIONLINE_ONLINEEVTSELECTOR_H
#define GAUDIONLINE_ONLINEEVTSELECTOR_H 1

// Include files
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiOnline/ISuspendable.h"
#include "GaudiOnline/MEPManager.h"
#include "MBM/Requirement.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class OnlineContext;

  /** @class OnlineEvtSelector  OnlineEvtSelector.h
    *
    * @author  M.Frank
    * @vrsion  1.0
    */
  class OnlineEvtSelector : public OnlineService, 
                            virtual public ISuspendable,
                            virtual public IEvtSelector,
                            virtual public IIncidentListener
  {
    friend class OnlineContext;

  public:
    /// IInterface implementation: query interfaces
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService implementation: Service finalization
    virtual StatusCode finalize();

    /// ISuspendable implementation: suspend operation
    virtual StatusCode suspend();

    /// ISuspendable implementation: resume operation
    virtual StatusCode resume();

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const;

    /** Access last item in the iteration
      * @param refContext [IN/OUT] Reference to the Context object.
      */
    virtual StatusCode last(Context& refContext) const;

    /// Get next iteration item from the event loop context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt) const;

    /// Get next iteration item from the event loop context, but skip jump elements
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt,int jump) const;

    /// Get previous iteration item from the event loop context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * @param jump      [IN]      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt) const;

    /// Get previous iteration item from the event loop context, but skip jump elements
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * @param jump      [IN]      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt,int jump) const;

    /// Rewind the dataset
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode rewind(Context& refCtxt) const;

    /// Create new Opaque address corresponding to the current record
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createAddress(const Context& refCtxt, IOpaqueAddress*&) const;

    /// Release existing event iteration context
    /** @param refCtxt   [IN/OUT]  Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode releaseContext(Context*& refCtxt) const;

    /** Will set a new criteria for the selection of the next list of events and will change
      * the state of the context in a way to point to the new list.
      * 
      * @param cr The new criteria string.
      * @param c  Reference pointer to the Context object.
      */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

    /// Service Constructor
    OnlineEvtSelector( const std::string& name, ISvcLocator* svcloc );

    /// Standard destructor
    virtual ~OnlineEvtSelector()    {}

    void increaseReqCount()    const     {  m_reqCount++;     }
    void increaseEvtCount()    const     {  m_evtCount++;     }
    bool mustDecode() const              {  return m_decode;  }
  protected:
    /// Lock handle to suspend/resume operations
    lib_rtl_event_t               m_suspendLock;
    /// Data Members
    LHCb::IMEPManager*            m_mepMgr;
    /// Partition ID
    int                           m_partID;
    /// Decode data buffer
    bool                          m_decode;
    /// Allow suspending event access
    bool                          m_allowSuspend;
    /// Input buffer name (default="EVENT")
    std::string                   m_input;
    /// Requirement properties
    std::string                   m_Rqs[8];
    /// Decoded requirements
    mutable MBM::Requirement      m_Reqs[8];
    /// Number of requirements
    mutable int                   m_nreqs;
    /// Property to indicate that the connection goes to a partitioned buffer
    bool                          m_partitionBuffer;
    /// Monitoring quantity: Number of events received
    mutable int                   m_evtCount;
    /// Monitoring quantity: Number of events requested
    mutable int                   m_reqCount;
  };
}
#endif  // GAUDIONLINE_ONLINEEVTSELECTOR_H
