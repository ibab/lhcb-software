#include "RichHpdPhotoElectricEffect.h"

#include "G4TransportationManager.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>

#include "GaudiKernel/IDataProviderSvc.h"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#include "RichG4AnalysisPhotElec.h"
RichHpdPhotoElectricEffect::RichHpdPhotoElectricEffect
( const GiGaBase* gigabase, 
  const G4String& processName): 
  G4VDiscreteProcess(processName), 
  m_numTotHpd(std::vector<int>(2)) 
{
  IDataProviderSvc* detSvc;
  StatusCode sc = gigabase->svcLoc()->service( "DetectorDataSvc", detSvc, true);

  //  m_PrePhotoElectricLogVolName="/dd/Geometry/Rich1/lvRichHPDQuartzWindow";
  //  m_PostPhotoElectricLogVolName="/dd/Geometry/Rich1/lvRichHPDPhCathode";
  //  m_HpdPhElectronKE=20*keV;
  //  m_PhCathodeToSilDetMaxDist=100.0*mm;

  m_HpdProperty = new RichHpdProperties(detSvc, gigabase->msgSvc()) ;
  m_HpdPhElectronKE=m_HpdProperty->RichHpdHighVoltage();
  m_PhCathodeToSilDetMaxDist=m_HpdProperty->RichHpdQWToSiDist(); 
  m_PrePhotoElectricLogVolName=m_HpdProperty->HpdQWLogVolName();
  m_PostPhotoElectricLogVolName=m_HpdProperty->HpdPhCathodeLogVolName();
  m_NumRichDet=m_HpdProperty->numberOfRichDetectors();
  if((int) m_numTotHpd.size() != m_NumRichDet ) 
    m_numTotHpd.resize(m_NumRichDet);
  m_numTotHpd = m_HpdProperty->numHpdTotRich();
  m_Rich1PhysVolNameA="/dd/Geometry/LHCb/lvLHCb#pvRich1Master";
  m_Rich2PhysVolNameA="/dd/Geometry/LHCb/lvLHCb#pvRich2Master";
  m_Rich1PhysVolNameB="/dd/Structure/LHCb/Rich1";
  m_Rich2PhysVolNameB="/dd/Structure/LHCb/Rich2";
  m_hpdPhCathodeInnerRadius= m_HpdProperty->HpdPhCathodeInnerRadius();
  m_MaxZHitInRich1=  m_HpdProperty->Rich1MaxZHitZCoord();

  m_MaxAnyHpdQEff =   m_HpdProperty-> HpdMaxQuantumEff();
  
  G4cout << GetProcessName() << " is created " << G4endl;
}

RichHpdPhotoElectricEffect::~RichHpdPhotoElectricEffect() {; }

