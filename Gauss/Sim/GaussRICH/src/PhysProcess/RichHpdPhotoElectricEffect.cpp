#include "RichHpdPhotoElectricEffect.h"

#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>

#include "GaudiKernel/IDataProviderSvc.h"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#include "RichG4AnalysisPhotElec.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"

RichHpdPhotoElectricEffect::RichHpdPhotoElectricEffect(const GiGaBase* gigabase,
                                  const G4String& processName, 
                                  G4ProcessType   aType)
  : G4VDiscreteProcess(processName, aType ),
    m_numTotHpd(std::vector<int>(2))
{
  IDataProviderSvc* detSvc;
  if ( gigabase->svcLoc()->service( "DetectorDataSvc", detSvc, true) ) {

    //  m_PrePhotoElectricLogVolName="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichHPDQuartzWindow";
    //  m_PostPhotoElectricLogVolName="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichHPDPhCathode";
    //  m_HpdPhElectronKE=20*keV;
    //  m_PhCathodeToSilDetMaxDist=111.2*mm;

    m_HpdProperty = new RichHpdProperties(detSvc, gigabase->msgSvc()) ;
    m_HpdPhElectronKE=m_HpdProperty->RichHpdHighVoltage();
    m_PhCathodeToSilDetMaxDist=m_HpdProperty->RichHpdQWToSiDist();
    m_PrePhotoElectricLogVolName=m_HpdProperty->HpdQWLogVolName();
    m_PostPhotoElectricLogVolName=m_HpdProperty->HpdPhCathodeLogVolName();
    m_NumRichDet=m_HpdProperty->numberOfRichDetectors();
    if((int) m_numTotHpd.size() != m_NumRichDet )
      m_numTotHpd.resize(m_NumRichDet);
    m_numTotHpd = m_HpdProperty->numHpdTotRich();
    m_Rich1PhysVolNameA= Rich1PhysVolGeomName;
    m_Rich2PhysVolNameA= Rich2PhysVolGeomName;
    m_Rich1PhysVolNameB= Rich1PhysVolDeName;
    m_Rich2PhysVolNameB= Rich2PhysVolDeName;
    m_hpdPhCathodeInnerRadius= m_HpdProperty->HpdPhCathodeInnerRadius();
    m_MaxZHitInRich1=  m_HpdProperty->Rich1MaxZHitZCoord();

    m_MaxAnyHpdQEff =   m_HpdProperty-> HpdMaxQuantumEff();

    //  G4cout << GetProcessName() << " is created " << G4endl;
  }

}

RichHpdPhotoElectricEffect::~RichHpdPhotoElectricEffect() {; }



