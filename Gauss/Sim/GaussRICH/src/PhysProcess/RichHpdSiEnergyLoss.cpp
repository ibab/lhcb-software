#include "RichHpdSiEnergyLoss.h"
#include "G4Material.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include <algorithm>
#include <math.h>
#include <vector>
RichHpdSiEnergyLoss::RichHpdSiEnergyLoss(const G4String& processName)
  :G4VEnergyLoss(processName),  MinKineticEnergy(1.*keV),
   MipEnergyHpdSiEloss(1.0*GeV),finalRangeforSiDetStep(0.15*mm),
   PhElectronMaxEnergy(25.0*keV),  SiHitDetGlobalEff(0.9), 
   PeBackScaProb(0.18)  {


  G4String materialName="/dd/Materials/RichMaterials/RichHpdSilicon";
  G4String EnvelopeMaterialName="/dd/Materials/RichMaterials/Kovar";

  //  SiHitDetGlobalEff=0.9;
  
  static const G4MaterialTable* theMaterialTable = 
    G4Material::GetMaterialTable();
  HpdSiElossMaterialName= materialName;
  HpdEnvelopeMaterialName =  EnvelopeMaterialName;
  G4int numberOfMat= theMaterialTable->size() ;
  G4int iMat, iMatk;
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

  for(iMatk=0;iMatk<numberOfMat;iMatk++) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMatk]->GetName()<<G4endl;

    if ( EnvelopeMaterialName == (*theMaterialTable)[iMatk]->GetName()){
      fMatIndexHpdEnvelopeKovar=(*theMaterialTable)[iMatk]->GetIndex();
             G4cout<<"Hpd Si energy Loss construt Material "<<materialName
          <<"   "<< fMatIndexHpdEnvelopeKovar<<G4endl;
      break;
    }
   
  }
  if(iMatk >= numberOfMat ) {
    G4Exception("Invalid material Name in RichHpdSiEnergyLoss constructor" );
  }


     G4cout<<GetProcessName() <<" is created "<<G4endl;
  
}
RichHpdSiEnergyLoss::~RichHpdSiEnergyLoss() {; }


G4bool RichHpdSiEnergyLoss::IsApplicable(const G4ParticleDefinition& 
                                         aParticleType) {

  //  return(aParticleType.GetPDGCharge()!= 0.);
  return(( aParticleType.GetPDGCharge()!= 0.) && 
         (aParticleType.GetParticleName() == "e-"));

}

