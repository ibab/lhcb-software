// $Id: PingPongCut.cpp,v 1.2 2009-05-26 17:15:25 gcorti Exp $
// Include files 


// local
#include "PingPongCut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PingPongCut
//
// 2009-03-26 : Gloria CORTI
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::PingPongCut::PingPongCut( const G4String& name, int nMaxStep, 
                          double maxLenght, int nMaxRepeat)
  : SpecialCuts(name)
  , m_fracTolerance(1.0e-6) 
{

  m_maxNumSteps = nMaxStep;
  m_maxStepLenght = maxLenght;
  m_maxRepeatSteps = nMaxRepeat;

  if ( verboseLevel > 1 ) {
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() 
              << " is created "<< std::endl;
  }
  
  SetProcessType(fUserDefined);
  
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::PingPongCut::~PingPongCut() {} 

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::PingPongCut::PingPongCut(GiGa::PingPongCut& right) : SpecialCuts(right) {}

//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::PingPongCut::
PostStepGetPhysicalInteractionLength(const G4Track& track,
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
    m_prevStepLenght = m_maxStepLenght;
    m_countRepeat = 0;
    m_prevPrevPrevVol = 0;
    m_prevPrevVol = 0;
    m_prevVol = 0;
  }
  if ( stepN < m_maxNumSteps ) {
    m_prevPrevPrevVol = m_prevPrevVol;
    m_prevPrevVol = m_prevVol;
    m_prevVol = currentVol;
    return proposedStep;    
  }

  // The particle took an extremely high number of steps check if the next
  // 20 steps very small steps
  double stepLenght = track.GetStepLength();

  if ( stepN == m_maxNumSteps ) {
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() << std::endl;
    std::cout << "   reached " << stepN << " num of steps for track "
              << track.GetTrackID() << ": " 
              << track.GetDefinition()->GetParticleName() << " of E_kin = "
              << track.GetKineticEnergy() << std::endl;
    std::cout << "   in volume " << currentVol->GetName() << std::endl;
  }

  if ( stepLenght <= m_prevStepLenght ) {
    // Now find if the step is a recursion between two volumes
    if ( currentVol == m_prevPrevVol && m_prevVol == m_prevPrevPrevVol ) {
      // If not a zero step reset size of previous step
      if ( stepLenght > 0.0 ) {
        m_prevStepLenght = stepLenght + m_fracTolerance*stepLenght;
      }
      m_countRepeat++;
    } 
  } else {
    m_countRepeat = 0;
  }
  
  if ( m_countRepeat > m_maxRepeatSteps ) {
    proposedStep = 0.;
    std::cout << "GAUSS::Geant4:: " <<  GetProcessName() << std::endl;
    std::cout << "   reached " << m_countRepeat << " repeated steps for track "
              << track.GetTrackID() << std::endl;
    std::cout << "   " << track.GetDefinition()->GetParticleName() 
              << " of E_kin = " << track.GetKineticEnergy() << std::endl;
    std::cout << "   " << std::endl;
    std::cout << "   at step no. " << stepN << std::endl;
    std::cout << "   last steps lenght are " << m_prevStepLenght << ", "
              << stepLenght << std::endl;
    std::cout << "   for volumes " << m_prevVol->GetName() << ", "
              << currentVol->GetName() << std::endl;
  }
  
  return proposedStep;    
}
  
//=============================================================================
// Overrides default behaviour of SpecialKillCuts not to deposit any energy
//=============================================================================
G4VParticleChange* GiGa::PingPongCut::PostStepDoIt( const G4Track& aTrack,
                                                    const G4Step& ) {

  aParticleChange.Initialize(aTrack);
  aParticleChange.ProposeEnergy(0.0) ;
  aParticleChange.ProposeLocalEnergyDeposit(0.0) ;
  aParticleChange.ProposeTrackStatus(fStopAndKill);
  return &aParticleChange;
}

//=============================================================================
