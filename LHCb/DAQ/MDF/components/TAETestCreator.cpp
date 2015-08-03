// $Id: TAETestCreator.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"

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
    bool m_removeDAQStatus;
  public: 
    /// Standard constructor
    TAETestCreator( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_numTAEEvents(3) {
      declareProperty("TAEEvents",m_numTAEEvents);
      declareProperty("RemoveDAQStatus",m_removeDAQStatus=false);
    }

    /// Destructor
    virtual ~TAETestCreator()  {} 

    /// Main execution
    virtual StatusCode execute()  {
      MsgStream log(msgSvc(),name());
      int nevts = abs(m_numTAEEvents)>7 ? 7 : abs(m_numTAEEvents);
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      if ( raw )  {
        // Need to remove all MDF header banks first
        typedef std::vector<RawBank*> _V;
        if ( m_removeDAQStatus ) {
          _V bnks = raw->banks(RawBank::DAQ);
          for(_V::iterator ib=bnks.begin(); ib != bnks.end(); ++ib)  {
            if ( (*ib)->version() == DAQ_STATUS_BANK )
              raw->removeBank(*ib);
          }
        }
        change2TAEEvent(raw, nevts);
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

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,TAETestCreator)
