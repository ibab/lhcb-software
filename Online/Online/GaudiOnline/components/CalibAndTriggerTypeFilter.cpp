#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "MDF/OnlineRunInfo.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

#include <cmath>
#ifndef CALIBANDTRIGGERTYPEFILTER_CALIBANDTRIGGERTYPEFILTER_H 
#define CALIBANDTRIGGERTYPEFILTER_CALIBANDTRIGGERTYPEFILTER_H 1
#include "GaudiKernel/Algorithm.h"

namespace LHCb {
  class CalibAndTriggerTypeFilter: public Algorithm {
  public: 
    /// Standard constructor
    CalibAndTriggerTypeFilter( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~CalibAndTriggerTypeFilter() {} ///< Destructor
    virtual StatusCode execute();    ///< Algorithm execution
  protected:
  private:
    std::vector< int > m_triggerTypesToPass;
    std::vector< int > m_calibTypesToPass;
    std::string        m_rawEventLoc;
  };
}

#endif // CALIBANDTRIGGERTYPEFILTER_CALIBTRIGGERTYPEFILTER_H
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( CalibAndTriggerTypeFilter )

CalibAndTriggerTypeFilter::CalibAndTriggerTypeFilter( const std::string& name,	ISvcLocator* pSvcLocator)  
: Algorithm(name , pSvcLocator), m_triggerTypesToPass(), m_calibTypesToPass()
{
  declareProperty("TriggerTypesToPass", m_triggerTypesToPass = { 4 , 7 } );
  declareProperty("CalibTypesToPass" , m_calibTypesToPass = { 2 , 3 } ) ;
  declareProperty("RawEventLocation",  m_rawEventLoc=LHCb::RawEventLocation::Default);
}

StatusCode CalibAndTriggerTypeFilter::execute() {
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
      // Check trigger type first
      if ( std::find( m_triggerTypesToPass.begin(), m_triggerTypesToPass.end(), odin->triggerType) !=
           m_triggerTypesToPass.end() ) {
        setFilterPassed(true);
        if ( ( odin->triggerType==7 ) && //calibration type check 
             ( std::find( m_calibTypesToPass.begin(), m_calibTypesToPass.end(), odin->readoutType) ==
               m_calibTypesToPass.end() ) )
          setFilterPassed(false);
      }
      else setFilterPassed(false) ;
    }
    else   {
      setFilterPassed(false) ;
    }
  }
  return StatusCode::SUCCESS;
}
