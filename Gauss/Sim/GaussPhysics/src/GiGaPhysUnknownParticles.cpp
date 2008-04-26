// $Id: GiGaPhysUnknownParticles.cpp,v 1.1 2008-04-26 22:00:45 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

// G4 
#include "G4UnknownParticle.hh"
#include "G4ProcessManager.hh"

// local
#include "G4BcMesonMinus.h"
#include "G4BcMesonPlus.h"
#include "G4AntiLambdabZero.h"
#include "G4LambdabZero.h"
#include "GiGaPhysUnknownParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaPhysUnknownParticles
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysUnknownParticles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaPhysUnknownParticles::GiGaPhysUnknownParticles
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
GiGaPhysUnknownParticles::~GiGaPhysUnknownParticles(){}

//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaPhysUnknownParticles::ConstructParticle()
{
  G4BcMesonMinus::BcMesonMinusDefinition() ;
  G4BcMesonPlus::BcMesonPlusDefinition() ;  
  G4AntiLambdabZero::AntiLambdabZeroDefinition();
  G4LambdabZero::LambdabZeroDefinition();
  G4UnknownParticle::UnknownParticleDefinition();
}

//=============================================================================
// ConstructProcess
//=============================================================================
void GiGaPhysUnknownParticles::ConstructProcess()
{
  theParticleIterator -> reset() ;
  while ( (*theParticleIterator)() ) {
    G4ParticleDefinition * particle = theParticleIterator -> value() ;
    G4ProcessManager     * pmanager = particle -> GetProcessManager() ;
//    if ( m_decayProcess.IsApplicable( *particle ) ) {
//      debug() << "### Decays for " << particle -> GetParticleName() 
//              << endreq ;
//      pmanager -> AddProcess( &m_decayProcess ) ;
      // set ordering for PostStepDoIt and AtRestDoIt
//      pmanager -> SetProcessOrdering( &m_decayProcess , idxPostStep ) ;
//      pmanager -> SetProcessOrdering( &m_decayProcess , idxAtRest   ) ;
//      pmanager -> DumpInfo() ;      
//    }
    if ( particle -> GetParticleName() == "unknown" ) {
      pmanager -> AddProcess( &m_unknownDecay ) ;
      pmanager -> SetProcessOrdering( &m_unknownDecay , idxPostStep ) ;
      debug() << "### Unknown Decays for " << particle -> GetParticleName() 
              << endreq ;
      if ( msgLevel( MSG::DEBUG) ) 
        pmanager -> DumpInfo() ;
    }
  }
}

//=============================================================================
