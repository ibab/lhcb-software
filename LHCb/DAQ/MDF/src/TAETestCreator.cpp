// $Id: TAETestCreator.cpp,v 1.2 2007-12-14 12:35:24 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

 /** @class TAETestCreator TAETestCreator.cpp
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class TAETestCreator : public Algorithm {
    /// Flag to test bank removal
    int m_numTAEEvents;
  public: 
    /// Standard constructor
    TAETestCreator( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_numTAEEvents(3) {
      declareProperty("TAEEvents",m_numTAEEvents);
    }

    /// Destructor
    virtual ~TAETestCreator()  {} 

    void setEventType(RawEvent* raw)  {
      typedef std::vector<RawBank*> _V;
      const _V& oBnks = raw->banks(RawBank::ODIN);
      for(_V::const_iterator i=oBnks.begin(); i != oBnks.end(); ++i)  {
        (*i)->begin<OnlineRunInfo>()->triggerType = ODIN::TimingTrigger;
      }
    }

    /// Main execution
    virtual StatusCode execute()  {
      MsgStream log(msgSvc(),name());
      int nevts = abs(m_numTAEEvents)>7 ? 7 : abs(m_numTAEEvents);
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      if ( raw )  {
        setEventType(raw);
        for(int i=-nevts; i<=nevts; ++i)  {
          if ( i != 0 )  {
            RawEvent* copy = 0;
            if ( cloneRawEvent(raw,copy).isSuccess() )  {
              std::string loc = rootFromBxOffset(i)+"/"+RawEventLocation::Default;
              if ( eventSvc()->registerObject(loc,copy).isSuccess() )  {
                continue;
              }
              delete copy;
              log << MSG::ERROR << "Failed to register RawEvent at " << loc << endmsg;
            }
            log << MSG::ERROR << "Failed to clone RawEvent." << endmsg;
          }
        }
        return StatusCode::SUCCESS;
      }
      log << MSG::ERROR << "Failed to access " << RawEventLocation::Default << endmsg;
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,TAETestCreator)
