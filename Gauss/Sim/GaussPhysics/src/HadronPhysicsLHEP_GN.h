// ============================================================================
#ifndef    GaussTools_HadronPhysicsLHEP_GN_H
#define    GaussTools_HadronPhysicsLHEP_GN_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

//G4
#include "G4HadronQEDBuilder.hh"
#include "G4StoppingHadronBuilder.hh"
#include "G4MiscLHEPBuilder.hh"
#include "G4LHEPPiKBuilder.hh"
#include "G4PiKBuilder.hh"
#include "G4LHEPProtonBuilder.hh"
#include "G4ProtonBuilder.hh"
#include "G4LHEPNeutronBuilder.hh"
#include "G4NeutronBuilder.hh"

// forward declarations 
template <class TYPE> class GiGaFactory;

class HadronPhysicsLHEP_GN : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<HadronPhysicsLHEP_GN>;

public:

  HadronPhysicsLHEP_GN  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~HadronPhysicsLHEP_GN();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///

private:
  G4NeutronBuilder theNeutrons;
  G4LHEPNeutronBuilder theLHEPNeutron;
  G4LHEPPiKBuilder theLHEPPiK;
  G4PiKBuilder thePiK;
  G4LHEPProtonBuilder theLHEPProton;
  G4ProtonBuilder theProton;
  G4MiscLHEPBuilder theMiscLHEP;
  G4StoppingHadronBuilder theStoppingHadron;
  G4HadronQEDBuilder theHadronQED;
  ///

};
// ============================================================================


// ============================================================================
#endif   ///< GaussTools_HadronPhysicsLHEP_GN_H
// ============================================================================











