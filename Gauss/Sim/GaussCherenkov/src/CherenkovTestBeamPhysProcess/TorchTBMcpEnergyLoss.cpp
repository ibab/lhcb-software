// $Id: $
// Include files 

#include "TorchTBMcpEnergyLoss.h"
#include "G4Material.hh"
#include "G4ParticleDefinition.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4Electron.hh"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#include "TorchTBGaussPathNames.h"
#include "Randomize.hh"
#include <algorithm>
#include <math.h>
#include <vector>
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "TorchTBMcpProperties.h"


// local
#include "TorchTBMcpEnergyLoss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBMcpEnergyLoss
//
// 2012-05-25 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBMcpEnergyLoss::TorchTBMcpEnergyLoss(const G4String& processName, G4ProcessType   aType ) 
  : G4VEnergyLoss(processName, aType ),
    m_MinKineticEnergy(1.*keV),
    m_MipEnergyMcpAnodeEloss(1.0*GeV),
    m_finalRangeforAnodeStep(0.15*mm),
    m_PhElectronMaxEnergy(25.0*keV),
    m_AnodeHitDetGlobalEff(1.0),
    m_McpAnodeEff(1.0),
    m_McpAnodeChipEff(1.0)
{
  const G4String materialName="/dd/Materials/TorchMaterials/TorchMCPAnodeMaterial";
  const G4String EnvelopeMaterialName="/dd/Materials/TorchMaterials/TorchMCPEnvelopeMaterial";
  static const G4MaterialTable* theMaterialTable =
    G4Material::GetMaterialTable();
  m_McpAnodeElossMaterialName=materialName;
  m_McpEnvelopeMaterialName=EnvelopeMaterialName;
  
  G4int numberOfMat= theMaterialTable->size() ;
  G4int iMat, iMatk;
  for(iMat=0;iMat<numberOfMat;++iMat) {
    //   G4cout<<"G4mat name list "<<(*theMaterialTable)[iMat]->GetName()<<G4endl;

    if ( materialName == (*theMaterialTable)[iMat]->GetName()){
      fMatIndexMcpAnodeEloss =(*theMaterialTable)[iMat]->GetIndex();
      //   G4cout<<"Mcp Si energy Loss construt Material "<<materialName
      //    <<"   "<< fMatIndexMcpAnodeEloss<<G4endl;
      break;
    }
    
    
  }
  if(iMat >= numberOfMat ) {
    //   G4Exception("Invalid Anode material Name in TorchTBMcpSiEnergyLoss constructor" );
  }

  for(iMatk=0;iMatk<numberOfMat;++iMatk) {
    //    G4cout<<"G4mat name list "<<(*theMaterialTable)[iMatk]->GetName()<<G4endl;

    if ( EnvelopeMaterialName == (*theMaterialTable)[iMatk]->GetName()){
      fMatIndexMcpEnvelopeKovar=(*theMaterialTable)[iMatk]->GetIndex();
      //              G4cout<<"Mcp energy Loss construt Material "<<EnvelopeMaterialName
      //          <<"   "<< fMatIndexMcpEnvelopeKovar<<G4endl;
      break;
    }
    
  }
  
  
  if(iMatk >= numberOfMat ) {
    //    G4Exception("Invalid Envelope material Name in TorchTBMcpSiEnergyLoss constructor" );
  }
  
  //  G4cout<<" Mcp Energy loss fmatindex fmatenvIndex "<<  fMatIndexMcpAnodeEloss <<"    "
  //      <<fMatIndexMcpEnvelopeKovar<<G4endl;
  

}
//=============================================================================
// Destructor
//=============================================================================
TorchTBMcpEnergyLoss::~TorchTBMcpEnergyLoss() {} 

//=============================================================================
void TorchTBMcpEnergyLoss::InitializeMcpProcParam(){

    TorchTBMcpProperties *  m_McpProperty = TorchTBMcpProperties::getTorchTBMcpPropertiesInstance();

    m_McpProperty->InitializeTorchTBAnodeDetParam();

    // G4cout<<" Init stage Mcp TB anode eff "<<  m_McpAnodeEff<< "    "
    // <<m_McpAnodeChipEff<<"    "<<m_AnodeHitDetGlobalEff<<G4endl;
    
    m_AnodeDetXSize = (G4double) (m_McpProperty->TorchAnodeDetXSize());
    m_AnodeDetYSize = (G4double) (m_McpProperty->TorchAnodeDetYSize());
    m_AnodeDetZSize = (G4double) (m_McpProperty->TorchAnodeDetZSize());

    m_AnodeHitDetGlobalEff= m_McpAnodeEff * m_McpAnodeChipEff;

    //  G4cout<<" Init stageA Mcp TB anode eff "<<  m_McpAnodeEff<< "    "
    //  <<m_McpAnodeChipEff<<"    "<<m_AnodeHitDetGlobalEff<<G4endl;
}

G4bool TorchTBMcpEnergyLoss::IsApplicable(const G4ParticleDefinition&
                                         aParticleType) {

   return(aParticleType.GetPDGCharge()!= 0.);

}

G4double  TorchTBMcpEnergyLoss::GetContinuousStepLimit(const G4Track& track,
                                                     G4double /*previousStepSize*/,
                                                     G4double /*currentMinimumStep*/,
                                                     G4double& /*currentSafety*/ ) {

  G4double  RangeForStep =  m_finalRangeforAnodeStep;

  if( fMatIndexMcpAnodeEloss  != (G4int) track.GetMaterial() -> GetIndex() &&
      fMatIndexMcpEnvelopeKovar != (G4int) track.GetMaterial() -> GetIndex() ) {
      RangeForStep = DBL_MAX;
  }


  return RangeForStep;

}
G4double  TorchTBMcpEnergyLoss::GetMeanFreePath(const G4Track& /*track*/,
                                               G4double /*previousStepSize*/,
                                               G4ForceCondition* condition) {
  // return infinity so that it does nothing.
  *condition = NotForced;
  return DBL_MAX;

}

