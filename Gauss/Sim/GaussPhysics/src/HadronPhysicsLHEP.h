// ============================================================================
#ifndef    GaussTools_HadronPhysicsLHEP_H
#define    GaussTools_HadronPhysicsLHEP_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

//G4/
#include "globals.hh"
#include "G4ios.hh"

#include "Packaging/G4HadronQEDBuilder.hh"
#include "Packaging/G4StoppingHadronBuilder.hh"
#include "Packaging/G4MiscLHEPBuilder.hh"

#include "Packaging/G4LHEPPiKBuilder.hh"
#include "Packaging/G4PiKBuilder.hh"

#include "Packaging/G4ProtonBuilder.hh"
#include "Packaging/G4LHEPProtonBuilder.hh"

#include "Packaging/G4NeutronBuilder.hh"
#include "Packaging/G4LHEPNeutronBuilder.hh"

// forward declarations 
template <class TYPE> class GiGaFactory;

class HadronPhysicsLHEP : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<HadronPhysicsLHEP>;

public:

  HadronPhysicsLHEP  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~HadronPhysicsLHEP();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///

private:
  G4NeutronBuilder theNeutrons;
  G4LHEPNeutronBuilder theLHEPNeutron;
  
  G4PiKBuilder thePiK;
  G4LHEPPiKBuilder theLHEPPiK;
  
  G4ProtonBuilder thePro;
  G4LHEPProtonBuilder theLHEPPro;
  
  G4MiscLHEPBuilder theMiscLHEP;
  G4StoppingHadronBuilder theStoppingHadron;
  G4HadronQEDBuilder theHadronQED;
  ///

};
// ============================================================================


// ============================================================================
#endif   ///< GaussTools_HadronPhysicsLHEP_H
// ============================================================================











