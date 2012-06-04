#include "RichPmtPhotoElectricEffect.h"
#include <gsl/gsl_math.h>

#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4GeometryTolerance.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>

#include "GaudiKernel/IDataProviderSvc.h"
#include "RichPhotoElectron.h"
#include "RichPEInfoAttach.h"
#include "RichG4AnalysisPhotElec.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "CkvG4GaussPathNames.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "DetDesc/DetectorElement.h"
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"
#include "RichPhotInfoAttach.h"

RichPmtPhotoElectricEffect::RichPmtPhotoElectricEffect(const GiGaBase* gigabase,
                                                       const G4String& processName,
                                                       G4ProcessType   aType)
  : G4VDiscreteProcess(processName, aType ),
    m_numTotPmt(std::vector<int>(2)),
    m_PmtQESourceTable(0)
{
    //  G4cout << GetProcessName() << " is created " << G4endl;
}

RichPmtPhotoElectricEffect::~RichPmtPhotoElectricEffect() {; }


void RichPmtPhotoElectricEffect::setPmtPhElecParam() 
{

  //    std::cout<< "RichPmtPhotoElectricEffect UsePmtMagDistortions = "
	//     <<m_UsePmtMagDistortions<<std::endl;
  //   std::cout<< "RichPmtPhotoElectricEffect PsfPreDc06Flag       = "
	//     << m_PSFPreDc06Flag <<std::endl;
     std::cout<< "RichPmtPhotoElectricEffect UseNominalPmtQE =   "<<m_PmtQEUsingNominalTable<<std::endl;

    RichPmtProperties*  m_PmtProperty = PmtProperty();
    //    m_PmtProperty -> setUsingPmtMagneticFieldDistortion((bool) m_UsePmtMagDistortions );
     m_PmtProperty -> setUseNominalPmtQE( (bool) m_PmtQEUsingNominalTable );
     m_PmtProperty -> SetCurQETableSourceOption ( m_PmtQESourceTable);
     
    m_PmtProperty -> InitializePmtProperties( );

    m_PmtPhElectronKE=m_PmtProperty->RichPmtHighVoltage();
    m_PhCathodeToSilDetMaxDist=m_PmtProperty->RichPmtQWToSiDist();
    m_PrePhotoElectricLogVolName=m_PmtProperty->PmtQWLogVolName();
    m_PostPhotoElectricLogVolName=m_PmtProperty->PmtPhCathodeLogVolName();
    m_PrePhotoElectricMatNameSec= RichPmtVacName;
    m_NumRichDet=m_PmtProperty->numberOfRichDetectors();
    if((int) m_numTotPmt.size() != m_NumRichDet )
      m_numTotPmt.resize(m_NumRichDet);
    m_numTotPmt = m_PmtProperty->numPmtTotRich();
    m_Rich1PhysVolNameA= Rich1PhysVolGeomName;
    m_Rich2PhysVolNameA= Rich2PhysVolGeomName;
    m_Rich1PhysVolNameB= Rich1DeStructurePathName;
    m_Rich2PhysVolNameB= Rich2DeStructurePathName;

    m_MaxZHitInRich1=  m_PmtProperty->Rich1MaxZHitZCoord();
    m_MaxAnyPmtQEff =   m_PmtProperty-> PmtMaxQuantumEff();
    
    // G4cout<<" Pmt Ph elect Names "<<m_PrePhotoElectricLogVolName<<"  "<<
    //  m_PostPhotoElectricLogVolName<<"   "
    //      <<m_PrePhotoElectricMatNameSec<<G4endl;
    


}