G4double RichHpdSiEnergyLoss::GetContinuousStepLimit(const G4Track& track,
                                                     G4double previousStepSize,
                                                     G4double currentMinimumStep,
                                                     G4double& currentSafety){

  G4double  RangeForStep =  finalRangeforSiDetStep;

   if( fMatIndexHpdSiEloss != (G4int) track.GetMaterial() -> GetIndex() && 
    fMatIndexHpdEnvelopeKovar != (G4int) track.GetMaterial() -> GetIndex() ) { 
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
  if(fMatIndexHpdSiEloss !=  (G4int) aTrack.GetMaterial()->GetIndex() && 
     fMatIndexHpdEnvelopeKovar != (G4int) aTrack.GetMaterial() -> GetIndex() ) {
    return &aParticleChange;
  }

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4double aKinEnergyInit = aParticle->GetKineticEnergy();
  //   G4cout<<"Now particle  "<<aParticle->GetDefinition()->GetParticleName()
  //       << "in HpdEnergyloss KE= "<<aKinEnergyInit <<G4endl;
  // if the Photoelectron hits the kovar of the Hpd envelope
  // endcap or barrel,  then the kill the photoelectron. 
  // G4ParticleDefinition* aParticleDef = aParticle-> GetDefinition();
    G4String  aCreatorProcessName= "NullProcess";
    const G4VProcess* aProcess = aTrack.GetCreatorProcess();
    if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
  if( fMatIndexHpdEnvelopeKovar == (G4int) aTrack.GetMaterial() -> GetIndex()) {
    if(aCreatorProcessName == "RichHpdPhotoelectricProcess" ) {
       aParticleChange.SetStatusChange(fStopAndKill);
    }  
    return &aParticleChange;
  }
  

  // if any charged particle (which can be a photoelectron or any other
  // charged particle) hits the silicon detector of the hpd, then store some
  // energy in the Silicon detector of the hpd.
  G4double Eloss, aKinEnergyFinal;
  // the following is just an initial fast approximation of the
  // energy loss in Silicon. This need to be modified in the future.
  // The following does not yet simulate the 1/beta**2 dependence of
  // the energy loss with respect to the incident energy.
  // The following is sufficient for the normal photoelectron
  // incidence on the Silicon detector.
  //
      // temporary fix until the magnetic shielding is implemented in
      //Gauss. SE 14-3-2003. The energy of the photoelectron is made to
      // be 20 GeV just to avoid the problem of field in richhpdphotoelectric effect.
      // once the shielding is implemented, this will be put back to 20 keV.


  // The following factor is  a temporary fix to distringuish
  // between the energy depostied by other proceseses and this
  // process. The other processes typicaly deposit about 60 KeV
  // So this process deposits 20*10 = 200 keV.  
  G4double EnegydepositMultFactor=1.0;
  
     if(aCreatorProcessName == "RichHpdPhotoelectricProcess" ) {
          if(aKinEnergyInit > 1500 ) {
            aKinEnergyInit= aKinEnergyInit/100000;
            EnegydepositMultFactor=10.0;
            
          }
          
    }
      //end of temporary fix.

  if(aKinEnergyInit < MinKineticEnergy ) {  Eloss=0.0 ; }
  else if( aKinEnergyInit <= PhElectronMaxEnergy ) {Eloss= aKinEnergyInit ;}
  else { Eloss = std::min(aKinEnergyInit,MipEnergyHpdSiEloss); }
 
  aKinEnergyFinal=aKinEnergyInit-Eloss;

  // Now for the hit detection effecidency and backscattering etc.

  //    aParticleChange.SetLocalEnergyDeposit(Eloss);  

   const G4double aEnergyAlreadyDeposit =  
               aStep.GetTotalEnergyDeposit();
   //   cout<<"Hpd sidet Energy already deposited  "
   //             <<aEnergyAlreadyDeposit<<endl;

  G4double EnergyTransfer=  
       EnegydepositMultFactor* RichHpdSiEnergyDeposit(Eloss);
  //  cout<<"EnergyTransfer in sidetEloss " << EnergyTransfer<<endl;

  
  if(   EnergyTransfer > 0.0 ) {
    
    aParticleChange.SetLocalEnergyDeposit(EnergyTransfer);  
  }

  
  if (aKinEnergyFinal <= MinKineticEnergy ) {
    aParticleChange.SetStatusChange(fStopAndKill);
  
  }else { 
    aParticleChange.SetEnergyChange(aKinEnergyFinal);

  }
  return &aParticleChange;

}
G4double RichHpdSiEnergyLoss::RichHpdSiEnergyDeposit(G4double   ElossInput) 
{
  G4double NetEnergyTransfer =    ElossInput;
   
    G4double Effrandom =  G4UniformRand();
    //  cout<<"SiEnergy dep  EFFR GloablEFF  "
    //    << Effrandom << "  "<< SiHitDetGlobalEff<<endl;
    
    if( Effrandom >  SiHitDetGlobalEff ) {
       
      NetEnergyTransfer= 0.0;
      
    }
    
    
    //  G4double EnergyFromPe =  ElossInput;
  
    // G4double EnergyTransfer = RichHpdSiBackScatter(  EnergyFromPe)  
    //  NetEnergyTransfer =  EnergyTransfer;
  
    
    return NetEnergyTransfer;
  
}

G4double RichHpdSiEnergyLoss::RichHpdSiBackScatter (G4double EnergyInput) 
{
  return EnergyInput;
  
}






