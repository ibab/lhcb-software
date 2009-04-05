// $Id: KillAtOriginCut.cpp,v 1.1 2009-04-05 17:40:50 gcorti Exp $
// Include files 


// local
#include "KillAtOriginCut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KillAtOriginCut
//
// 2009-03-26 : Gloria CORTI
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::KillAtOriginCut::KillAtOriginCut( const G4String& name )
  : SpecialCuts(name)
{

  if ( verboseLevel > 1 ) {
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() 
              << " is created "<< std::endl;
  }
  SetProcessType(fUserDefined);

  m_counters.clear();

}

//=============================================================================
// Destructor
//=============================================================================
GiGa::KillAtOriginCut::~KillAtOriginCut() {} 

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::KillAtOriginCut::KillAtOriginCut(GiGa::KillAtOriginCut& right)
  : SpecialCuts(right) {}

//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::KillAtOriginCut::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = Forced;

  // default proposed step
  G4double proposedStep = 0.0;  

  // Introduce counter for particle type (map like) to write at the end
  std::string pname = track.GetDefinition()->GetParticleName();
  ++m_counters[pname];

  return proposedStep;    
}
  
//=============================================================================
// Overrides default behaviour of SpecialKillCuts not to deposit any energy
//=============================================================================
G4VParticleChange* GiGa::KillAtOriginCut::PostStepDoIt( const G4Track& aTrack,
                                                        const G4Step& ) 
{

  aParticleChange.Initialize(aTrack);
  aParticleChange.ProposeEnergy(0.0) ;
  aParticleChange.ProposeLocalEnergyDeposit(0.0) ;
  aParticleChange.ProposeTrackStatus(fStopAndKill);
  return &aParticleChange;
}

//=============================================================================
// Print how many particles of a given type have been killed
//=============================================================================
void GiGa::KillAtOriginCut::printCounters()
{
  for( std::map<std::string,int>::const_iterator counter = m_counters.begin();
       m_counters.end() != counter; ++counter ) {

    std::cout << "GAUSS::Geant4::" << GetProcessName() << "# " 
              << counter->first << " killed at origin = " << counter->second 
              << std::endl;
  }
}
  

//=============================================================================
