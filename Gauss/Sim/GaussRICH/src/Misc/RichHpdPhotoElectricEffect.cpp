#include "GaussRICH/RichHpdPhotoElectricEffect.h"
#include <gsl/gsl_math.h>

#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4GeometryTolerance.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaussRICH/RichPhotoElectron.h"
#include "GaussRICH/RichPEInfoAttach.h"
#include "GaussRICH/RichG4AnalysisPhotElec.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "DetDesc/DetectorElement.h"
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichPhotInfoAttach.h"

RichHpdPhotoElectricEffect::RichHpdPhotoElectricEffect(const GiGaBase *  /* gigabase  */,
                                                       const G4String& processName,
                                                       G4ProcessType   aType)
  : G4VDiscreteProcess(processName, aType ),
    m_numTotHpd(std::vector<int>(2))
{
    //  G4cout << GetProcessName() << " is created " << G4endl;
}

RichHpdPhotoElectricEffect::~RichHpdPhotoElectricEffect() {; }


void RichHpdPhotoElectricEffect::setHpdPhElecParam() 
{

    std::cout<< "RichHpdPhotoElectricEffect UseHpdMagDistortions = "
	     <<m_UseHpdMagDistortions<<std::endl;
    std::cout<< "RichHpdPhotoElectricEffect PsfPreDc06Flag       = "
	     << m_PSFPreDc06Flag <<std::endl;
     std::cout<< "RichHpdPhotoElectricEffect UseNominalHpdQE =   "<<m_HpdQEUsingNominalTable<<std::endl;

     std::cout<< "RichHpd Hit Smearing Flag and Values  =   "<< m_activateRichHitSmear <<"  "<< m_RichAerogelHitSmearVal <<"  "
              <<m_Rich1GasHitSmearVal<<"   "<<m_Rich2GasHitSmearVal<<std::endl;

    RichHpdProperties*  m_HpdProperty = HpdProperty();
    m_HpdProperty -> setUsingHpdMagneticFieldDistortion((bool) m_UseHpdMagDistortions );
    m_HpdProperty -> setUseNominalHpdQE( (bool) m_HpdQEUsingNominalTable );
    m_HpdProperty -> SetHpdSmearValueVect(m_RichAerogelHitSmearVal, m_Rich1GasHitSmearVal, m_Rich2GasHitSmearVal );
    m_HpdProperty -> InitializeHpdProperties( );

    m_HpdPhElectronKE=m_HpdProperty->RichHpdHighVoltage();
    m_PhCathodeToSilDetMaxDist=m_HpdProperty->RichHpdQWToSiDist();
    m_PrePhotoElectricLogVolName=m_HpdProperty->HpdQWLogVolName();
    m_PostPhotoElectricLogVolName=m_HpdProperty->HpdPhCathodeLogVolName();
    m_PrePhotoElectricMatNameSec= RichHpdVacName;
    m_NumRichDet=m_HpdProperty->numberOfRichDetectors();
    if((int) m_numTotHpd.size() != m_NumRichDet )
      m_numTotHpd.resize(m_NumRichDet);
    m_numTotHpd = m_HpdProperty->numHpdTotRich();
    m_Rich1PhysVolNameA= Rich1PhysVolGeomName;
    m_Rich2PhysVolNameA= Rich2PhysVolGeomName;
    m_Rich1PhysVolNameB= Rich1DeStructurePathName;
    m_Rich2PhysVolNameB= Rich2DeStructurePathName;
    m_hpdPhCathodeInnerRadius= m_HpdProperty->HpdPhCathodeInnerRadius();
    m_MaxZHitInRich1=  m_HpdProperty->Rich1MaxZHitZCoord();
    m_MaxAnyHpdQEff =   m_HpdProperty-> HpdMaxQuantumEff();
    



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


  //   if(( (PrePhName == m_PrePhotoElectricLogVolName) &&
  //      (PostPhName == m_PostPhotoElectricLogVolName) ) ||
  //    ( (PostPhName == m_PrePhotoElectricLogVolName)  &&
  //     (PrePhName == m_PostPhotoElectricLogVolName) )) {

     if(( (PrePhName == m_PrePhotoElectricLogVolName) &&
        (PostPhName == m_PostPhotoElectricLogVolName) ) ||
        ( (PrePhName == m_PrePhotoElectricMatNameSec )  &&
          (PostPhName == m_PostPhotoElectricLogVolName) )) {



  // temporary test with only qw-pc photons allowed to convert
  //if  if(( PostPhName == m_PostPhotoElectricLogVolName ) ) {
    // end of temporary test

    //  G4cout<<"RichHpdPhElec effect PreVol Post Vol "<<PrePhName
    //    <<"   "<<PostPhName<<G4endl;
  }else {


    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

  }

  //GC kCarTolerance removed from G4 9.0, class G4GeometryTolerance to
  //   be used instead so add the following line. kCarTolerance can be
  //   adjusted before setting up the geometry
  G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
  if (aTrack.GetStepLength()<=kCarTolerance/2) {
    //  G4cout<<"Too small step length at hpd  ph cathode boundary "
    //     <<aTrack.GetStepLength()<<G4endl;

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

    //    currentRichDetPhysName = CurTT -> GetVolume(4)->GetName();
    // the following change made in accordance with the change in the 
    // rich2 geometry in January 2008, where an extra layer
    // is added as a container of the array of hpds.
    currentRichDetPhysName = CurTT -> GetVolume(5)->GetName();

    if(currentRichDetPhysName !=  m_Rich2PhysVolNameA &&
       currentRichDetPhysName != m_Rich2PhysVolNameB ){

      G4cout << "hpd phot elec: Unknown RICH2 det Phys Name "
             << currentRichDetPhysName << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    // modif to accomodate the hpd copy numbering scheme in rich2 db. 26-10-2006

    currentHpdNumber -= m_numTotHpd[0];     

  }
  // end of the extra tests that can be removed for optimization.


  if(currentHpdNumber >=  numTotHpdInCurrentRichDet(currentRichDetNumber) ) {
    G4cout<<"Hpd phot elec: Inadmissible hpd number for richdet. Hpdnum=  "
          <<currentHpdNumber
          <<"  for richdetnum  "<<currentRichDetNumber 
          <<"     Please check the XMLDDDB Version " <<G4endl;
    G4cout<<" Max Num Hpd in Rich1 Rich2 "<<m_numTotHpd[0]<<" "<<m_numTotHpd[1]<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Now for histogram Analysis stuff

  //  RichG4AnalysisPhotElecA (aStep, currentRichDetNumber,
  //                         currentHpdNumber, PhotonEnergy);
  RichG4AnalysisPhotElecA (aStep, currentRichDetNumber);
 
  // End of histogram Analysis stuff
  double CurPhCathodeQE = getCurrentHpdQE(currentHpdNumber, currentRichDetNumber,
                                          PhotonEnergy);
  G4double randomnum = G4UniformRand();
  //RWL Modif 09.11.06 and SE 26-10-2007
  //Get User information about photon

  G4int aQW2PCreflFlag=getRichHpdQwPcReflInfo(aTrack);        
      //RWL: Don't do QE if photon has been reflected!!  

      //       G4cout<<" reflection flag and randonnum and QE for photon  "<< aQW2PCreflFlag <<"  "
      //        <<randomnum<<"  "<<CurPhCathodeQE<< G4endl;
        
  if( aQW2PCreflFlag == 0 && randomnum* m_MaxAnyHpdQEff <  CurPhCathodeQE )
  {

    //Now for histogram Analysis stuff.

    //    RichG4AnalysisPhotElecB (aStep, currentRichDetNumber,
    //                         currentHpdNumber, PhotonEnergy);
    RichG4AnalysisPhotElecB (aStep, currentRichDetNumber);

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

    //  G4cout<<"Current demag factors  in ph elec effect "
    // <<CurDemagFactor[0]<<"  "<< CurDemagFactor[1]<<G4endl;

    std::vector<double>psfVectInXY (2);

    if(m_activateRichHitSmear) {
      int currentRadiatorNumber=2;
      if(currentRichDetNumber == 0) {
        currentRadiatorNumber=RichPhotTkRadiatorNumber(aTrack);
        if(currentRadiatorNumber > 2 )currentRadiatorNumber=0;
      }
      
      double hitSmearValue = getCurrentHitSmearSigma(currentHpdNumber,
                                                     currentRichDetNumber,
                                                    currentRadiatorNumber);
      
      psfVectInXY=GetPSFXYForASigma(hitSmearValue);
      
    } else {
       
      psfVectInXY = getCurrentHpdPSFXY(currentHpdNumber,currentRichDetNumber);
      
    }
    
    
    //  G4cout<<" Photoelec: Current psfX psfY "<<psfVectInXY[0]<<"  "<<psfVectInXY[1]  <<G4endl;

    G4ThreeVector LocalElectronDirection(0,0,1);

    if( UseHpdMagDistortions() ) {
     
       LocalElectronDirection=getCathodeToAnodeDirection( currentHpdNumber,
							   currentRichDetNumber,
							   LocalElectronOrigin, psfVectInXY );
    }else {

    //  now apply the linear and quadratic factor of the demag;
      //    G4double ElectronCathodeRadius = sqrt( pow(LocalElectronOrigin.x(), 2) +
      //                                     pow(LocalElectronOrigin.y(), 2) );

      G4double ElectronCathodeRadius = LocalElectronOrigin.perp();

     G4double scaleFact = ((CurDemagFactor[1]*ElectronCathodeRadius) +
                        CurDemagFactor[0]) -1.0 ;

    // CurDemagFactor[0] is a negative number.

    
     LocalElectronDirection.set( scaleFact * (LocalElectronOrigin.x()) +psfVectInXY[0] ,
                               scaleFact * (LocalElectronOrigin.y()) +psfVectInXY[1] ,
                               -( m_PhCathodeToSilDetMaxDist-
                                  ( m_hpdPhCathodeInnerRadius-(LocalElectronOrigin.z()))));

    }
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

    //  G4DynamicParticle* aElectron=
    //  new G4DynamicParticle (G4Electron::Electron(),
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
    G4Track* aTaggedSecPETrack = RichPEInfoAttach(aTrack,aSecPETrack,LocalElectronOrigin);
    aParticleChange.AddSecondary(aTaggedSecPETrack);

    //  G4cout<<" RichHpdPhotoelectric effect : Now created a photoelectron with energy =  "
    //      << ElecKineEnergy<<G4endl;


    // Kill the incident photon when it has converted to photoelectron.
    // G4cout<<"Now killing photon which is converted"<<G4endl;
    aParticleChange.ProposeLocalEnergyDeposit(PhotonEnergy);
    //   aParticleChange.SetEnergyChange(0.);
    //      aParticleChange.ProposeEnergy(0.0);
    aParticleChange.ProposeTrackStatus(fStopAndKill);

  } else {

    //photon is not killed if it is not converted to photoelectron


  }

  //RWL modif 09.11.06
  //Always set reflection flag back to 0 at end of QE process
   setRichHpdQwPcReflInfo(aTrack,0);


  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}

double RichHpdPhotoElectricEffect::getCurrentHpdQE(int ihpdnum,
                                                   int richdetnum,
                                                   double photonenergy){
  //convert from MeV to eV
  double photonenergyeV=photonenergy*1000000.0;
  //  return m_HpdProperty->getRichHpdQE(ihpdnum,richdetnum)->
  return HpdProperty()->getRichHpdQE(ihpdnum,richdetnum)->
    getHpdQEffFromPhotEnergy(photonenergyeV);
}
std::vector<double> RichHpdPhotoElectricEffect::getCurrentHpdPSFXY ( int hpdnumb, int richdetnumb) {
  // this gets only the first value, which is same all photon energy and radial pos
  double PsfStdSigma =  HpdProperty()->getRichHpdPSF(hpdnumb,richdetnumb)->hpdPointSpreadFunctionVectSingleValue(0);
  
  return GetPSFXYForASigma( PsfStdSigma);
}
std::vector<double> RichHpdPhotoElectricEffect::getCurrentHpdPSFFullParamXY ( int hpdnumb, int richdetnumb) {

  // this gets in the future the full value, which is a function of photon energy and radial pos
  // by interpolation.
  
  double PsfStdSigma =  HpdProperty()->getRichHpdPSF(hpdnumb,richdetnumb)->hpdPointSpreadFunctionVectSingleValue(0);

  return GetPSFXYForASigma( PsfStdSigma);

}
double RichHpdPhotoElectricEffect::getCurrentHitSmearSigma (int hpdnumb, 
    int richdetnumb, int radiatorNum ){
  double hitsmearSigma = 
   HpdProperty()->getRichHpdPSF(hpdnumb,richdetnumb)->hitSmearSingleValue(radiatorNum);
  double apsfSigma= 
    HpdProperty()->getRichHpdPSF(hpdnumb,richdetnumb)->hpdPointSpreadFunctionVectSingleValue(0);
  
  double aTotalSigma= pow((hitsmearSigma*hitsmearSigma+apsfSigma*apsfSigma),0.5);
  

  return  aTotalSigma;
  
}

std::vector<double> RichHpdPhotoElectricEffect::GetPSFXYForASigma(double aPSFsigma ) {
  std::vector<double> apsfXY(2);
    // now get the Point Spread function.
    //  Typical width in r =0.2 mm
    // which means for the predc06 option 
    // example for +x side it is (0.1 mm)*cos(phi) and -x side it is
    // also (0.1 mm)*cos(phi) giving a total of (0.2 mm) *cos(phi). 
    // in when non-PSFPreDc06Flag option, it is sampled in X,Y independently.
    //
   //
  // G4cout<<"Current PSF sigma "<< aPSFSigma<<G4endl;

  if( PSFPreDc06Flag() ) { 
    G4double PsfRandomAzimuth = twopi*G4UniformRand();
    G4double PsfRandomRad= G4RandGauss::shoot(0.0,aPSFsigma);
    //    G4double PsfX= fabs(PsfRandomRad)*cos( PsfRandomAzimuth);
    //  G4double PsfY= fabs(PsfRandomRad)*sin( PsfRandomAzimuth);
    apsfXY[0]= fabs(PsfRandomRad)*cos( PsfRandomAzimuth);
    apsfXY[1]= fabs(PsfRandomRad)*sin( PsfRandomAzimuth);
  }else {
    //  G4double PsfRandomX= G4RandGauss::shoot(0.0,aPSFsigma);
    //G4double PsfRandomY= G4RandGauss::shoot(0.0,aPSFsigma);
    // apsfXY[0]=  PsfRandomX; 
    // apsfXY[1]= PsfRandomY;
   apsfXY[0]= G4RandGauss::shoot(0.0,aPSFsigma);
   apsfXY[1]= G4RandGauss::shoot(0.0,aPSFsigma);
  }
  return apsfXY;
}
G4ThreeVector RichHpdPhotoElectricEffect::getCathodeToAnodeDirection(int ihpdnum,
                                                                     int richdetnum,
    								     G4ThreeVector v,
                                                      std::vector<double>apsfVectInXY ){

  // v = starting position on cathode
  G4double ElectronCathodeRadius = v.perp();
  G4double ElectronCathodePhi    = v.phi();
  if(ElectronCathodePhi < 0.0)  ElectronCathodePhi =  2 * M_PI + ElectronCathodePhi;

  //demagnify:
  
  G4ThreeVector ElectronAnode = HpdProperty()
    ->getRichHpdDeMag(ihpdnum, richdetnum)
    ->getPositionOnAnode( ElectronCathodeRadius, ElectronCathodePhi );
 
  //find direction for the electron to follow
  G4ThreeVector aLocalElectronDirection( ElectronAnode.x() +  apsfVectInXY[0]- v.x(),
					ElectronAnode.y() + apsfVectInXY[1] - v.y(),
					-( m_PhCathodeToSilDetMaxDist-
					   ( m_hpdPhCathodeInnerRadius - (v.z()) )));
  //G4cout<<"NEW: Local eln origin dir "
  //	<<aLocalElectronDirection.x()<<" "<<aLocalElectronDirection.y()
  //	<<" "<<aLocalElectronDirection.z()<<G4endl;

  //  return aLocalElectronDirection.unit();
  return aLocalElectronDirection;

}
