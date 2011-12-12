// $Id: RootEvtSelector.h,v 1.3 2010-08-24 23:30:32 frankb Exp $
//====================================================================
//	RootTreeEvtSelector.h
//--------------------------------------------------------------------
//
//	Package    : RootCnv
//
//  Description: The RootEvtSelector component is able
//               to produce a list of event references given a set of "selection
//               criteria".
//
//  Author     : M.Frank
//  Created    : 4/01/99
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootEvtSelector.h,v 1.3 2010-08-24 23:30:32 frankb Exp $

#ifndef GAUDIROOTCNV_ROOTEVTSELECTOR_H
#define GAUDIROOTCNV_ROOTEVTSELECTOR_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  // Forward declarations
  class RootCnvSvc;

  /** @class RootEvtSelector RootEvtSelector.h GAUDIROOT/RootEvtSelector.h
   *
   *  Concrete event selector implementation to access ROOT files.
   *
   *  @author  M.Frank
   *  @version 1.0
   *  @date    20/12/2009
   */
  class GAUDI_API RootEvtSelector : public extends1<Service, IEvtSelector> {

    /// Helper method to issue error messages
    StatusCode error(const std::string& msg)  const;
  public:

    /// Service Constructor
    RootEvtSelector(const std::string& name, ISvcLocator* svcloc);

    /// Standard destructor
    virtual ~RootEvtSelector() {}

    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    /// IService implementation: Service finalization
    virtual StatusCode finalize();

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
     *  the state of the context in a way to point to the new list.
     *
     * @param cr The new criteria string.
     * @param c Reference pointer to the Context object.
     */
    virtual StatusCode resetCriteria(const std::string& cr,Context& c)const;
  protected:
    // Data Members
    /// Reference to the corresponding conversion service
    mutable RootCnvSvc*             m_dbMgr;
    /// Class id of root node to create opaque address
    CLID                            m_rootCLID;
    /// Property; Name of the persistency service to search for conversion service.
    std::string                     m_persName;
    /// Property; Name of the concversion service used to create opaque addresses
    std::string                     m_cnvSvcName;
    /// Property: Name of the ROOT entry name
    std::string                     m_rootName;
    /// Property: File criteria to define item iteration
    std::string                     m_criteria;
    /// Property: dummy to fake backwards compatibility
    std::string                     m_dummy;
  };
}
#endif  // GAUDIROOTCNV_ROOTEVTSELECTOR_H
