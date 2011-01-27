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
//#include "GaudiKernel/Algorithm.h" 
//#include "TriggerTypeFilter.h"
#include <cmath>
#ifndef TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H 
#define TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H 1
#include "GaudiAlg/GaudiAlgorithm.h"
class TriggerTypeFilter: public GaudiAlgorithm {
  public: 
	/// Standard constructor
	TriggerTypeFilter( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~TriggerTypeFilter( ); ///< Destructor
	virtual StatusCode initialize();    ///< Algorithm initialization
	virtual StatusCode execute   ();    ///< Algorithm execution
	virtual StatusCode finalize  ();    ///< Algorithm finalization
  protected:
  private:
	int m_triggerTypeToPass;
	std::string  m_rawEventLoc;
	StatusCode Expo( unsigned int numSteps);
};
#endif // TRIGGERTYPEFILTER_TRIGGERTYPEFILTER_H


DECLARE_ALGORITHM_FACTORY( TriggerTypeFilter )
TriggerTypeFilter::TriggerTypeFilter( const std::string& name,	ISvcLocator* pSvcLocator)  
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_triggerTypeToPass(0)
{
  declareProperty("TriggerTypeToPass", m_triggerTypeToPass=5 );
  declareProperty("RawEventLocation", m_rawEventLoc=LHCb::RawEventLocation::Default);
}

TriggerTypeFilter::~TriggerTypeFilter() {} 
StatusCode TriggerTypeFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); 
  debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode TriggerTypeFilter::execute() {
  debug() << "==> Execution counter:" << endmsg;
  debug()<< " ==> getRawEvent() " <<endmsg;
  LHCb::RawEvent* rawEvent;
  if(!exist<LHCb::RawEvent>(m_rawEventLoc)){
	error()<< " ==> There is no RawEvent at: "  <<  m_rawEventLoc  <<endmsg;
	return ( StatusCode::FAILURE );
  }else{
	rawEvent=get<LHCb::RawEvent>(m_rawEventLoc);
	debug()<< " ==> The RawEvent has been retrieved from location: " << m_rawEventLoc  <<endmsg;
  }
  const std::vector<LHCb::RawBank*>&  odinBanks = rawEvent->banks(LHCb::RawBank::ODIN);
  if(odinBanks.size() >0){ 
	LHCb::OnlineRunInfo *odin = odinBanks[0]->begin<LHCb::OnlineRunInfo>(); 
	if ( odin->triggerType == m_triggerTypeToPass) setFilterPassed(true);
	else setFilterPassed(false) ;
  }
  else setFilterPassed(false) ;
  return StatusCode::SUCCESS;
}


StatusCode TriggerTypeFilter::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  
}