G4VParticleChange*
RichHpdPhotoElectricEffect::PostStepDoIt(const G4Track& aTrack,
                                         const G4Step& aStep)
{
  aParticleChange.Initialize(aTrack);

  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();


  if (pPostStepPoint->GetStepStatus() != fGeomBoundary)
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  G4String PrePhName = pPreStepPoint -> GetPhysicalVolume() ->
    GetLogicalVolume() ->GetName();
  G4String PostPhName= pPostStepPoint -> GetPhysicalVolume() ->
    GetLogicalVolume() -> GetName();

  if(( PrePhName == m_PrePhotoElectricLogVolName &&
       PostPhName == m_PostPhotoElectricLogVolName ) ||
     ( PostPhName == m_PrePhotoElectricLogVolName  &&
       PrePhName == m_PostPhotoElectricLogVolName )) {
    //   G4cout<<"RichHpdPhElec effect PreVol Post Vol "<<PrePhName
    //     <<"   "<<PostPhName<<G4endl;
  }else {

    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

  }

  if (aTrack.GetStepLength()<=kCarTolerance/2) {
    G4cout<<"Too small step length at hpd  ph cathode boundary "
          <<aTrack.GetStepLength()<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  const G4DynamicParticle* aDynamicPhoton = aTrack.GetDynamicParticle();
  G4double PhotonEnergy = aDynamicPhoton->GetKineticEnergy();

  if(PhotonEnergy <= 0.0 ) {
    G4cout<<" zero or negative photon energy at Hpd phcathode "
          <<PhotonEnergy<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  //Now use the QE for the current HPD to determine if a
  // photoelectron should be produced or not.

  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(pPreStepPoint->GetTouchable());

  // the following modif done for the new G4 version. SE Nov,2005.
  //  CurTT -> MoveUpHistory(2);
  //  G4int currentHpdNumber= CurTT -> GetVolume() -> GetCopyNo();

  G4int currentHpdNumber= CurTT->GetReplicaNumber(2);
  G4int currentRichDetNumber=0;

  //Current Rich Det is found by checking the global Z coordinate
  // of the hit.
  G4double CurrentZCoord = pPreStepPoint->GetPosition().z();

  if(CurrentZCoord <= 0.0 ) {
    // 0 or negative global Z coord. Something wrong.

    G4cout<<" zero or negative photon Z coord at Photocathode  "
          <<CurrentZCoord <<"     in Hpd  "<<currentHpdNumber<<G4endl;
  }
  else if(CurrentZCoord < m_MaxZHitInRich1 )
  {
    // we are in Rich1
    currentRichDetNumber=0;
  }
  else
  {
    // we are in Rich2
    currentRichDetNumber=1;
  }

  // now make extra tests for the detector number.
  // These tests can be removed in the future for optimization.
  G4String currentRichDetPhysName;
  if( currentRichDetNumber == 0 ) {
    // for rich1
  // the following modif done for the new G4 version. SE Nov,2005.

  //  CurTT -> MoveUpHistory(4);
  //  currentRichDetPhysName = CurTT -> GetVolume() -> GetName();
    
    currentRichDetPhysName = CurTT -> GetVolume(6)->GetName();

    if(currentRichDetPhysName != m_Rich1PhysVolNameA &&
       currentRichDetPhysName != m_Rich1PhysVolNameB ){
      G4cout << "hpd phot elec Proc: Unknown RICH1 det Phys Name "
             << currentRichDetPhysName << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
  }
  else if (currentRichDetNumber==1 )
  {
  // the following modif done for the new G4 version. SE Nov,2005.

  //  CurTT -> MoveUpHistory(2);
  //  currentRichDetPhysName = CurTT -> GetVolume() -> GetName();

    currentRichDetPhysName = CurTT -> GetVolume(4)->GetName();

    if(currentRichDetPhysName !=  m_Rich2PhysVolNameA &&
       currentRichDetPhysName != m_Rich2PhysVolNameB ){

      G4cout << "hpd phot elec: Unknown RICH2 det Phys Name "
             << currentRichDetPhysName << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
  }
  // end of the extra tests that can be removed for optimization.


  if(currentHpdNumber >=  numTotHpdInCurrentRichDet(currentRichDetNumber) ) {
    G4cout<<"Hpd phot elec: Inadmissible hpd number for richdet. Hpdnum= "
          <<currentHpdNumber
          <<"  for richdetnum  "<<currentRichDetNumber<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Now for histogram Analysis stuff

  RichG4AnalysisPhotElecA (aStep, currentRichDetNumber,
                           currentHpdNumber, PhotonEnergy);

  // End of histogram Analysis stuff
  double CurPhCathodeQE = getCurrentHpdQE(currentHpdNumber, currentRichDetNumber,
                                          PhotonEnergy);
  G4double randomnum = G4UniformRand();

  if( randomnum* m_MaxAnyHpdQEff <  CurPhCathodeQE )
  {

    //Now for histogram Analysis stuff.

    RichG4AnalysisPhotElecB (aStep, currentRichDetNumber,
                             currentHpdNumber, PhotonEnergy);

    // End of histogram Analysis stuff.
    //  G4double aPhotonTime= aParticleChange.GetProperTimeChange();
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
    G4double ElecOriginTolerence= 0.004*mm;
    G4ThreeVector LocalElectronOrigin (LocalElectronOriginInit.x(),
                                       LocalElectronOriginInit.y(),
                 LocalElectronOriginInit.z()- ElecOriginTolerence );

    std::vector<double> CurDemagFactor=
      getCurrentHpdDemag(currentHpdNumber,currentRichDetNumber);
    //  G4cout<<"Current demag factors "
    // <<CurDemagFactor[0]<<"  "<< CurDemagFactor[1]<<G4endl;
    // now get the Point Spread function.
    G4double PSFsigma=
      getCurrentHpdPSFSigma(currentHpdNumber,currentRichDetNumber);

    G4double PsfRandomAzimuth = twopi*G4UniformRand();
    G4double PsfRandomRad= G4RandGauss::shoot(0.0,PSFsigma);
    G4double PsfX= PsfRandomRad*cos( PsfRandomAzimuth);
    G4double PsfY= PsfRandomRad*sin( PsfRandomAzimuth);
    // G4cout<<" Photoelec: Current psfSigma psfX psfY "<<PSFsigma
    //      <<"    "<< PsfX<<"   "<< PsfY<<G4endl;

    //  now apply only the linear and quadratic factor of the demag;
    G4double ElectronCathodeRadius = sqrt( pow(LocalElectronOrigin.x(), 2) +
                                             pow(LocalElectronOrigin.y(), 2) );
    G4double scaleFact = ((CurDemagFactor[1]*ElectronCathodeRadius) +  
                                         CurDemagFactor[0]) -1.0 ;
    // CurDemagFactor[0] is a negative number.
       
     G4ThreeVector
      LocalElectronDirection ( scaleFact * (LocalElectronOrigin.x()) + PsfX,
                               scaleFact * (LocalElectronOrigin.y()) + PsfY,
                               -( m_PhCathodeToSilDetMaxDist-
                             ( m_hpdPhCathodeInnerRadius-(LocalElectronOrigin.z()))));

    // test printout
    // G4cout<<"Photoeleceff: Local eln origin dir "<<
    //   LocalElectronOrigin.x()<<"   "<< LocalElectronOrigin.y()<<
    //   "    "<<      LocalElectronOrigin.z()<<"   "<<
    //    LocalElectronDirection.x()<<"   "<<   LocalElectronDirection.y()<<
    //   "    "<< LocalElectronDirection.z()<<G4endl;
    
    //normalize this vector and then transform back to global coord system.
    LocalElectronDirection = LocalElectronDirection.unit();

    //  G4cout<<"Photoeleceff: normalized Local eln  dir "<<
    //   LocalElectronDirection.x()<<"   "<<   LocalElectronDirection.y()<<
    //   "    "<< LocalElectronDirection.z()<<G4endl;
       
    const G4ThreeVector GlobalElectronDirection = theNavigator->
      GetLocalToGlobalTransform().
      TransformAxis(LocalElectronDirection);


    // temporay fix for not having the shielding of hpds in Gauss. SE 14-3-2003.

    //   G4double ElecKineEnergy= m_HpdPhElectronKE;

    //create the photoelectron
      G4double ElecKineEnergy= 100000*m_HpdPhElectronKE;

      // G4DynamicParticle* aElectron=
      // new G4DynamicParticle (G4Electron::Electron(),
      //                       GlobalElectronDirection, ElecKineEnergy) ;
    //
    // end of temporary fix.
    //   test of number of proc for the photoelectron particle
    // G4cout<<" stat test for photoelectron "<<G4endl;
    // 
    //      G4ParticleDefinition* photoelectronDef = 
    //      RichPhotoElectron::PhotoElectron();
    //    G4cout<<" Now checking if photoelectron def exists "<<G4endl;
    //    G4String pname= photoelectronDef->GetParticleName();
    //    G4cout<<" Now checking if photoelectron def exists "<<
    //      pname<< G4endl;
    //  
    //    if( photoelectronDef) {
    //      
    //     G4ProcessManager* pmanager =  photoelectronDef->GetProcessManager();
    //     G4cout<< "Check if pmanager exists "<<G4endl;
    //     if(pmanager) {  
    //     G4ProcessVector* pVector = 
    //          pmanager->GetProcessList();
    //      G4cout << "size of ProcList for pe-   "
    //             << (G4int) pVector->size()<< G4endl;
    //      G4cout << "List of phys proc for pe-   " << G4endl;          
    //      pmanager->DumpInfo();
    //     }
    //     
    //    }       
    //      G4cout<<" end test for photoelectron "<<G4endl;
    //      
    // end of test of number of proc for photoelectron particle.	   
    //
    //    G4double ElecKineEnergy= m_HpdPhElectronKE;

           G4DynamicParticle* aElectron=
             new G4DynamicParticle (RichPhotoElectron::PhotoElectron(),
                       GlobalElectronDirection, ElecKineEnergy) ;

    aParticleChange.SetNumberOfSecondaries(1) ;
    //  aParticleChange.AddSecondary( aElectron ) ;
    // the following line replaced by the following set of lines.
    //      aParticleChange.AddSecondary( aElectron,GlobalElectronOrigin,true ) ;

    // now create a track and add the track directly to the list of
    // secondaries.
    G4Track* aSecPETrack =
      new G4Track(aElectron,aPhotonTime,GlobalElectronOrigin);
    aSecPETrack->SetTouchableHandle((G4VTouchable*)0);
    aSecPETrack->SetParentID(aTrack.GetTrackID());
    aSecPETrack->SetGoodForTrackingFlag(true);
    G4Track* aTaggedSecPETrack = RichPEInfoAttach(aTrack,aSecPETrack);
    aParticleChange.AddSecondary(aTaggedSecPETrack);

    //   G4cout<<" RichHpdPhotoelectric effect : Now created a photoelectron with energy =  "
    //      << ElecKineEnergy<<G4endl;


    // Kill the incident photon when it has converted to photoelectron.
    // G4cout<<"Now killing photon which is converted"<<G4endl;
    aParticleChange.ProposeLocalEnergyDeposit(PhotonEnergy);
    //   aParticleChange.SetEnergyChange(0.);
    aParticleChange.ProposeTrackStatus(fStopAndKill);

  } else {

    //photon is not killed if it is not converted to photoelectron                                                              

                                                      
  }
  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}

double RichHpdPhotoElectricEffect::getCurrentHpdQE(int ihpdnum,
                                                   int richdetnum,
                                                   double photonenergy){
  //convert from MeV to eV
  double photonenergyeV=photonenergy*1000000.0;
  return m_HpdProperty->getRichHpdQE(ihpdnum,richdetnum)->
    getHpdQEffFromPhotEnergy(photonenergyeV);
}



