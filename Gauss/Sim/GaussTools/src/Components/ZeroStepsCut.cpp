// $Id: ZeroStepsCut.cpp,v 1.1 2009-05-26 17:14:25 gcorti Exp $
// Include files 


// local
#include "ZeroStepsCut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ZeroStepsCut
//
// 2009-03-26 : Gloria CORTI
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::ZeroStepsCut::ZeroStepsCut( const G4String& name, int nTotSteps, 
                                  double maxLenght, int nMaxZeroSteps, 
                                  const G4String& world )
  : SpecialCuts(name)
{

  m_maxNumSteps = nTotSteps;
  m_maxStepLenght = maxLenght;
  m_maxZeroSteps = nMaxZeroSteps;
  m_worldVol = world;

  if ( verboseLevel > 1 ) {
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() 
              << " is created "<< std::endl;
  }
  
  SetProcessType(fUserDefined);
  
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::ZeroStepsCut::~ZeroStepsCut() {} 

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::ZeroStepsCut::ZeroStepsCut(GiGa::ZeroStepsCut& right) : SpecialCuts(right) {}

//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::ZeroStepsCut::
PostStepGetPhysicalInteractionLength( const G4Track& track,
                                      G4double ,
                                      G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;

  // default proposed step
  G4double proposedStep = DBL_MAX;  

  // Check this only if the step number is > 1M (we can reduce it)
  int stepN = track.GetCurrentStepNumber();  
  G4VPhysicalVolume* currentVol = track.GetVolume();
  if ( stepN == 1 ) {
    m_countZero = 0;
    m_prevVol = 0;
  }
  if ( stepN < m_maxNumSteps ) {
    m_prevVol = currentVol;
    return proposedStep;    
  }

  // The particle took an extremely high number of steps check if the next
  // 20 steps very small steps
  double stepLenght = track.GetStepLength();

  if ( stepN == m_maxNumSteps ) {
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() << std::endl;
    std::cout << "   reached " << stepN << " steps for track "
              << track.GetTrackID() << ": " 
              << track.GetDefinition()->GetParticleName() << " of E_kin = "
              << track.GetKineticEnergy() << std::endl;
    std::cout << "   in volume " << currentVol->GetName() << std::endl;
  }

  std::cout << "GAUSS::Geant4:: " <<  GetProcessName() << std::endl;
  std::cout << "   reached " << stepN << " for track "
            << track.GetTrackID() << std::endl;
  std::cout << "   step lenght      " << stepLenght << std::endl;
  std::cout << "   volume           " << currentVol->GetName() << std::endl;

  if ( stepLenght <= m_maxStepLenght ) {

    // Now find if the step is always in the same volume, beside the 
    // world
    if ( currentVol->GetName() != m_worldVol ) {
      if ( currentVol == m_prevVol ) {
        m_countZero++;
      }
      else {
        m_countZero = 0;
      }
      m_prevVol = currentVol;
    } 
  } else {
    m_countZero = 0;
    if ( currentVol->GetName() != m_worldVol ) {
      m_prevVol = currentVol;
    }
  }
  
  if ( m_countZero >= m_maxZeroSteps ) {
    proposedStep = 0.;
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() << std::endl;
    std::cout << "   reached " << m_countZero 
              << " repeated zero steps for track "
              << track.GetTrackID() << std::endl;
    std::cout << "   " << track.GetDefinition()->GetParticleName() 
              << " of E_kin = " << track.GetKineticEnergy() 
              << std::endl;
    std::cout << "   at step no. " << stepN << std::endl;
    std::cout << "   last step lenght is " << stepLenght << std::endl;
    std::cout << "   for volume " << currentVol->GetName() << std::endl;
  }
  
  return proposedStep;    
}
  
//=============================================================================
// Overrides default behaviour of SpecialKillCuts not to deposit any energy
//=============================================================================
G4VParticleChange* GiGa::ZeroStepsCut::PostStepDoIt( const G4Track& aTrack,
                                                     const G4Step& ) {

  aParticleChange.Initialize(aTrack);
  aParticleChange.ProposeEnergy(0.0) ;
  aParticleChange.ProposeLocalEnergyDeposit(0.0) ;
  aParticleChange.ProposeTrackStatus(fStopAndKill);
  return &aParticleChange;
}

//=============================================================================
