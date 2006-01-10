// $Id: RawDataSelector.h,v 1.1 2006-01-10 09:43:16 frankb Exp $
//====================================================================
//	RawDataSelector.h
//--------------------------------------------------------------------
//
//  Description: The RawDataSelector component is able
//               to read MDF (Master Data File) files.
//
//  Author     : M.Frank
//  Created    : 12/12/2005
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawDataSelector.h,v 1.1 2006-01-10 09:43:16 frankb Exp $

#ifndef MDF_RAWDATASELECTOR_H
#define MDF_RAWDATASELECTOR_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "MDF/StreamDescriptor.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  class RawBank;

  /** @class RawDataSelector
    */
  class RawDataSelector : public Service, virtual public IEvtSelector  {
  public:
    /** @class LoopContext
      *
      *  @author  M.Frank
      *  @version 1.0
      */
    class LoopContext : public IEvtSelector::Context {
    public:
      enum StreamType { 
        STREAM_NONE, 
        STREAM_FILE, 
        STREAM_INET,
      };
    protected:
      typedef StreamDescriptor   DSC;
      const RawDataSelector*     m_sel;
      DSC                        m_descriptor;
      StreamType                 m_type;
      std::string                m_conSpec;
      DSC::Access                m_bindDsc;
      DSC::Access                m_accessDsc;

    public:
      /// Standard constructor
      LoopContext(const RawDataSelector* pSelector)
      : m_sel(pSelector), m_type(STREAM_NONE)   {     }
      /// Standard destructor 
      virtual ~LoopContext()          { close();             }
      /// IEvtSelector::Context overload; context identifier
      virtual void* identifier() const { return (void*)m_sel; }
      void setCriteria(const std::string& crit);
      /// Receive event and update communication structure
      virtual StatusCode receiveData() = 0;
      /// Set connection
      virtual StatusCode connect();
      /// close connection
      virtual void close()    {
        DSC::close(m_accessDsc);
        DSC::close(m_bindDsc);
      }
      const DSC& descriptor() const  { return m_descriptor; }
      DSC& descriptor()              { return m_descriptor; }
      /// Connection specification
      const std::string& specs() const          {      return m_conSpec;    }
      virtual const std::vector<LHCb::RawBank*>& banks()  const = 0;
    };

    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    // IInterface implementation: query interfaces
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const = 0;

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
    RawDataSelector( const std::string& name, ISvcLocator* svcloc );

    /// Standard destructor
    virtual ~RawDataSelector()  {}

  protected:
    // Data Members
    CLID                          m_rootCLID;
    std::string                   m_cnvSvcName;
    std::string                   m_rootName;
    std::string                   m_buffLen;
  };
}
#endif  // MDF_RAWDATASELECTOR_H
