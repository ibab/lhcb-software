// ============================================================================
#ifndef    GIGA_MuonPhysics_H
#define    GIGA_MuonPhysics_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"
// G4
#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4MultipleScattering.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuIonisation.hh"
#include "G4hIonisation.hh"

#include "G4MuonMinusCaptureAtRest.hh"

// forward declarations 
template <class TYPE> class GiGaFactory;

class MuonPhysics : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<MuonPhysics>;

public:

  MuonPhysics  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~MuonPhysics();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///
  
protected:
   // Muon physics
   G4MultipleScattering   fMuPlusMultipleScattering;
   G4MuBremsstrahlung     fMuPlusBremsstrahlung ;
   G4MuPairProduction     fMuPlusPairProduction;
   G4MuIonisation         fMuPlusIonisation;

   G4MultipleScattering   fMuMinusMultipleScattering;
   G4MuBremsstrahlung     fMuMinusBremsstrahlung ;
   G4MuPairProduction     fMuMinusPairProduction;
   G4MuIonisation         fMuMinusIonisation;

   G4MuonMinusCaptureAtRest fMuMinusCaptureAtRest;

   // Tau physics
   G4MultipleScattering   fTauPlusMultipleScattering;
   G4hIonisation          fTauPlusIonisation;

   G4MultipleScattering   fTauMinusMultipleScattering;
   G4hIonisation          fTauMinusIonisation;

  G4bool wasActivated;
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_MuonPhysics_H
// ============================================================================











