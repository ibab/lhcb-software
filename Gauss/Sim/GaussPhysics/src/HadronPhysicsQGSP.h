// ============================================================================
#ifndef    GaussTools_HadronPhysicsQGSP_H
#define    GaussTools_HadronPhysicsQGSP_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"

//G4
#include "Packaging/G4HadronQEDBuilder.hh"
#include "Packaging/G4StoppingHadronBuilder.hh"
#include "Packaging/G4MiscLHEPBuilder.hh"

#include "Packaging/G4PiKBuilder.hh"
#include "Packaging/G4LEPPiKBuilder.hh"
#include "Packaging/G4QGSPPiKBuilder.hh"

#include "Packaging/G4ProtonBuilder.hh"
#include "Packaging/G4LEPProtonBuilder.hh"
#include "Packaging/G4QGSPProtonBuilder.hh"

#include "Packaging/G4NeutronBuilder.hh"
#include "Packaging/G4LEPNeutronBuilder.hh"
#include "Packaging/G4QGSPNeutronBuilder.hh"
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











