#include "RichG4Hit.h"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4Transform3D.hh"
#include "G4LogicalVolume.hh"

G4Allocator<RichG4Hit> RichG4HitAllocator;

RichG4Hit::RichG4Hit() {;}

RichG4Hit::~RichG4Hit(){;}

RichG4Hit::RichG4Hit(const RichG4Hit &right):GaussHitBase(right)
{
  m_edep = right.m_edep;
  m_GlobalPos = right.m_GlobalPos;
  m_LocalPos = right.m_LocalPos;
  m_GlobalPEOriginPos=right.m_GlobalPEOriginPos;
  m_LocalPEOriginPos=right.m_LocalPEOriginPos;
  m_CurRichNum = right.m_CurRichNum;
  m_CurSectorNum = right.m_CurSectorNum;
  m_CurHpdNum = right.m_CurHpdNum;
  m_CurPixelXNum = right.m_CurPixelXNum;
  m_CurPixelYNum = right.m_CurPixelYNum;
  m_OptPhotID = right.m_OptPhotID;
  m_ChTrackID = right.m_ChTrackID;
  m_ChTrackPDG= right.m_ChTrackPDG;
  m_PETrackID = right.m_PETrackID;
  m_PETrackPDG= right.m_PETrackPDG;
  m_RadiatorNumber=right.m_RadiatorNumber;
  m_PhotEmisPt=right.m_PhotEmisPt;
  m_PhotEnergyAtProd=right.m_PhotEnergyAtProd;
  m_ThetaCkvAtProd=right.m_ThetaCkvAtProd;
  m_PhiCkvAtProd=right.m_PhiCkvAtProd;
  m_ChTrackTotMom=right.m_ChTrackTotMom;
  m_ChTrackMomVect=right.m_ChTrackMomVect;
  m_RichHitGlobalTime = right.m_RichHitGlobalTime;
  
}

const RichG4Hit& RichG4Hit::operator=(const RichG4Hit &right)
{
  m_edep = right.m_edep;
  m_GlobalPos = right.m_GlobalPos;
  m_LocalPos = right.m_LocalPos;
  m_GlobalPEOriginPos=right.m_GlobalPEOriginPos;
  m_LocalPEOriginPos=right.m_LocalPEOriginPos;
  m_CurRichNum = right.m_CurRichNum;
  m_CurSectorNum = right.m_CurSectorNum;
  m_CurHpdNum = right.m_CurHpdNum;
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

  return *this;
}

int RichG4Hit::operator==(const RichG4Hit &right) const
{

  //  return 0;
  return((m_CurRichNum==right.m_CurRichNum)&&(m_CurHpdNum==right.m_CurHpdNum)
         &&(m_CurPixelXNum==right.m_CurPixelXNum)&&
         (m_CurPixelYNum==right.m_CurPixelYNum ) );
}

void RichG4Hit::Draw()
{

 
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

void RichG4Hit::DrawPEOrigin()
{
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


void RichG4Hit::Print()
{
  G4cout<<"RichG4Hit: "<<" Rich Sect Hpd Pix: "<<m_CurRichNum<<"   "
        <<m_CurSectorNum<<"   "<<m_CurHpdNum<<"    "<<m_CurPixelXNum
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
  G4cout<<"RichG4Hit: "<<"MotherChTrack MomVect XYZ  PDGCode:  "
        << m_ChTrackMomVect.x()<<"   "<<m_ChTrackMomVect.y()
        << "    "<<m_ChTrackMomVect.z()
        <<"    "<<  m_ChTrackPDG <<G4endl;
  G4cout<<"RichG4Hit: "<<"TrackID: MotherCh OptPhot PE: "
        << m_ChTrackID<<"     "<< m_OptPhotID<<"   "
        << m_PETrackID<<G4endl;
  
  G4cout<<"RichHit Global Time = " 
        <<m_RichHitGlobalTime<<G4endl;

  }
// This is a forward declaration of an instantiated G4Allocator<Type> object.
// It has been added in order to make code portable for the GNU g++ 
// (release 2.7.2) compiler. 
// Whenever a new Type is instantiated via G4Allocator, it has to be forward
// declared to make object code (compiled with GNU g++) link successfully. 
// 
#ifdef GNU_GCC
  template class G4Allocator<RichG4Hit>;
#endif


