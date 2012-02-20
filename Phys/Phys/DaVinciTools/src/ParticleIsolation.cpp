// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Property.h"

#include "Kernel/IConeVariables.h"
// local
#include "ParticleIsolation.h"

//-----------------------------------------------------------------------
// Implementation file for class : ParticleIsolation
//
// Author: Anton Poluektov
//-----------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleIsolation );

//=======================================================================
ParticleIsolation::ParticleIsolation(const std::string& name,
                   ISvcLocator* pSvcLocator):
  DVAlgorithm(name, pSvcLocator),
  m_coneVariablesToolName("ConeVariables")
{ 
  declareProperty("ConeVariablesTool", m_coneVariablesToolName = "ConeVariables",
                  "Name of ConeVariables tool" );
}

//=======================================================================
StatusCode ParticleIsolation::initialize() { 

  m_coneVar = 0;

  return DVAlgorithm::initialize() ; 

}

ParticleIsolation::~ParticleIsolation() {}

//=======================================================================
// Main executio
//=======================================================================
StatusCode ParticleIsolation::execute() {

  if (m_coneVar == 0) {
    m_coneVar = tool<IConeVariables>(m_coneVariablesToolName,this);
    
    if (!m_coneVar) {
      error() << "Cone variable too not found, name = " << m_coneVariablesToolName << endreq;
      return StatusCode::FAILURE;
    }
  }

  std::vector<std::string>::const_iterator iLoc = inputLocations().begin();
  std::vector<std::string>::const_iterator endLoc = inputLocations().end();
  for ( ; iLoc != endLoc; ++iLoc) {
  
    std::string location = (*iLoc) + "/Particles";
  
    if(!exist<LHCb::Particle::Range>(location) ) {
      debug()<<("No selection found in "+ location)<<endreq;
      return StatusCode::SUCCESS;
    }
   
    const Particle::Range parts = get<Particle::Range>( location );
    if( parts.empty() ) {
      Warning("No particles found at "+ location, 
              StatusCode::SUCCESS,10).ignore();
      return StatusCode::SUCCESS;
    }

    if (msgLevel(MSG::VERBOSE)) verbose() << " Found "<< parts.size() << " particles" <<endreq;

    Particle::Range::const_iterator icand;
    for ( icand = parts.begin(); icand != parts.end(); icand++) {
    
      ConeInfo info;
    
      StatusCode sc = m_coneVar->getConeInfo(*icand, *icand, info);
      if (sc.isFailure()) {
        
        error() << "Error calculating cone variables" << endreq;
        
        return sc;
      }
      
      Particle* c = const_cast<Particle*>(*icand);
      
      int index = Particle::FirstIsolationInfoIndex;
      
      if (msgLevel(MSG::VERBOSE)) { 
        verbose() << "Adding extraInfo, index " << index << endreq;
        verbose() << " Cone multiplicity " << info.mult << endreq;
        verbose() << " Cone ptasy = " << info.ptasy << endreq;
      }
      
      c->addInfo( index, (double)info.mult );   index++;
      c->addInfo( index, (double)info.px );     index++;
      c->addInfo( index, (double)info.py );     index++;
      c->addInfo( index, (double)info.pz );     index++;
      c->addInfo( index, (double)info.pt );     index++;
      c->addInfo( index, (double)info.p );      index++;
      c->addInfo( index, (double)info.pxasy );  index++;
      c->addInfo( index, (double)info.pyasy );  index++;
      c->addInfo( index, (double)info.pzasy );  index++;
      c->addInfo( index, (double)info.ptasy );  index++;
      c->addInfo( index, (double)info.pasy );   index++;
      c->addInfo( index, (double)info.deltaEta );  index++;
      c->addInfo( index, (double)info.deltaPhi );  index++;

    }
  
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}


//=========================================================================
StatusCode ParticleIsolation::finalize() { 
  return DVAlgorithm::finalize(); 
}

//==========================================================================
