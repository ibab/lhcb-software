// $Id: MEPSelector.cpp,v 1.2 2006-01-10 09:43:16 frankb Exp $
//====================================================================
//	MEPSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/MEPEvent.h"
#include "MDF/RawDataSelector.h"
#include "MDF/RawEventHelpers.h"
#include <map>

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
  public:
    /// Standard constructor
    MEPContext(const RawDataSelector* pSel) : RawDataSelector::LoopContext(pSel) {}
    /// Standard destructor 
    virtual ~MEPContext()          {                      }
    /// Receive event and update communication structure
    StatusCode receiveData()  {
      m_banks.clear();
      if ( m_events.empty() )  {
        readNextRecord();
      }
      if ( !m_events.empty() )  {
        Events::iterator i = m_events.begin();
        m_banks = (*i).second;
        m_events.erase(i);
        return StatusCode::SUCCESS; 
      }
      return StatusCode::FAILURE;
    }
    StatusCode readNextRecord()  {
      m_descriptor.setLength(0);
      if ( m_accessDsc.ioDesc > 0 )  {
        unsigned int partitionID, len = 0;
        if ( StreamDescriptor::read(m_accessDsc,&len,sizeof(len)) )  {
          if ( len+sizeof(len) > size_t(m_descriptor.max_length()) )  {
            m_descriptor.allocate(sizeof(len) + size_t(len*1.5));
          }
          MEPEvent* me = (MEPEvent*)m_descriptor.data();
          me->setSize(len);
          if ( StreamDescriptor::read(m_accessDsc,me->first(),len) )  {
            m_descriptor.setLength(len+sizeof(len));
            return decodeMEP2EventBanks(me,partitionID,m_events);
          }
        }
      }
      return StatusCode::FAILURE;
    }
    virtual const std::vector<RawBank*>& banks()  const { return m_banks;  }
    std::vector<RawBank*>& banks()                      { return m_banks;  }
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