G4VParticleChange*
RichPmtPhotoElectricEffect::PostStepDoIt(const G4Track& aTrack,
                                         const G4Step& aStep)
{
  aParticleChange.Initialize(aTrack);
  // G4cout<<" Now in RichPmt Ph Elect PostStepDoit "<<G4endl;
  
  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();


  if (pPostStepPoint->GetStepStatus() != fGeomBoundary)
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  G4String PrePhName = pPreStepPoint -> GetPhysicalVolume() ->
   GetLogicalVolume() ->GetName();
  G4String PostPhName= pPostStepPoint -> GetPhysicalVolume() ->
    GetLogicalVolume() -> GetName();

  //  G4cout<<" Pmt Ph elec PrePh PostPh Names "<<PrePhName<<"  "
  //      <<PostPhName<<G4endl;
  
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

       //    G4cout<<"RichPmtPhElec effect PreVol Post Vol "<<PrePhName
       // <<"   "<<PostPhName<<G4endl;
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
    G4cout<<" zero or negative photon energy at Pmt phcathode "
          <<PhotonEnergy<<G4endl;


    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  //Now use the QE for the current PMT to determine if a
  // photoelectron should be produced or not.

  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(pPreStepPoint->GetTouchable());

  // the following modif done for the new G4 version. SE Nov,2005.
  //  CurTT -> MoveUpHistory(2);
  //  G4int currentPmtNumber= CurTT -> GetVolume() -> GetCopyNo();

  G4int currentPmtNumber= CurTT->GetReplicaNumber(2);
  //  G4cout<<" Pmt Ph elect Current Pmt Copy num "<<currentPmtNumber<<G4endl;
  
  G4int currentRichDetNumber=0;

  //Current Rich Det is found by checking the global Z coordinate
  // of the hit.
  G4double CurrentZCoord = pPreStepPoint->GetPosition().z();
  //G4cout<<" Pmt Ph elect Current Z coord "<<CurrentZCoord <<G4endl;

  if(CurrentZCoord <= 0.0 ) {
    // 0 or negative global Z coord. Something wrong.

    G4cout<<" zero or negative photon Z coord at Photocathode  "
          <<CurrentZCoord <<"     in Pmt  "<<currentPmtNumber<<G4endl;
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

  // G4cout<<"Pmt phot elec effect Z coord RichDetnum  "<< CurrentZCoord <<"  "<<currentRichDetNumber<<G4endl;
  
  // now make extra tests for the detector number.
  // These tests can be removed in the future for optimization.
  //G4cout<<" PmtPhElect  Current richdet number "<<currentRichDetNumber<<G4endl;
  
  G4String currentRichDetPhysName;
  if( currentRichDetNumber == 0 ) {
    // for rich1
    // the following modif done for the new G4 version. SE Nov,2005.

    //  CurTT -> MoveUpHistory(4);
    //  currentRichDetPhysName = CurTT -> GetVolume() -> GetName();

    currentRichDetPhysName = CurTT -> GetVolume(7)->GetName();

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
    currentRichDetPhysName = CurTT -> GetVolume(6)->GetName();

    if(currentRichDetPhysName !=  m_Rich2PhysVolNameA &&
       currentRichDetPhysName != m_Rich2PhysVolNameB ){

      G4cout << "pmt phot elec: Unknown RICH2 det Phys Name "
             << currentRichDetPhysName << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    // modif to accomodate the hpd copy numbering scheme in rich2 db. 26-10-2006

    currentPmtNumber -= m_numTotPmt[0];     
    //    G4cout<<"Current Pmt copy num in Rich2" <<currentPmtNumber <<G4endl;
    
  }
  // end of the extra tests that can be removed for optimization.


  if(currentPmtNumber >=  numTotPmtInCurrentRichDet(currentRichDetNumber) ) {
    G4cout<<"Pmt phot elec: Inadmissible pmt number for richdet. Pmtnum=  "
          <<currentPmtNumber
          <<"  for richdetnum  "<<currentRichDetNumber 
          <<"     Please check the XMLDDDB Version " <<G4endl;
    G4cout<<" Max Num Pmt in Rich1 Rich2 "<<m_numTotPmt[0]<<" "<<m_numTotPmt[1]<<G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Now for histogram Analysis stuff

  //  RichG4AnalysisPhotElecA (aStep, currentRichDetNumber,
  //                         currentPmtNumber, PhotonEnergy);
  RichG4AnalysisPhotElecA (aStep, currentRichDetNumber);
 
  // End of histogram Analysis stuff
  double CurPhCathodeQE = getCurrentPmtQE(currentPmtNumber, currentRichDetNumber,
                                          PhotonEnergy);
  G4double randomnum = G4UniformRand();
  //RWL Modif 09.11.06 and SE 26-10-2007
  //Get User information about photon

  // G4int aQW2PCreflFlag=getRichHpdQwPcReflInfo(aTrack);        
      //RWL: Don't do QE if photon has been reflected!!  

  // G4cout<<" RichPmt PhElectEff randonnum and QE for photon  " <<"  "
  //            <<randomnum<<"  "<<CurPhCathodeQE<< G4endl;
        
  //  if( aQW2PCreflFlag == 0 && randomnum* m_MaxAnyPmtQEff <  CurPhCathodeQE )
  if( randomnum* m_MaxAnyPmtQEff <  CurPhCathodeQE )  // for now ignore the reflection effects.
                                                      // to be improved later.
  {

    //Now for histogram Analysis stuff.

    //    RichG4AnalysisPhotElecB (aStep, currentRichDetNumber,
    //                         currentPmtNumber, PhotonEnergy);
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

    // std::vector<double> CurDemagFactor=
    //  getCurrentPmtDemag(currentPmtNumber,currentRichDetNumber);

    //  G4cout<<"Current demag factors  in ph elec effect "
    // <<CurDemagFactor[0]<<"  "<< CurDemagFactor[1]<<G4endl;

    //    std::vector<double>psfVectInXY = getCurrentPmtPSFXY(currentPmtNumber,currentRichDetNumber);
    std::vector<double>psfVectInXY(2,0.0);

    
    //  G4cout<<" Photoelec: Current psfX psfY "<<psfVectInXY[0]<<"  "<<psfVectInXY[1]  <<G4endl;

    G4ThreeVector LocalElectronDirection(0,0,1);

    if( UsePmtMagDistortions() ) {
     
      // LocalElectronDirection=getCathodeToAnodeDirection( currentPmtNumber,
			//				   currentRichDetNumber,
      //					   LocalElectronOrigin, psfVectInXY );

       LocalElectronDirection=getCathodeToAnodeDirectionDefault();
    }else {

    //  now apply the linear and quadratic factor of the demag;
      //    G4double ElectronCathodeRadius = sqrt( pow(LocalElectronOrigin.x(), 2) +
      //                                     pow(LocalElectronOrigin.y(), 2) );

      //      G4double ElectronCathodeRadius = LocalElectronOrigin.perp();

      // G4double scaleFact = ((CurDemagFactor[1]*ElectronCathodeRadius) +
      //                  CurDemagFactor[0]) -1.0 ;
      
         
    // CurDemagFactor[0] is a negative number.

      LocalElectronDirection.set(0.0,0.0,-1.0);
      
      //     LocalElectronDirection.set( scaleFact * (LocalElectronOrigin.x())+ psfVectInXY[0] ,
      //                         scaleFact * (LocalElectronOrigin.y()) +psfVectInXY[1] ,
      //                         -( m_PhCathodeToSilDetMaxDist-
      //                            ( m_hpdPhCathodeInnerRadius-(LocalElectronOrigin.z()))));

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

    //   G4double ElecKineEnergy= m_PmtPhElectronKE;

    //create the photoelectron
    G4double ElecKineEnergy= 100000*m_PmtPhElectronKE;

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
    //    G4double ElecKineEnergy= m_PmtPhElectronKE;

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

    // G4cout<<" RichPmtPhotoelectric effect : Now created a photoelectron with energy =  "      << ElecKineEnergy<<G4endl;


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
  // setRichHpdQwPcReflInfo(aTrack,0);


  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}

double RichPmtPhotoElectricEffect::getCurrentPmtQE(int ihpdnum,
                                                   int richdetnum,
                                                   double photonenergy){
  //convert from MeV to eV
  double photonenergyeV=photonenergy*1000000.0;
  //  return m_PmtProperty->getRichPmtQE(ihpdnum,richdetnum)->
  return PmtProperty()->getRichPmtQE(ihpdnum,richdetnum)->
    getPmtQEffFromPhotEnergy(photonenergyeV);
}
std::vector<double> RichPmtPhotoElectricEffect::getCurrentPmtPSFXY ( int hpdnumb, int richdetnumb) {
  // this gets only the first value, which is same all photon energy and radial pos
  double PsfStdSigma =  PmtProperty()->getRichPmtPSF(hpdnumb,richdetnumb)->pmtPointSpreadFunctionVectSingleValue(0);
  
  return GetPSFXYForASigma( PsfStdSigma);
}
std::vector<double> RichPmtPhotoElectricEffect::getCurrentPmtPSFFullParamXY ( int hpdnumb, int richdetnumb) {

  // this gets in the future the full value, which is a function of photon energy and radial pos
  // by interpolation.
  
  double PsfStdSigma =  PmtProperty()->getRichPmtPSF(hpdnumb,richdetnumb)->pmtPointSpreadFunctionVectSingleValue(0);

  return GetPSFXYForASigma( PsfStdSigma);

}

std::vector<double> RichPmtPhotoElectricEffect::GetPSFXYForASigma(double aPSFsigma ) {
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

  //  if( PSFPreDc06Flag() ) { 
  //  G4double PsfRandomAzimuth = twopi*G4UniformRand();
  //  G4double PsfRandomRad= G4RandGauss::shoot(0.0,aPSFsigma);
    //    G4double PsfX= fabs(PsfRandomRad)*cos( PsfRandomAzimuth);
    //  G4double PsfY= fabs(PsfRandomRad)*sin( PsfRandomAzimuth);
  //  apsfXY[0]= fabs(PsfRandomRad)*cos( PsfRandomAzimuth);
  //  apsfXY[1]= fabs(PsfRandomRad)*sin( PsfRandomAzimuth);
  // }else {
    //  G4double PsfRandomX= G4RandGauss::shoot(0.0,aPSFsigma);
    //G4double PsfRandomY= G4RandGauss::shoot(0.0,aPSFsigma);
    // apsfXY[0]=  PsfRandomX; 
    // apsfXY[1]= PsfRandomY;
   apsfXY[0]= G4RandGauss::shoot(0.0,aPSFsigma);
   apsfXY[1]= G4RandGauss::shoot(0.0,aPSFsigma);
   //  }
  return apsfXY;
}
//G4ThreeVector RichPmtPhotoElectricEffect::getCathodeToAnodeDirection(int ihpdnum,
//                                                      int richdetnum,G4ThreeVector v,
//                                                      std::vector<double>apsfVectInXY ){

G4ThreeVector RichPmtPhotoElectricEffect::getCathodeToAnodeDirectionDefault(){
  


  // v = starting position on cathode
  //  G4double ElectronCathodeRadius = v.perp();
  //G4double ElectronCathodePhi    = v.phi();
  // if(ElectronCathodePhi < 0.0)  ElectronCathodePhi =  2 * M_PI + ElectronCathodePhi;

  //demagnify:
  
  G4ThreeVector ElectronAnode = G4ThreeVector(0.0,0.0,-1.0);
  G4ThreeVector aLocalElectronDirection = G4ThreeVector(0.0,0.0,-1.0);
  
  //  G4ThreeVector ElectronAnode = PmtProperty()
  //  ->getRichPmtDeMag(ihpdnum, richdetnum)
  //  ->getPositionOnAnode( ElectronCathodeRadius, ElectronCathodePhi );
 
  //find direction for the electron to follow
  // G4ThreeVector aLocalElectronDirection( ElectronAnode.x() +  apsfVectInXY[0]- v.x(),
  //					ElectronAnode.y() + apsfVectInXY[1] - v.y(),
  //					-( m_PhCathodeToSilDetMaxDist-
  //					   ( m_hpdPhCathodeInnerRadius - (v.z()) )));
  //G4cout<<"NEW: Local eln origin dir "
  //	<<aLocalElectronDirection.x()<<" "<<aLocalElectronDirection.y()
  //	<<" "<<aLocalElectronDirection.z()<<G4endl;

  //  return aLocalElectronDirection.unit();
  return aLocalElectronDirection;

}