G4VParticleChange*  TorchTBMcpEnergyLoss::PostStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {
  // Do nothing
  aParticleChange.Initialize(aTrack) ;
  return G4VContinuousDiscreteProcess::PostStepDoIt(aTrack,aStep);

}

G4VParticleChange* TorchTBMcpEnergyLoss::AlongStepDoIt(const G4Track& aTrack,
                                                      const G4Step& aStep) {

  aParticleChange.Initialize(aTrack);

  //  G4cout<<" In In Mcp  Energy Loss AlongStep "<< aTrack.GetMaterial()->GetName()<<G4endl;
  

  G4int aMaterialIndex = aTrack.GetMaterial()->GetIndex();
  //  G4cout<<" Mat index in Mcp energy loss "<<aMaterialIndex<<"  "<< fMatIndexMcpAnodeEloss<<"    "
  //      << fMatIndexMcpEnvelopeKovar<<G4endl;
  
  if(fMatIndexMcpAnodeEloss !=  aMaterialIndex  &&
     fMatIndexMcpEnvelopeKovar != aMaterialIndex  ) {
    return &aParticleChange;
  }

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4double aKinEnergyInit = aParticle->GetKineticEnergy();
  G4String  aCreatorProcessName= "NullProcess";
  const G4VProcess* aProcess = aTrack.GetCreatorProcess();
  if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();

  if( fMatIndexMcpEnvelopeKovar == (G4int) aTrack.GetMaterial() -> GetIndex()) {

    //    G4cout<<" Mcp Energy Loss Process name "<<aCreatorProcessName <<G4endl;
    
    if(aCreatorProcessName == "TorchTBMcpPhotoElectricProcess" ) {
      aParticleChange.ProposeTrackStatus(fStopAndKill);
    }
    return &aParticleChange;
  }


  // if any charged particle (which can be a photoelectron or any other
  // charged particle) hits the Anode of the MCP, then store some
  // energy in the Anode of MCP.
  G4double Eloss, aKinEnergyFinal;

  // The following factor is  a temporary fix to distringuish
  // between the energy depostied by other proceseses and this
  // process. The other processes typicaly deposit about 60 KeV
  // So this process deposits 20*10 = 200 keV.
  // SE June 2004. now using RichPhotoelectron where no further deposit of
  // energy. Hence the energy deposited on the hit is 20 keV.

  G4double EnegydepositMultFactor=1.0;

  if( (aCreatorProcessName == "TorchTBMcpPhotoElectricProcess") ||  (aCreatorProcessName == "TorchTBMcpEnergyLossProcess") ) {
    if(aKinEnergyInit > 1500 ) {
      aKinEnergyInit= aKinEnergyInit/100000;

      // if( aCreatorProcessName == "RichPmtSiEnergyLossProcess" ) {
      //	G4cout<<" a backscatered eln back in RichPmtenergyLossProcess "<<G4endl;
      // }
      // EnegydepositMultFactor=10.0;
    }    
  } else {
    // G4cout<<" Now a non pe particle in RicPmtEnergyLoss  " <<G4endl;
  }
    
  //end of temporary fix.
  

  if(aKinEnergyInit < m_MinKineticEnergy ) {  Eloss=0.0 ; }
  else if( aKinEnergyInit <= m_PhElectronMaxEnergy ) {Eloss= aKinEnergyInit ;}
  else { Eloss = std::min(aKinEnergyInit,m_MipEnergyMcpAnodeEloss); }

  //  G4cout<<" MCp energy loss Eloss "<<Eloss<<G4endl;
  
  aKinEnergyFinal=aKinEnergyInit-Eloss;

  // Now for the hit detection effeciency and backscattering etc.  
  G4double EnergyTransfer=
    EnegydepositMultFactor* TorchTBMcpSiEnergyDeposit(Eloss);
  //  G4cout<<"EnergyTransfer in sidetEloss " << EnergyTransfer<<G4endl;
  if(   EnergyTransfer > 0.0 ) {

    aParticleChange.ProposeLocalEnergyDeposit(EnergyTransfer);
  }else if (aKinEnergyInit >= m_MipEnergyMcpAnodeEloss ) {
    
    aParticleChange.ProposeLocalEnergyDeposit(Eloss);
  }

  //  G4cout<<" Ke init and final MCp energyloss "<<aKinEnergyInit<<"  "<<aKinEnergyFinal<<G4endl;
  

  

   if ( (aKinEnergyFinal <= m_MinKineticEnergy) || 
      (aCreatorProcessName == "TorchTBMcpPhotoElectricProcess") ||  (aCreatorProcessName =="TorchTBMcpEnergyLossProcess")) {
       aParticleChange.ProposeTrackStatus(fStopAndKill);
       aParticleChange.ProposeEnergy(0.0);

   }else {
       aParticleChange.ProposeEnergy(aKinEnergyFinal);
 
   }


   //   G4cout<<" End Mcp energy loss "<<G4endl;
   
  
  return &aParticleChange;

}
G4double TorchTBMcpEnergyLoss::TorchTBMcpSiEnergyDeposit(G4double   ElossInput)
{
  G4double NetEnergyTransfer =    ElossInput;

  G4double Effrandom =  G4UniformRand();

  //  G4cout<<" Mcp energyloss Anode Global eff "<< m_AnodeHitDetGlobalEff<<G4endl;
  
  if( Effrandom >  m_AnodeHitDetGlobalEff ) {

    NetEnergyTransfer= 0.0;

  }

  return NetEnergyTransfer;

}

