#include "STRndmBeetleStateTransition.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/StatusMap.h"

// for rndm numbers
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include <boost/foreach.hpp>

DECLARE_TOOL_FACTORY( STRndmBeetleStateTransition)
 
STRndmBeetleStateTransition::STRndmBeetleStateTransition( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  declareProperty("newState", m_newState = "ReadoutProblems"  );
  declareProperty("fracToChange", m_fracToChange = 0.02);

  setForcedInit();
  declareInterface<IAlgTool>(this);
}

STRndmBeetleStateTransition::~STRndmBeetleStateTransition()
{
}

StatusCode STRndmBeetleStateTransition::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // get the random number generator
  auto tRandNumSvc = service<IRndmGenSvc>("RndmGenSvc", true);
  m_uniformDist = tRandNumSvc->generator( Rndm::Flat(0.,1.0) );
  if (!m_uniformDist) return Error( "Failed to init generator ", sc);

  DeSTSector::Status newState = ::Status::toStatus(m_newState);
 
  // get the list of sectors 

  info() << "Active fraction at input " << tracker()->fractionActive() << endmsg; 
  unsigned int tcounter = 0;
  for(DeSTSector* sector: tracker()->sectors()){
    for (unsigned int i = 1; i <= sector->nBeetle(); ++i ){
      if (m_uniformDist->shoot() < m_fracToChange  ){
        sector->setBeetleStatus(i,newState);  
        ++tcounter; 
        info() << "Changed status of " << sector->nickname() << " Beetle "<< i 
               << "to " << m_newState << endmsg;
      } 
    } // loop beetles
  } // loop sectors

  info() << "Killed: " << tcounter << " Beetles " << endmsg;
  info() << "Active fraction now " << tracker()->fractionActive() << endmsg;
  
  return StatusCode::SUCCESS;
}



