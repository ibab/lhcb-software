// $Id: TriggerTypeFilter.cpp,v 1.1 2006/09/04 08:22:30 hmd Exp $
//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTypeFilter
//
// 2011-1-27 Kazu.
//-----------------------------------------------------------------------------
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "MDF/OnlineRunInfo.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

//#include "TriggerTypeFilter.h"
#include <cmath>
#ifndef TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H 
#define TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H 1
#include "GaudiKernel/Algorithm.h"

namespace LHCb {
  class TriggerTypeFilter: public Algorithm {
  public: 
    /// Standard constructor
    TriggerTypeFilter( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TriggerTypeFilter() {} ///< Destructor
    virtual StatusCode execute();    ///< Algorithm execution
  protected:
  private:
    int          m_triggerTypeToPass;
    std::string  m_rawEventLoc;
  };
}

#endif // TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TriggerTypeFilter )

TriggerTypeFilter::TriggerTypeFilter( const std::string& name,	ISvcLocator* pSvcLocator)  
  : Algorithm(name , pSvcLocator), m_triggerTypeToPass(0)
{
  declareProperty("TriggerTypeToPass", m_triggerTypeToPass=5 );
  declareProperty("RawEventLocation",  m_rawEventLoc=LHCb::RawEventLocation::Default);
}

StatusCode TriggerTypeFilter::execute() {
  MsgStream log(msgSvc(), name());
  SmartDataPtr<RawEvent> raw(eventSvc(),m_rawEventLoc);
  if ( !raw ) {
    log << MSG::ERROR << " ==> There is no RawEvent at: "  <<  m_rawEventLoc  <<endmsg;
    setFilterPassed(false) ;
    return StatusCode::FAILURE;
  }
  else   {
    const std::vector<LHCb::RawBank*>&  odinBanks = raw->banks(LHCb::RawBank::ODIN);
    if(odinBanks.size() >0){ 
      LHCb::OnlineRunInfo *odin = odinBanks[0]->begin<LHCb::OnlineRunInfo>();
      unsigned int typ = m_triggerTypeToPass;
      if ( odin->triggerType == typ) setFilterPassed(true);
      else setFilterPassed(false) ;
    }
    else   {
      setFilterPassed(false) ;
    }
  }
  return StatusCode::SUCCESS;
}
