#include <iostream>

#include "G4RunManager.hh"
#include "G4VUserPhysicsList.hh"

namespace 
{
  /** 
   * Hack to allow the generation of the configurables for Gauss.
   * Some components inherit from Geant4 classes that cannot be instantiated if
   * other structures have not been initialized first.
   * This class and its static instance should enfore the correct order if
   * loaded with LD_PRELOAD when calling genconf.
   */
  struct GenConfForG4UAction 
  {
    class GenConfG4PhysicsList: public G4VUserPhysicsList {
      virtual void SetCuts() {}
      virtual void ConstructParticle() {}
      virtual void ConstructProcess() {}
    };
    
    GenConfForG4UAction()
    {
      std::cout << "**************************************************" << std::endl;
      std::cout << "**************************************************" << std::endl;
      std::cout << "**************************************************" << std::endl;
      std::cout << "**************************************************" << std::endl;
      std::cout << "**************************************************" << std::endl;

      // Minimal G4 initialization needed by G4 actions.
      G4RunManager * runManager = new G4RunManager;
      G4VUserPhysicsList* physics = new GenConfG4PhysicsList;
      runManager->SetUserInitialization(physics);

    }
  };
  
  GenConfForG4UAction genConfG4UAction;
}
// Override the standard destructor provided by Geant4, because it undoes the special
// initialization of GenConfForG4UAction::GenConfForG4UAction().
G4VUserPhysicsList::~G4VUserPhysicsList() {}
