#include "STBeetleStateTransition.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/StatusMap.h"

#include <boost/foreach.hpp>

DECLARE_TOOL_FACTORY( STBeetleStateTransition)
 
STBeetleStateTransition::STBeetleStateTransition( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  declareProperty("initalState", m_initialState = "ReadoutProblems"  );
  declareProperty("finalState", m_finalState = "OK"); 
  setForcedInit();
  declareInterface<IAlgTool>(this);
}

STBeetleStateTransition::~STBeetleStateTransition()
{
  //destructer
}

StatusCode STBeetleStateTransition::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  DeSTSector::Status initialState = ::Status::toStatus(m_initialState);
  DeSTSector::Status finalState = ::Status::toStatus(m_finalState);

  info() << "Active fraction at input " << tracker()->fractionActive() << endmsg; 

  // get the list of sectors 
  const DeSTDetector::Sectors& sec = tracker()->sectors();
  BOOST_FOREACH(DeSTSector* sector, sec){

    if (sector->sectorStatus() == initialState) continue; // only change a beetle state not sectors !!

    for (unsigned int i = 1; i <= sector->nBeetle() ;++i ){
      if (sector->beetleStatus(i) == initialState )  {
        info() << "Changing state of " << sector->nickname() << " Beetle: " << i 
               << " from " << m_initialState << " to " << m_finalState << endmsg;
        sector->setBeetleStatus(i,finalState);             
      }  
    } // loop beetles
  } // loop sectors

  info() << "Active fraction at end of process " << tracker()->fractionActive() << endmsg;

  return StatusCode::SUCCESS;
}