G4bool RichHpdPhotoElectricEffect::IsApplicable(const G4ParticleDefinition& 
                                                aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

G4double 
RichHpdPhotoElectricEffect::GetMeanFreePath(const G4Track& ,
                                            G4double ,
                                            G4ForceCondition* condition)
{
	*condition = Forced;
  
	return DBL_MAX;
}

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

  G4int currentHpdNumber= pPreStepPoint->GetPhysicalVolume() 
    -> GetMother() -> GetMother() -> GetCopyNo();
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
    currentRichDetPhysName = pPreStepPoint->GetPhysicalVolume()
      -> GetMother() -> GetMother() -> GetMother() -> GetMother() -> GetMother()
      -> GetMother() -> GetName();
    
    if(currentRichDetPhysName != m_Rich1PhysVolNameA &&
       currentRichDetPhysName != m_Rich1PhysVolNameB ){
      G4cout << "hpd phot elec: Unknown RICH1 det Phys Name " 
             << currentRichDetPhysName << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep); 
    }
  }
  else if (currentRichDetNumber==1 ) 
    {
      currentRichDetPhysName = pPreStepPoint->GetPhysicalVolume()
        -> GetMother() -> GetMother() ->GetMother() ->GetMother() ->GetName();
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
        G4double aPhotonTime= aParticleChange.GetProperTimeChange(); 

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
      // for now apply only the linear factor of the demag;
      G4ThreeVector
        LocalElectronDirection(
                     (CurDemagFactor[0]-1.0)*LocalElectronOrigin.x()+PsfX,
                     (CurDemagFactor[0]-1.0)*LocalElectronOrigin.y()+PsfY,
                      -( m_PhCathodeToSilDetMaxDist-
                       ( m_hpdPhCathodeInnerRadius-LocalElectronOrigin.z())));
      //normalize this vector and then transform back to global coord system.
      LocalElectronDirection = LocalElectronDirection.unit();
  
      const G4ThreeVector GlobalElectronDirection = theNavigator->
        GetLocalToGlobalTransform().
				TransformAxis(LocalElectronDirection);
  

      // temporay fix for not having the shielding of hpds in Gauss. SE 14-3-2003.
     
      //      G4double ElecKineEnergy= m_HpdPhElectronKE;
  
      //create the photoelectron
            G4double ElecKineEnergy= 100000*m_HpdPhElectronKE;
             G4DynamicParticle* aElectron= 
                     new G4DynamicParticle (G4Electron::Electron(),
                           GlobalElectronDirection, ElecKineEnergy) ;

      // end of temporary fix.
      //     G4DynamicParticle* aElectron=
      //       new G4DynamicParticle (RichPhotoElectron::PhotoElectron(),
      //                         GlobalElectronDirection, ElecKineEnergy) ;

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
      


      // Kill the incident photon when it has converted to photoelectron.
      // G4cout<<"Now killing photon which is converted"<<G4endl;
      aParticleChange.SetLocalEnergyDeposit(PhotonEnergy);
      //   aParticleChange.SetEnergyChange(0.);  
      aParticleChange.SetStatusChange(fStopAndKill); 

    } else {
      //photon is not to be killed if it is not converted to photoelectron. This is to
      // simulate the reflections at the Silicon surface.  For now the reflectivity of
      // silicon is 0. This may be changed in the future.

      // for now for test the incident photon is killed SE 13-3-02
      //  G4cout<<"Now  killing photon which is not converted"<<G4endl;
      aParticleChange.SetLocalEnergyDeposit(PhotonEnergy);
      // aParticleChange.SetEnergyChange(0.);  
      aParticleChange.SetStatusChange(fStopAndKill); 
    }
  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}

double RichHpdPhotoElectricEffect::getCurrentHpdQE(int ihpdnum, 
                                                   int richdetnum,  
                                                   double photonenergy){

  //convert from MeV to eV
  double photonenergyeV=photonenergy*1000000.0   ;

  return m_HpdProperty->getRichHpdQE(ihpdnum,richdetnum)->
                            getHpdQEffFromPhotEnergy(photonenergyeV);

}

void RichHpdPhotoElectricEffect::setHpdPhElectronKE(double HpdPhElecKE) {
  m_HpdPhElectronKE=HpdPhElecKE;
}
void RichHpdPhotoElectricEffect::setPhCathodeToSilDetMaxDist
                                   (double PhCathodeToSilZ){
  m_PhCathodeToSilDetMaxDist= PhCathodeToSilZ;
}
double RichHpdPhotoElectricEffect::getCurrentHpdPSFSigma
                                  (int hpdnumb, int richdetnumb){
  return m_HpdProperty->getRichHpdPSF(hpdnumb,richdetnumb)
                                        ->hpdPointSpreadFunction();

}
std::vector<double>  RichHpdPhotoElectricEffect::getCurrentHpdDemag
                                 (int hpdnumber, int richdetnumber){

  return m_HpdProperty->getRichHpdDeMag(hpdnumber,richdetnumber)->
                                            HpdDemagVect();

}
void RichHpdPhotoElectricEffect::setPrePhotoElectricLogVolName
                               (G4String PrePhotoElecLogVolName){

  m_PrePhotoElectricLogVolName= PrePhotoElecLogVolName;
}
void RichHpdPhotoElectricEffect::setPostPhotoElectricLogVolName
                               (G4String PostPhotoElecLogVolName){
  m_PostPhotoElectricLogVolName=PostPhotoElecLogVolName;
} 
void  RichHpdPhotoElectricEffect::setNumRichDet(int numrichDetect ) {
  m_NumRichDet= numrichDetect;

}
int RichHpdPhotoElectricEffect::numTotHpdInCurrentRichDet(int richdetnum) {
  int curHpdTot=0;
  if(richdetnum >=0 &&   richdetnum < (int) m_numTotHpd.size()  ) {
    curHpdTot = m_numTotHpd[richdetnum];
  }
  return  curHpdTot;
}
void RichHpdPhotoElectricEffect::sethpdPhCathodeInnerRadius(double hpdphcInnr ){
  m_hpdPhCathodeInnerRadius= hpdphcInnr;

}

void RichHpdPhotoElectricEffect::setMaxZHitInRich1(double MaxZHitWithinRich1 ){
  m_MaxZHitInRich1 =  MaxZHitWithinRich1;

}


