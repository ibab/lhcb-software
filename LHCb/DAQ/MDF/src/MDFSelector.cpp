// $Id: MDFSelector.cpp,v 1.2 2006-01-10 09:43:16 frankb Exp $
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
    std::vector<RawBank*> m_banks;
  public:
    /// Standard constructor
    MDFContext(const RawDataSelector* pSel) : RawDataSelector::LoopContext(pSel) {}
    /// Standard destructor 
    virtual ~MDFContext()          {                      }
    /// Receive event and update communication structure
    virtual StatusCode receiveData()  {
      m_descriptor.setLength(0);
      m_banks.clear();
      if ( m_accessDsc.ioDesc > 0 )  {
        MDFHeader h;
        int len = 0;
        if ( StreamDescriptor::read(m_accessDsc,&h,sizeof(MDFHeader)) )  {
          if ( h.size()+sizeof(MDFHeader) > size_t(m_descriptor.max_length()) )  {
            m_descriptor.allocate(sizeof(MDFHeader) + size_t(h.size()*1.5));
          }
          char* ptr = m_descriptor.data()+sizeof(MDFHeader);
          MDFHeader* hdr = (MDFHeader*)m_descriptor.data();
          *hdr = h;
          if ( StreamDescriptor::read(m_accessDsc,ptr,h.size()) )  {
            m_descriptor.setLength(h.size()+sizeof(MDFHeader));
            return decodeRawBanks(ptr,ptr+h.size(),m_banks);
          }
        }
      }
      return StatusCode::FAILURE;
    }

    virtual const std::vector<LHCb::RawBank*>& banks()  const { return m_banks;  }
    std::vector<LHCb::RawBank*>& banks()                      { return m_banks;  }
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
