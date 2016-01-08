// $Id: RawDataSelector.h,v 1.19 2008-12-04 13:38:25 frankb Exp $
//====================================================================
#ifndef MDF_RAWDATASELECTOR_H
#define MDF_RAWDATASELECTOR_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "MDF/StreamDescriptor.h"

// Forward declarations
namespace Gaudi { 
  class IIODataManager;
  class IDataConnection;
 }

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RawDataSelector RawDataSelector.h MDF/RawDataSelector.h
    * Component for reading MDF (Master Data File) files.
    *
    * @author M.Frank
    * @date   12/12/2005
    */
  class RawDataSelector : public extends<Service, IEvtSelector>  {
  public:

    /** @class LoopContext
      *
      *  @author  M.Frank
      *  @version 1.0
      */
    class LoopContext : public IEvtSelector::Context {
    protected:
      /// Owning event selector
      const RawDataSelector*           m_sel;
      /// Connection specs of current file
      std::string                      m_conSpec;
      /// Data holder
      std::pair<char*,int>             m_data;
      /// Current file offset
      long long                        m_fileOffset;
      /// Pointer to file manager service
      Gaudi::IIODataManager*           m_ioMgr;
      /// Pointer to file connection
      Gaudi::IDataConnection*          m_connection;
      /// Cached pointer to trigger mask of the event selector
      const std::vector<unsigned int>* m_trgMask;
      /// Cached pointer to veto mask of the event selector
      const std::vector<unsigned int>* m_vetoMask;

    public:
      /// Standard constructor
      LoopContext(const RawDataSelector* pSelector);
      /// Standard destructor 
      virtual ~LoopContext()                    { close();              }
      /// IEvtSelector::Context overload; context identifier
      virtual void* identifier() const          { return (void*)m_sel;  }
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

    /// IService implementation: initialize the service
    virtual StatusCode initialize();

    /// IService implementation: finalize the service
    virtual StatusCode finalize();


    /** Create a new event loop context
      * @param[in,out] refpCtxt  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const = 0;

    /** Access last item in the iteration
      * @param[in,out] refContext Reference to the Context object.
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
      * @param[in]     jump      Number of events to be skipped
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode next(Context& refCtxt,int jump) const;

    /** Get previous iteration item from the event loop context
      * @param[in,out] refCtxt   Reference to the context
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

    /// Access to the file manager
    Gaudi::IIODataManager* fileMgr()  const  {   return m_ioMgr; }

    /// Additional dataspace in buffer [BYTES]
    int additionalSpace() const {   return m_addSpace*1024; }

    /// Access to required trigger mask
    const std::vector<unsigned int>& triggerMask()  const  {   return m_trgMask; }

    /// Access to required veto mask
    const std::vector<unsigned int>& vetoMask()     const  {   return m_vetoMask; }

    /// Service Constructor
    RawDataSelector( const std::string& name, ISvcLocator* svcloc );

    /// Standard destructor
    virtual ~RawDataSelector()  {}

  protected:
    /// Definition of the masj type
    typedef std::vector<unsigned int> Mask;

    // Data Members
    /// ROOT class ID in TES
    CLID                   m_rootCLID = CLID_NULL;
    /// Name of ROOT element in TES
    std::string            m_rootName;
    /// Property: name of the file manager service
    std::string            m_ioMgrName;
    /// Property: input dataset name
    std::string            m_input;
    /// Pointer to file manager service
    Gaudi::IIODataManager* m_ioMgr = nullptr;
    /// Property: First event to process
    int                    m_skipEvents;
    /// Property: printout frequency
    int                    m_printFreq;
    /// Property: additional dataspace to be used to add data [KBYTES]. Default=0
    int                    m_addSpace;
    /// Property: required trigger mask from MDF header (only 128 bits significant)
    Mask                   m_trgMask;
    /// Property: veto mask from MDF header (only 128 bits significant)
    Mask                   m_vetoMask;
    /// Event record count
    mutable int            m_evtCount = 0;
  };
}
#endif  // MDF_RAWDATASELECTOR_H
