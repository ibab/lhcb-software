// ============================================================================
#ifndef    GIGA_EM_GNPhysics_H
#define    GIGA_EM_GNPhysics_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"
// G4
#include "globals.hh"
#include "G4ios.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4EMBuilder.hh"
#include "G4ElectroNuclearBuilder.hh"
// forward declarations 
template <class TYPE> class GiGaFactory;

class EM_GNPhysics : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<EM_GNPhysics>;

public:

  EM_GNPhysics  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~EM_GNPhysics();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///

private:
  G4EMBuilder theEMPhysics;
  G4ElectroNuclearBuilder theGNPhysics;

};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_EM_GNPhysics_H
// ============================================================================











