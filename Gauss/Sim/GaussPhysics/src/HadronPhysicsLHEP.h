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

#include "G4HadronQEDBuilder.hh"
#include "G4StoppingHadronBuilder.hh"
#include "G4MiscLHEPBuilder.hh"

#include "G4LHEPPiKBuilder.hh"
#include "G4PiKBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4LHEPProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4LHEPNeutronBuilder.hh"

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











