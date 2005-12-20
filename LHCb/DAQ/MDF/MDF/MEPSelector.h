// $Id: MEPSelector.h,v 1.1 2005-12-20 16:33:38 frankb Exp $
//====================================================================
//	MEPSelector.h
//--------------------------------------------------------------------
//
//  Description: The MEPSelector component is able
//               to read MDF (Master Data File) files.
//
//  Author     : M.Frank
//  Created    : 12/12/2005
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MEPSelector.h,v 1.1 2005-12-20 16:33:38 frankb Exp $

#ifndef MDF_MEPSelector_H
#define MDF_MEPSelector_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MDF::EvtSelector
  */
  class MEPSelector : public Service, virtual public IEvtSelector  {
  public:
    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

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
    virtual StatusCode last(Context& refContext) const    {
      return StatusCode::FAILURE;
    }

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
    virtual StatusCode rewind(Context& refCtxt) const   {
      return StatusCode::FAILURE;
    }

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
        the state of the context in a way to point to the new list.
        
        @param cr The new criteria string.
        @param c  Reference pointer to the Context object.
    */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;

    /// Service Constructor
    MEPSelector( const std::string& name, ISvcLocator* svcloc );

    /// Standard destructor
    virtual ~MEPSelector()  {}

  protected:
    // Data Members
    CLID                          m_rootCLID;
    std::string                   m_cnvSvcName;
    std::string                   m_rootName;
    std::string                   m_buffLen;
  };
}
#endif  // MDF_MEPSelector_H
