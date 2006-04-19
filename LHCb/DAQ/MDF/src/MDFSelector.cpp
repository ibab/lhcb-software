// $Id: MDFSelector.cpp,v 1.6 2006-04-19 11:44:48 frankb Exp $
//====================================================================
//	MDFSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataSelector.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MDFContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class MDFContext : public RawDataSelector::LoopContext {
    typedef std::vector<RawBank*> Banks;
    Banks     m_banks;
    long long m_fileOffset;
    const MDFHeader* header() const  {
      return (MDFHeader*)m_descriptor.data();
    }

  public:
    /// Standard constructor
    MDFContext(const RawDataSelector* pSel) 
      : RawDataSelector::LoopContext(pSel), m_fileOffset(0)  { }
    /// Standard destructor 
    virtual ~MDFContext()                                    { }
    /// Receive event and update communication structure
    virtual StatusCode receiveData()  {
      m_banks.clear();
      m_fileOffset = StreamDescriptor::seek(m_accessDsc,0,SEEK_CUR);
      if ( readMDFrecord(m_descriptor, m_accessDsc).isSuccess() )  {
        MDFHeader* h = (MDFHeader*)m_descriptor.data();
        char* ptr = m_descriptor.data()+sizeof(MDFHeader);
        return decodeRawBanks(ptr,ptr+h->size(),m_banks);
      }
      return StatusCode::FAILURE;
    }
    long long offset()  const                       { return m_fileOffset;            }
    /// Access to RawBank array
    virtual const Banks& banks()  const             { return m_banks;                 }
    /// Access to RawBank array (NON const)
    Banks& banks()                                  { return m_banks;                 }
    /// Accessor: event size
    virtual const unsigned int  size() const        { return header()->size();        }
    /// Accessor: event type identifier
    virtual const unsigned char eventType() const   { return header()->eventType();   }
    /// Accessor: event type identifier
    virtual const unsigned int  partitionID() const { return 0; }
    /// Accessor: trigger mask
    virtual const unsigned int* triggerMask() const { return header()->triggerMask(); }
  };

  /** @class MDFSelector
    */
  class MDFSelector : public RawDataSelector  {
  public:
    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const {
      refpCtxt = new MDFContext(this);
      return StatusCode::SUCCESS;
    }

    /// Service Constructor
    MDFSelector( const std::string& nam, ISvcLocator* svcloc )
    : RawDataSelector( nam, svcloc)   {     }

    /// Standard destructor
    virtual ~MDFSelector()  {}
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MDFSelector);
