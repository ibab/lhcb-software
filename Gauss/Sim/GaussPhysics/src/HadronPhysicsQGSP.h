// ============================================================================
#ifndef    GaussTools_HadronPhysicsQGSP_H
#define    GaussTools_HadronPhysicsQGSP_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

//G4
#include "G4HadronQEDBuilder.hh"
#include "G4StoppingHadronBuilder.hh"
#include "G4MiscLHEPBuilder.hh"

#include "G4PiKBuilder.hh"
#include "G4LEPPiKBuilder.hh"
#include "G4QGSPPiKBuilder.hh"

#include "G4ProtonBuilder.hh"
#include "G4LEPProtonBuilder.hh"
#include "G4QGSPProtonBuilder.hh"

#include "G4NeutronBuilder.hh"
#include "G4LEPNeutronBuilder.hh"
#include "G4QGSPNeutronBuilder.hh"
// forward declarations 
template <class TYPE> class GiGaFactory;

class HadronPhysicsQGSP : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<HadronPhysicsQGSP>;

public:

  HadronPhysicsQGSP  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~HadronPhysicsQGSP();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///

private:
    G4NeutronBuilder theNeutrons;
    G4LEPNeutronBuilder theLEPNeutron;
    G4QGSPNeutronBuilder theQGSPNeutron;
    
    G4PiKBuilder thePiK;
    G4LEPPiKBuilder theLEPPiK;
    G4QGSPPiKBuilder theQGSPPiK;
    
    G4ProtonBuilder thePro;
    G4LEPProtonBuilder theLEPPro;
    G4QGSPProtonBuilder theQGSPPro;    
    
    G4MiscLHEPBuilder theMiscLHEP;
    G4StoppingHadronBuilder theStoppingHadron;
    G4HadronQEDBuilder theHadronQED;
  ///

};
// ============================================================================


// ============================================================================
#endif   ///< GaussTools_HadronPhysicsQGSP_H
// ============================================================================











