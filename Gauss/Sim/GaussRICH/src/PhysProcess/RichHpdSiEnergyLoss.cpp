#include "RichHpdSiEnergyLoss.h"
#include "G4Material.hh"
#include "Randomize.hh"
#include <algorithm>
#include <math.h>
#include <vector>
RichHpdSiEnergyLoss::RichHpdSiEnergyLoss(const G4String& processName)
  :G4VEnergyLoss(processName),  MinKineticEnergy(1.*keV),
   MipEnergyHpdSiEloss(1.0*GeV),finalRangeforSiDetStep(0.15*mm),
   PhElectronMaxEnergy(25.0*keV) {


  G4String materialName="/dd/Materials/RichMaterials/RichHpdSilicon";
  static const G4MaterialTable* theMaterialTable = 
    G4Material::GetMaterialTable();
  HpdSiElossMaterialName= materialName;
  G4int numberOfMat= theMaterialTable->size() ;
  G4int iMat;
  for(iMat=0;iMat<numberOfMat;iMat++) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;

    if ( materialName == (*theMaterialTable)[iMat]->GetName()){
      fMatIndexHpdSiEloss=(*theMaterialTable)[iMat]->GetIndex();
      //        G4cout<<"Hpd Si energy Loss construt Material "<<materialName
      //    <<"   "<< fMatIndexHpdSiEloss<<G4endl;
      break;
    }
   
  }
  if(iMat >= numberOfMat ) {
    G4Exception("Invalid material Name in RichHpdSiEnergyLoss constructor" );
  }
  //   G4cout<<GetProcessName() <<" is created "<<G4endl;
  
}
RichHpdSiEnergyLoss::~RichHpdSiEnergyLoss() {; }


G4bool RichHpdSiEnergyLoss::IsApplicable(const G4ParticleDefinition& 
                                         aParticleType) {
  return(aParticleType.GetPDGCharge()!= 0.);

}

G4double RichHpdSiEnergyLoss::GetContinuousStepLimit(const G4Track& track,
                                                     G4double previousStepSize,
                                                     G4double currentMinimumStep,
                                                     G4double& currentSafety){

  G4double  RangeForStep =  finalRangeforSiDetStep;

  if( fMatIndexHpdSiEloss != track.GetMaterial() -> GetIndex() ) { 
    RangeForStep = DBL_MAX;
  }
    
   
  return RangeForStep;

}

G4double  RichHpdSiEnergyLoss::GetMeanFreePath(const G4Track& track,
                                               G4double previousStepSize,
                                               G4ForceCondition* condition) {
  // return infinity so that it does nothing.
  *condition = NotForced;
  return DBL_MAX;

}
G4VParticleChange*  RichHpdSiEnergyLoss::PostStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {
  // Do nothing
  aParticleChange.Initialize(aTrack) ;
  return G4VContinuousDiscreteProcess::PostStepDoIt(aTrack,aStep);
   
}
G4VParticleChange* RichHpdSiEnergyLoss::AlongStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {

  aParticleChange.Initialize(aTrack);
  G4int aMaterialIndex = aTrack.GetMaterial()->GetIndex();
  if(fMatIndexHpdSiEloss != aTrack.GetMaterial()->GetIndex() ) {
    return &aParticleChange;
  }

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4double aKinEnergyInit = aParticle->GetKineticEnergy();
  G4double Eloss, aKinEnergyFinal;
  // the following is just an initial fast approximation of the
  // energy loss in Silicon. This need to be modified in the future.
  // The following does not yet simulate the 1/beta**2 dependence of
  // the energy loss with respect to the incident energy.
  // The following is sufficient for the normal photoelectron
  // incidence on the Silicon detector.
  //
  if(aKinEnergyInit < MinKineticEnergy ) {  Eloss=0.0 ; }
  else if( aKinEnergyInit <= PhElectronMaxEnergy ) {Eloss= aKinEnergyInit ;}
  else { 
    // Eloss = min(aKinEnergyInit,MipEnergyHpdSiEloss) ; 
    Eloss = aKinEnergyInit < MipEnergyHpdSiEloss ? aKinEnergyInit : MipEnergyHpdSiEloss ;
  }
  
  aKinEnergyFinal=aKinEnergyInit-Eloss;

  aParticleChange.SetLocalEnergyDeposit(Eloss);  
  if (aKinEnergyFinal <= MinKineticEnergy ) {
    aParticleChange.SetStatusChange(fStopAndKill);
  
  }else { 
    aParticleChange.SetEnergyChange(aKinEnergyFinal);

  }
  return &aParticleChange;

}







