// ============================================================================
#ifndef    GIGA_EMPhysics_H
#define    GIGA_EMPhysics_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"
// G4
#include "globals.hh"
#include "G4ios.hh"
#include "G4VPhysicsConstructor.hh"
#include "Packaging/G4EMBuilder.hh"

// forward declarations 
template <class TYPE> class GiGaFactory;

class EMPhysics : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<EMPhysics>;

public:

  EMPhysics  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~EMPhysics();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///

private:
  G4EMBuilder theEMPhysics;
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_EMPhysics_H
// ============================================================================











