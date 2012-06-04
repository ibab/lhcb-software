// $Id: $
// Include files 

#include <gsl/gsl_math.h>

#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
//#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4GeometryTolerance.hh"
#include "TorchTBGaussPathNames.h"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#
#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>


// local
#include "TorchTBMcpPhotoElectricEffect.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBMcpPhotoElectricEffect
//
// 2012-05-18 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBMcpPhotoElectricEffect::TorchTBMcpPhotoElectricEffect( const GiGaBase* gigabase,
                                                       const G4String& processName,
                                                              G4ProcessType   aType )
  :G4VDiscreteProcess(processName, aType ),
   m_numTotMcp(1) 
{
  //    G4cout << GetProcessName() << " is created " << G4endl;

}
//=============================================================================
// Destructor
//=============================================================================
TorchTBMcpPhotoElectricEffect::~TorchTBMcpPhotoElectricEffect() {} 

//=============================================================================
void TorchTBMcpPhotoElectricEffect::setMcpPhElecParam() 
{
  
  TorchTBMcpProperties*  aMcpProperty = McpProperty() ;
  aMcpProperty  -> InitializeTorchTBMcpProperties ( );
  m_PrePhotoElectricLogVolName = aMcpProperty ->TorchTBMcpQWLogVolName();
  m_PostPhotoElectricLogVolName= aMcpProperty ->TorchTBMcpPhCathodeLogVolName();

  G4cout<<" TorchMCP pre Posl Log Vol "<<m_PrePhotoElectricLogVolName <<"   "
        <<m_PostPhotoElectricLogVolName<<G4endl;
  
  
    // m_PrePhotoElectricMatNameSec=
  m_numTotMcp= aMcpProperty->numMcpInTorchTB();
  
  m_MaxAnyMcpQEff=aMcpProperty->TorchTBMcpMaxQuantumEff();
  m_McpPhElectronKE=aMcpProperty->TorchTBMcpHighVoltage();
  m_elecOrigTolerence= 0.004*mm;
  
  // m_PhCathodeToAnodeDetMaxDist=

}
G4VParticleChange*
TorchTBMcpPhotoElectricEffect::PostStepDoIt(const G4Track& aTrack,
                                         const G4Step& aStep){


  //  G4cout<<" Now starting Torch MCP photo electric "<<G4endl;
  
  aParticleChange.Initialize(aTrack);
  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
  if (pPostStepPoint->GetStepStatus() != fGeomBoundary)
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  G4String PrePhName = pPreStepPoint -> GetPhysicalVolume() ->
   GetLogicalVolume() ->GetName();
  G4String PostPhName= pPostStepPoint -> GetPhysicalVolume() ->
    GetLogicalVolume() -> GetName();

  //      G4cout<<"Now at start of  TorchMCp photoelectic proc "<<PrePhName<<"  "<<PostPhName<<G4endl;

  if( (PrePhName == m_PrePhotoElectricLogVolName) &&
      (PostPhName == m_PostPhotoElectricLogVolName) ) {
    


    
  }else {

    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    
  }
  //  G4cout<<"Now in TorchMCp photoelectic proc "<<PrePhName<<"  "<<PostPhName<<G4endl;
  
  G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  if (aTrack.GetStepLength()<=kCarTolerance/2) {
    //  G4cout<<"Too small step length at hpd  ph cathode boundary "
    //     <<aTrack.GetStepLength()<<G4endl;

    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  const G4DynamicParticle* aDynamicPhoton = aTrack.GetDynamicParticle();
  G4double PhotonEnergy = aDynamicPhoton->GetKineticEnergy();
  if(PhotonEnergy <= 0.0 ) {
    G4cout<<" zero or negative photon energy at Mcp phcathode "
          <<PhotonEnergy<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(pPreStepPoint->GetTouchable());

  G4int currentMcpNumber= CurTT->GetReplicaNumber(2);
  //  G4cout<<" Photoelctirc Proc current MCP Number "<<currentMcpNumber <<G4endl;
  
  if( currentMcpNumber >=   m_numTotMcp ) {     
      G4cout<<"Mcp phot elec: Inadmissible pmt number for. TorchTB  Mcpnum=  "
          <<currentMcpNumber <<"     Please check the XMLDDDB Version " <<G4endl;
      G4cout<<" Max Num Mcp in TorchTB  "<<m_numTotMcp  <<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  
  double CurPhCathodeQE=  getCurrentMcpQE(currentMcpNumber, PhotonEnergy);
  //  G4cout<<" Current Ph cath QE "<<CurPhCathodeQE<<G4endl;
  
  G4double randomnum = G4UniformRand();
  
  if( randomnum* m_MaxAnyMcpQEff <  CurPhCathodeQE )  // for now ignore the reflection effects..
  {

    //    G4cout<<" Torch MCP Now creating the photo electron "<<G4endl;
    
    G4double aPhotonTime= aTrack.GetGlobalTime();
    G4ThreeVector GlobalElectronOrigin= pPostStepPoint->GetPosition();
    G4Navigator* theNavigator =
      G4TransportationManager::GetTransportationManager()->
      GetNavigatorForTracking();
    G4ThreeVector LocalElectronOriginInit = theNavigator->
      GetGlobalToLocalTransform().
      TransformPoint(GlobalElectronOrigin);
    // Shift the electron origin by 0.004 mm to avoid multiple scattering in the
    // photocathode.
    G4ThreeVector LocalElectronOrigin (LocalElectronOriginInit.x(),
                                       LocalElectronOriginInit.y(),
                                       LocalElectronOriginInit.z()- m_elecOrigTolerence );

     std::vector<double>psfVectInXY(2,0.0);

     G4ThreeVector LocalElectronDirection(0.0,0.0,-1.0);
     LocalElectronDirection = LocalElectronDirection.unit();
     const G4ThreeVector GlobalElectronDirection = theNavigator->
      GetLocalToGlobalTransform().
      TransformAxis(LocalElectronDirection);
      G4double ElecKineEnergy= 100000*m_McpPhElectronKE;
      G4DynamicParticle* aElectron=
        new G4DynamicParticle (RichPhotoElectron::PhotoElectron(),
                             GlobalElectronDirection, ElecKineEnergy) ;
      aParticleChange.SetNumberOfSecondaries(1) ;
    G4Track* aSecPETrack =
      new G4Track(aElectron,aPhotonTime,GlobalElectronOrigin);
    aSecPETrack->SetTouchableHandle((G4VTouchable*)0);
    aSecPETrack->SetParentID(aTrack.GetTrackID());
    aSecPETrack->SetGoodForTrackingFlag(true);
    //    G4cout<<"Tagging Ph electron "<<G4endl;
    
    G4Track* aTaggedSecPETrack = RichPEInfoAttach(aTrack,aSecPETrack,LocalElectronOrigin);
    aParticleChange.AddSecondary(aTaggedSecPETrack);
    aParticleChange.ProposeLocalEnergyDeposit(PhotonEnergy);
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    //  G4cout<<"Tagging Ph electron tagged "<<G4endl;
    
    

  }else {
    
    //photon is not killed if it is not converted to photoelectron

    
  }

  //  G4cout<<" end of Ph elec proc "<<G4endl;
  
  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  
  
  
}
double  TorchTBMcpPhotoElectricEffect::getCurrentMcpQE(int imcpnum,double photonenergy) 
{
   //convert from MeV to eV
  double photonenergyeV=photonenergy*1000000.0;
  return McpProperty()->getTorchTBMcpQE(imcpnum)->
    getMcpQEffFromPhotEnergy(photonenergyeV); 
}
