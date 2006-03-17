// $Id: MEPSelector.cpp,v 1.5 2006-03-17 17:23:56 frankb Exp $
//====================================================================
//	MEPSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataSelector.h"
#include "MDF/MEPEvent.h"

namespace LHCb  {
  /** @class MEPContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class MEPContext : public RawDataSelector::LoopContext {
    typedef std::vector<RawBank*>   Banks;
    typedef std::map<unsigned int, Banks> Events;
    Banks  m_banks;
    Events m_events;
    unsigned m_mask[4];
  public:
    /// Standard constructor
    MEPContext(const RawDataSelector* pSel) : RawDataSelector::LoopContext(pSel) {
      m_mask[0] = m_mask[1] = m_mask[2] = m_mask[3] = ~0x0;
    }
    /// Standard destructor 
    virtual ~MEPContext()          {                      }
    /// Receive event and update communication structure
    StatusCode receiveData()  {
      m_banks.clear();
      if ( m_events.empty() )  {
        unsigned int partitionID;
        StatusCode sc = readMEPrecord(m_descriptor, m_accessDsc);
        if ( !sc.isSuccess() )  {
          return sc;
        }
        MEPEvent* me = (MEPEvent*)m_descriptor.data();
        decodeMEP2EventBanks(me,partitionID,m_events);
      }
      if ( !m_events.empty() )  {
        Events::iterator i = m_events.begin();
        m_banks = (*i).second;
        m_events.erase(i);
        return StatusCode::SUCCESS; 
      }
      return StatusCode::FAILURE;
    }
    virtual const std::vector<RawBank*>& banks()  const { return m_banks;  }
    std::vector<RawBank*>& banks()                      { return m_banks;  }
    /// Accessor: event size
    const unsigned int  size() const        { return 0;         }
    /// Accessor: event type identifier
    const unsigned char eventType() const   { return 1;         }
    /// Accessor: trigger mask
    const unsigned int* triggerMask() const { return m_mask;    }
  };

  /** @class MEPSelector
    */
  class MEPSelector : public RawDataSelector  {
  public:
    /// Create a new event loop context
    /** @param refpCtxt   [IN/OUT]  Reference to pointer to store the context
      * 
      * @return StatusCode indicating success or failure
      */
    virtual StatusCode createContext(Context*& refpCtxt) const {
      refpCtxt = new MEPContext(this);
      return StatusCode::SUCCESS;
    }

    /// Service Constructor
    MEPSelector( const std::string& nam, ISvcLocator* svcloc )
    : RawDataSelector( nam, svcloc)    {    }

    /// Standard destructor
    virtual ~MEPSelector()  {}
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPSelector);
