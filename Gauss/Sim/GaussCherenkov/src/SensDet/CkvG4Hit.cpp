// $Id: $
// Include files 

#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4Transform3D.hh"
#include "G4LogicalVolume.hh"


// local
#include "GaussCherenkov/CkvG4Hit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CkvG4Hit
//
// 2011-03-18 : Sajan Easo
//-----------------------------------------------------------------------------
G4Allocator<CkvG4Hit> CkvG4HitAllocator;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvG4Hit::CkvG4Hit(const CkvG4Hit & right  ) 
  :GaussHitBase(right),
   m_CurModuleNum(right.m_CurModuleNum),
   m_CurHitInPixelGap(right.m_CurHitInPixelGap),
   m_CurPmtNum(right.m_CurPmtNum),
    m_edep ( right.m_edep ),
    m_GlobalPos ( right.m_GlobalPos ),
    m_LocalPos ( right.m_LocalPos ),
    m_GlobalPEOriginPos(right.m_GlobalPEOriginPos ),
    m_LocalPEOriginPos(right.m_LocalPEOriginPos ),
    m_CurRichNum ( right.m_CurRichNum ),
    m_CurSectorNum ( right.m_CurSectorNum ),
    m_CurPixelXNum ( right.m_CurPixelXNum ),
    m_CurPixelYNum ( right.m_CurPixelYNum ),
    m_OptPhotID ( right.m_OptPhotID ),
    m_ChTrackID ( right.m_ChTrackID ),
    m_PETrackID ( right.m_PETrackID ),
    m_ChTrackPDG( right.m_ChTrackPDG ),
    m_PETrackPDG( right.m_PETrackPDG ),
    m_RadiatorNumber(right.m_RadiatorNumber ),
    m_PhotEmisPt(right.m_PhotEmisPt ),
    m_PhotEnergyAtProd(right.m_PhotEnergyAtProd ),
    m_ThetaCkvAtProd(right.m_ThetaCkvAtProd ),
    m_PhiCkvAtProd(right.m_PhiCkvAtProd ),
    m_ChTrackTotMom(right.m_ChTrackTotMom ),
    m_ChTrackMomVect(right.m_ChTrackMomVect ),
    m_RichHitGlobalTime ( right.m_RichHitGlobalTime ),
    m_RichChTrackMass ( right.m_RichChTrackMass ),
    m_ChTrackCkvPreStepPos(right.m_ChTrackCkvPreStepPos ),
    m_ChTrackCkvPostStepPos(right.m_ChTrackCkvPostStepPos ),
    m_OptPhotRayleighFlag(right.m_OptPhotRayleighFlag ),
    m_OptPhotAgelExitPos ( right.m_OptPhotAgelExitPos ),
    m_Mirror1PhotonReflPosition(right.m_Mirror1PhotonReflPosition),
    m_Mirror2PhotonReflPosition(right.m_Mirror2PhotonReflPosition),
    m_Mirror1PhotonDetectorCopyNum(right.m_Mirror1PhotonDetectorCopyNum),
    m_Mirror2PhotonDetectorCopyNum(right.m_Mirror2PhotonDetectorCopyNum),
    m_RichVerboseHitInfo(right.m_RichVerboseHitInfo ),
    m_RichHpdQW2PhCathReflFlag(right.m_RichHpdQW2PhCathReflFlag),
    m_ElectronBackScatterFlag(right.m_ElectronBackScatterFlag),
    m_PhotoElectricProductionFlag(right.m_PhotoElectricProductionFlag),
    m_RichHpdPhotonReflectionFlag(right.m_RichHpdPhotonReflectionFlag),
    m_HpdQuartzWindowExtSurfPhotIncidentPosition(right.m_HpdQuartzWindowExtSurfPhotIncidentPosition),
    m_PhotonSourceProcessInfo(right.m_PhotonSourceProcessInfo)   
{ }

