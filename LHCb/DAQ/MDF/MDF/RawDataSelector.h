// $Id: RawDataSelector.h,v 1.13 2007-04-20 12:40:25 cattanem Exp $
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

#ifndef MDF_RAWDATASELECTOR_H
#define MDF_RAWDATASELECTOR_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "MDF/StreamDescriptor.h"

// Forward declarations
namespace Gaudi  { class IFileCatalogSvc; }

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
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
    protected:
      typedef StreamDescriptor   DSC;
      const RawDataSelector*     m_sel;
      std::string                m_fid;
      std::string                m_conSpec;
      DSC::Access                m_bindDsc;
      DSC::Access                m_accessDsc;
      std::pair<char*,int>       m_data;
      long long                  m_fileOffset;

    public:
      /// Standard constructor
      LoopContext(const RawDataSelector* pSelector);
      /// Standard destructor 
      virtual ~LoopContext()                    { close();              }
      /// IEvtSelector::Context overload; context identifier
      virtual void* identifier() const          { return (void*)m_sel;  }
      /// File identifier (when used with catalog)
      virtual const std::string& fid() const    { return m_fid;         }
      /// Connection specification
      const std::string& specs() const          { return m_conSpec;     }
      /// Access to file offset(if possible)
      virtual long long offset()  const         { return m_fileOffset;  }
      /// Raw data buffer (if it exists)
      virtual std::pair<char*,int> data() const { return m_data;        }
      /// Receive event and update communication structure
      virtual StatusCode receiveData(IMessageSvc* msg) = 0;
      /// Skip N events
      virtual StatusCode skipEvents(IMessageSvc* msg,int numEvt) = 0;
      /// Set connection
      virtual StatusCode connect(const std::string& specs);
      /// close connection
      virtual void close();
    };

    /// IService implementation: Db event selector override
    virtual StatusCode initialize();

    // IInterface implementation: query interfaces
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /** Create a new event loop context
      * @param[in,out] refpCtxt  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const = 0;

    /** Access last item in the iteration
      * @param[in.out] refContext Reference to the Context object.
      */
    virtual StatusCode last(Context& /* refContext */ ) const    {
      return StatusCode::FAILURE;
    }

    /** Get next iteration item from the event loop context
      * @param[in,out] refCtxt   Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt) const;

    /** Get next iteration item from the event loop context, but skip jump elements
      * @param[in,out] refCtxt   Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt,int jump) const;

    /** Get previous iteration item from the event loop context
      * @param[in,out] refCtxt   Reference to the context
      * @param[in]     jump      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt) const;

    /** Get previous iteration item from the event loop context, but skip jump elements
      * @param[in,out] refCtxt   Reference to the context
      * @param[in]     jump      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode previous(Context& refCtxt,int jump) const;

    /** Rewind the dataset
      * @param[in,out] refCtxt   Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode rewind(Context& /* refCtxt */ ) const   {
      return StatusCode::FAILURE;
    }

    /** Create new Opaque address corresponding to the current record
      * @param[in,out] refCtxt   Reference to the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createAddress(const Context& refCtxt, IOpaqueAddress*&) const;

    /** Release existing event iteration context
      * @param[in,out] refCtxt   Reference to the context
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
    typedef Gaudi::IFileCatalogSvc Catalog;

    // Data Members
    /// ROOT class ID in TES
    CLID        m_rootCLID;
    /// Name of ROOT element in TES
    std::string m_rootName;
    /// Property: name of the filecatalog service
    std::string m_catalogName;
    /// Pointer to file catalog service
    Catalog*    m_catalog;
    /// Property: First event to process
    int         m_skipEvents;
  };
}
#endif  // MDF_RAWDATASELECTOR_H
