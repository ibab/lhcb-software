// ============================================================================
#ifndef    GIGA_IonPhysics_H
#define    GIGA_IonPhysics_H 1 
// ============================================================================
// include files 

//GaudiKernel 

// GiGa 
#include "GiGa/GiGaPhysConstructorBase.h"
// G4
#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4HadronElasticProcess.hh"
#include "G4LElastic.hh"

#include "G4DeuteronInelasticProcess.hh"
#include "G4LEDeuteronInelastic.hh"

#include "G4TritonInelasticProcess.hh"
#include "G4LETritonInelastic.hh"

#include "G4AlphaInelasticProcess.hh"
#include "G4LEAlphaInelastic.hh"

#include "G4hIonisation.hh"
#include "G4MultipleScattering.hh"

// forward declarations 
template <class TYPE> class GiGaFactory;

class IonPhysics : public GiGaPhysConstructorBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<IonPhysics>;

public:

  IonPhysics  
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  virtual ~IonPhysics();


public:
  
  /// from G4VUserPhysicsList 
  virtual void ConstructParticle ();
  virtual void ConstructProcess  ();
  ///
  
protected:
  // Elastic Process
  G4LElastic*            theElasticModel;
  
  // Generic Ion physics
  G4HadronElasticProcess theIonElasticProcess;
  G4MultipleScattering   fIonMultipleScattering;
  G4hIonisation          fIonIonisation;
  
  // Deuteron physics
  G4HadronElasticProcess      theDElasticProcess;
  G4MultipleScattering        fDeuteronMultipleScattering;
  G4hIonisation               fDeuteronIonisation;
  G4DeuteronInelasticProcess  fDeuteronProcess;
  G4LEDeuteronInelastic*      fDeuteronModel;

  // Triton physics
  G4HadronElasticProcess      theTElasticProcess;
  G4MultipleScattering        fTritonMultipleScattering;
  G4hIonisation               fTritonIonisation;
  G4TritonInelasticProcess    fTritonProcess;
  G4LETritonInelastic*        fTritonModel;
  
  // Alpha physics
  G4HadronElasticProcess      theAElasticProcess;
  G4MultipleScattering        fAlphaMultipleScattering;
  G4hIonisation               fAlphaIonisation;
  G4AlphaInelasticProcess     fAlphaProcess;
  G4LEAlphaInelastic*         fAlphaModel;
  
  // He3 physics
  G4HadronElasticProcess      theHe3ElasticProcess;
  G4MultipleScattering        fHe3MultipleScattering;
  G4hIonisation               fHe3Ionisation;
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_IonPhysics_H
// ============================================================================