const CkvG4Hit& CkvG4Hit::operator=(const CkvG4Hit &right)
{
  m_CurModuleNum=right.m_CurModuleNum;
  m_CurHitInPixelGap=right.m_CurHitInPixelGap;
  m_CurPmtNum= right.m_CurPmtNum;
  m_edep = right.m_edep;
  m_GlobalPos = right.m_GlobalPos;
  m_LocalPos = right.m_LocalPos;
  m_GlobalPEOriginPos=right.m_GlobalPEOriginPos;
  m_LocalPEOriginPos=right.m_LocalPEOriginPos;
  m_CurRichNum = right.m_CurRichNum;
  m_CurSectorNum = right.m_CurSectorNum;
  m_CurPixelXNum = right.m_CurPixelXNum;
  m_CurPixelYNum = right.m_CurPixelYNum;
  m_OptPhotID = right.m_OptPhotID;
  m_ChTrackID = right.m_ChTrackID;
  m_ChTrackPDG= right.m_ChTrackPDG;
  m_PETrackID = right.m_PETrackID;
  m_PETrackPDG= right.m_PETrackPDG;
  m_RadiatorNumber=right. m_RadiatorNumber;
  m_PhotEmisPt=right.m_PhotEmisPt;
  m_PhotEnergyAtProd=right.m_PhotEnergyAtProd;
  m_ThetaCkvAtProd=right.m_ThetaCkvAtProd;
  m_PhiCkvAtProd=right.m_PhiCkvAtProd;
  m_ChTrackTotMom=right.m_ChTrackTotMom;
  m_ChTrackMomVect=right.m_ChTrackMomVect;
  m_RichHitGlobalTime = right.m_RichHitGlobalTime;
  m_RichChTrackMass = right.m_RichChTrackMass;
  m_ChTrackCkvPreStepPos=right.m_ChTrackCkvPreStepPos;
  m_ChTrackCkvPostStepPos=right.m_ChTrackCkvPostStepPos;
  m_OptPhotRayleighFlag=right.m_OptPhotRayleighFlag;
  m_OptPhotAgelExitPos = right.m_OptPhotAgelExitPos;
  m_Mirror1PhotonReflPosition=right.m_Mirror1PhotonReflPosition;
  m_Mirror2PhotonReflPosition=right.m_Mirror2PhotonReflPosition;
  m_Mirror1PhotonDetectorCopyNum=right.m_Mirror1PhotonDetectorCopyNum;
  m_Mirror2PhotonDetectorCopyNum=right.m_Mirror2PhotonDetectorCopyNum;
  m_RichVerboseHitInfo=right.m_RichVerboseHitInfo;
  m_RichHpdQW2PhCathReflFlag=right.m_RichHpdQW2PhCathReflFlag;
  m_ElectronBackScatterFlag=right.m_ElectronBackScatterFlag;
  m_PhotoElectricProductionFlag=right.m_PhotoElectricProductionFlag;
  m_RichHpdPhotonReflectionFlag=right.m_RichHpdPhotonReflectionFlag;
  m_HpdQuartzWindowExtSurfPhotIncidentPosition=right.m_HpdQuartzWindowExtSurfPhotIncidentPosition;
  m_PhotonSourceProcessInfo=right.m_PhotonSourceProcessInfo;
 
  return *this;
  
}
int CkvG4Hit::operator==(const CkvG4Hit &right) const
{
  //  return 0;
  
  return((m_CurRichNum==right.m_CurRichNum)&&(m_CurPmtNum==right.m_CurPmtNum)
         &&(m_CurPixelXNum==right.m_CurPixelXNum)&&
         (m_CurPixelYNum==right.m_CurPixelYNum ) );

}
void CkvG4Hit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(m_GlobalPos);
    circle.SetScreenSize(5.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(0.0,0.5,0.8);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
  else
  {
    G4cout<<"No VisManager to draw Rich Hits " <<G4endl;
  }

}
void  CkvG4Hit::DrawPEOrigin() {
   G4VVisManager* pVVisManagerA = G4VVisManager::GetConcreteInstance();
  if(pVVisManagerA)
  {
    G4Circle circlePEO(m_GlobalPEOriginPos);
    circlePEO.SetScreenSize(0.04);
    circlePEO.SetFillStyle(G4Circle::filled);
    G4Colour colourPEO(0.0,0.5,0.8);
    G4VisAttributes attribsPEO(colourPEO);
    circlePEO.SetVisAttributes(attribsPEO);
    pVVisManagerA->Draw(circlePEO);
  }
  else
  {
    G4cout<<"No VisManager to draw Rich PEOrigin from Hits " <<G4endl;
  }
}
void CkvG4Hit::Print() 
{
  G4cout<<" CkvG4Hit PmtModuleNumber" <<  m_CurModuleNum <<G4endl;
  G4cout<<" CkvG4Hit PmtHitInPixelGap " <<m_CurHitInPixelGap <<G4endl;
  G4cout<<" CkvG4Hit current Pmt number "<<m_CurPmtNum<<G4endl;

   G4cout<<"RichG4Hit: "<<" Rich Sect pmt  Pix: "<<m_CurRichNum<<"   "
        <<m_CurSectorNum<<"   "<<m_CurPmtNum<<"    "<<m_CurPixelXNum
        <<m_CurPixelYNum<<G4endl;
  G4cout<<"RichG4Hit: "<<" Ener GlobalPos LocalPos:   "<<m_edep<<"       "
        << m_GlobalPos <<"         "<< m_LocalPos
        <<" Global PEOrigin:   "<<m_GlobalPEOriginPos
        <<" Local PEORigin:   "<<m_LocalPEOriginPos <<G4endl;
  G4cout<<"RichG4Hit: "<<" Radiator number: "<<m_RadiatorNumber
        <<" EmissPt X Y Z:   "<<m_PhotEmisPt.x()<<"      "
        <<m_PhotEmisPt.y()<<"     "<<m_PhotEmisPt.z()<<G4endl;

  G4cout<<"RichG4Hit:  "<<"AtProduction CherenkovTheta = "<< m_ThetaCkvAtProd
        <<G4endl;
  G4cout<<"RichG4Hit:  "<<"AtProduction CherenkovPhi ="<< m_PhiCkvAtProd
        <<G4endl;

  G4cout<<"RichG4Hit:  "<<"OpticalPhotonEnergy MotherChTrackMom: "<<
    m_PhotEnergyAtProd<<"     "<< m_ChTrackTotMom<<G4endl;


  G4cout<<"RichG4Hit: "<<"TrackID: MotherCh OptPhot PE: "
        << m_ChTrackID<<"     "<< m_OptPhotID<<"    "
        << m_PETrackID<< "    MotherChTrack PDGCode =   "
        <<m_ChTrackPDG<< G4endl;

  G4cout<<"RichHit Global Time = "
        <<m_RichHitGlobalTime<<G4endl;


  G4cout<<" RichG4Hit Verbose Hit Output Flag:  "
        << m_RichVerboseHitInfo<<G4endl;

  if(   m_RichVerboseHitInfo > 0 ) {

    G4cout<<"RichG4Hit: "<<"MotherChTrack MomVect XYZ  Mass :  "
          << m_ChTrackMomVect.x()<<"   "<<m_ChTrackMomVect.y()
          << "    "<<m_ChTrackMomVect.z()
          << "      "<<  m_RichChTrackMass <<G4endl;
    G4cout<<"RichG4Hit: Charged track Pre Step XYZ For Ckv proc=   "
          << m_ChTrackCkvPreStepPos.x()<<"  "
          << m_ChTrackCkvPreStepPos.y()<<"   "
          << m_ChTrackCkvPreStepPos.z()<<G4endl;
    G4cout<<"RichG4Hit: Charged track Post Step XYZ For Ckv proc=  "
          << m_ChTrackCkvPostStepPos.x()<<"  "
          << m_ChTrackCkvPostStepPos.y()<<"   "
          << m_ChTrackCkvPostStepPos.z()<<G4endl;
    G4cout<<"RichG4Hit: Rayleigh Scat flag  Aerogel Exit XYZ    "
          << m_OptPhotRayleighFlag<<"    "
          << m_OptPhotAgelExitPos.x() <<"    "
          << m_OptPhotAgelExitPos.y()<<"     "
          <<  m_OptPhotAgelExitPos.z() <<G4endl;
    G4cout<<" RichG4Hit: Mirror1ReflXYZ Mirror2ReflXYZ "
          <<   m_Mirror1PhotonReflPosition.x()<<"   "
          <<   m_Mirror1PhotonReflPosition.y()<<"   "
          <<   m_Mirror1PhotonReflPosition.z()<<"   "
          <<   m_Mirror2PhotonReflPosition.x()<<"   "
          <<   m_Mirror2PhotonReflPosition.y()<<"   "
          <<   m_Mirror2PhotonReflPosition.z()<<G4endl;
    G4cout<<" RichG4Hit: Mirror1ReflEncodedCopyNum  Mirror2ReflEncodedCopyNum   "
          << m_Mirror1PhotonDetectorCopyNum<<"   "<< m_Mirror2PhotonDetectorCopyNum
          <<G4endl;
    G4cout<<" RichG4hit QWPhcathode reflection flag "<<m_RichHpdQW2PhCathReflFlag<<G4endl;
    G4cout<<" RichG4hit ElectronBackScatterFlag  "<< m_ElectronBackScatterFlag<<G4endl;
    G4cout<<" RichG4hit PhotoelectricProduction flag "<<m_PhotoElectricProductionFlag<<G4endl;
    G4cout<<" Photon reflection flag in Hpd "<<m_RichHpdPhotonReflectionFlag<<G4endl;
    G4cout<<" Photon Hpd reflection decoded:  "<<G4endl;
    
    std::vector<bool> aHfl = DecodeRichHpdReflectionFlag();
    RichG4HpdReflectionFlag* aRichG4HpdReflectionFlag= RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();
    G4int aMaxFlag= aRichG4HpdReflectionFlag->MaxNumHpdReflFlag();

    for(int j=0; j<aMaxFlag; ++j){
      G4cout<<aHfl[j]<<G4endl; 
    }
   
    G4cout<<"RichG4Hit  Hpd QW Incidence Point "<<m_HpdQuartzWindowExtSurfPhotIncidentPosition.x()
          <<"   "<<m_HpdQuartzWindowExtSurfPhotIncidentPosition.y()<<"    "
          << m_HpdQuartzWindowExtSurfPhotIncidentPosition.z()  <<G4endl;

    G4cout<<" RichG4Hit source process info "<<m_PhotonSourceProcessInfo<<G4endl;
    
  
  } 
}
std::vector<bool>  CkvG4Hit::DecodeRichHpdReflectionFlag() const
{


    RichG4HpdReflectionFlag* aRichG4HpdReflectionFlag= RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();
    G4int aMaxFlag= aRichG4HpdReflectionFlag->MaxNumHpdReflFlag();
    std::vector<bool> aBVect(aMaxFlag);
    std::bitset<16> aCurFl((unsigned long)m_RichHpdPhotonReflectionFlag);
    for(G4int i=0; i<aMaxFlag; ++i){
      aBVect[i]= aCurFl.test(i);
    }
  
  return aBVect;
  
}
Rich::RadiatorType CkvG4Hit::radiatorType() const
{
  RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  // default invalid radiator type
  Rich::RadiatorType rad = Rich::InvalidRadiator;
  // fill if known ID
  if      ( GetRadiatorNumber() == ( aRMIdValues-> Rich1GaseousCkvRadiatorNum () )){ rad = Rich::Rich1Gas; }
  else if ( GetRadiatorNumber() == ( aRMIdValues-> Rich2GaseousCkvRadiatorNum())) { rad = Rich::Rich2Gas; }
  else if ( aRMIdValues->  IsRich1AerogelAnyTileRad (  GetRadiatorNumber() ))    { rad = Rich::Aerogel; }
  else if (( aRMIdValues->  RichHpdQuartzWindowCkvRadiatorNum()) == GetRadiatorNumber() ) { rad = Rich::HPDQuartzWin; }
  else if ( ( aRMIdValues-> Rich1GasQWindowCkvRadiatorNum()) == GetRadiatorNumber() ||
            ( aRMIdValues-> Rich2GasQWindowCkvRadiatorNum() ) == GetRadiatorNumber() )     { rad = Rich::GasQuartzWin; }
  else if ( ( aRMIdValues-> RichFilterGenericCkvRadiatorNum()) == GetRadiatorNumber() ||
            ( aRMIdValues-> RichFilterD263CkvRadiatorNum())    == GetRadiatorNumber() )   { rad = Rich::AerogelFilter; }
  else if ( ( aRMIdValues-> Rich1NitrogenCkvRadiatorNum()) == GetRadiatorNumber() ||
            ( aRMIdValues-> Rich2NitrogenCkvRadiatorNum()) == GetRadiatorNumber() )       { rad = Rich::Nitrogen; }
  else if ( ( aRMIdValues-> RichCO2CkvRadiatorNum()) == GetRadiatorNumber() ) { rad = Rich::CO2;}
  else if  (aRMIdValues-> RichPmtQuartzWindowCkvRadiatorNum()== GetRadiatorNumber() ) { rad = Rich::PMTQuartzWin;} 
  
  

  // return final type
  return rad;
}

//=============================================================================
// Destructor
//=============================================================================
CkvG4Hit::~CkvG4Hit() {} 

//=============================================================================

#ifdef GNU_GCC
template class G4Allocator<CkvG4Hit>;
#endif

