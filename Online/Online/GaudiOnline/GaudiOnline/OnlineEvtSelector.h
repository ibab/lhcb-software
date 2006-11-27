// $Id: OnlineEvtSelector.h,v 1.12 2006-11-27 13:46:37 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/OnlineEvtSelector.h,v 1.12 2006-11-27 13:46:37 frankb Exp $

#ifndef GAUDIONLINE_ONLINEEVTSELECTOR_H
#define GAUDIONLINE_ONLINEEVTSELECTOR_H 1

// Include files
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiOnline/OnlineService.h"
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
  class OnlineEvtSelector : public OnlineService, virtual public IEvtSelector  {
    friend class OnlineContext;

  public:
    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService implementation: Service finalization
    virtual StatusCode finalize();

    // IInterface implementation: query interfaces
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

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

  protected:
    // Data Members
    LHCb::IMEPManager*            m_mepMgr;
    /// Partition ID
    int                           m_partID;
    /// Decode data buffer
    bool                          m_decode;
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
    int                           m_evtCount;
  };
}
#endif  // GAUDIONLINE_ONLINEEVTSELECTOR_H
