// $Id: MDFSelector.cpp,v 1.4 2006-01-10 18:14:29 frankb Exp $
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
      m_banks.clear();
      if ( readMDFrecord(m_descriptor, m_accessDsc).isSuccess() )  {
        MDFHeader* h = (MDFHeader*)m_descriptor.data();
        char* ptr = m_descriptor.data()+sizeof(MDFHeader);
        return decodeRawBanks(ptr,ptr+h->size(),m_banks);
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
