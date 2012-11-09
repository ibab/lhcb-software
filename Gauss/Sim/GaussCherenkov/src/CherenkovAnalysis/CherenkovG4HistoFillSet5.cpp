// $Id: $
// Include files 

#include "GaussRICH/RichG4Counters.h"
// local

//GEANT4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"

/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
//#include "AIDA/IProfile.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "CherenkovG4Counters.h"


// local
#include "CherenkovG4HistoFillSet5.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoFillSet5
//
// 2011-04-15 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoFillSet5::CherenkovG4HistoFillSet5(  ):
  m_TotNumHitR1InCurEv(0),
  m_TotNumHitR2InCurEv(0),
  m_TotNumHitR2NoScintInCurEv(0) {

}
//=============================================================================
// Destructor
//=============================================================================
CherenkovG4HistoFillSet5::~CherenkovG4HistoFillSet5() {} 

//=============================================================================
void CherenkovG4HistoFillSet5::InitCherenkovG4HistoFillSet5() 
{
  m_TotNumHitR1InCurEv=0;
  m_TotNumHitR2InCurEv=0;
  m_TotNumHitR2NoScintInCurEv=0;
  
}
void CherenkovG4HistoFillSet5::FillRichG4HistoSet5NumH() 
{
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  SmartDataPtr<IHistogram1D> hNumTotHitRich1All (CurrentHistoSvc, "RICHG4HISTOSET5/363");
  SmartDataPtr<IHistogram1D> hNumTotHitRich2All (CurrentHistoSvc, "RICHG4HISTOSET5/383");
  SmartDataPtr<IHistogram1D> hNumTotHitNoScintRich2All (CurrentHistoSvc, "RICHG4HISTOSET5/384");
  if( hNumTotHitRich1All &&  (m_TotNumHitR1InCurEv > 0) ) hNumTotHitRich1All->fill(m_TotNumHitR1InCurEv);
  if( hNumTotHitRich2All &&  (m_TotNumHitR2InCurEv > 0) ) hNumTotHitRich2All->fill(m_TotNumHitR2InCurEv);
  if( hNumTotHitNoScintRich2All &&  (m_TotNumHitR2NoScintInCurEv > 0) ) 
            hNumTotHitNoScintRich2All->fill(m_TotNumHitR2NoScintInCurEv);
  
}

void CherenkovG4HistoFillSet5::FillRichG4HistoSet5Coord( CkvG4Hit* adHit, Gaudi::XYZPoint aHitCoordInPmtPlane){
        double xC=  aHitCoordInPmtPlane.x();
        double yC=  aHitCoordInPmtPlane.y();
        int aPmtNumA =  (int) ( adHit-> CurPmtNum()) ;
        
  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Gas (CurrentHistoSvc, "RICHG4HISTOSET5/361");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/381");
  SmartDataPtr<IHistogram2D> hXYHitLocationNoScintRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/382");

  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Mirror1 (CurrentHistoSvc, "RICHG4HISTOSET5/366");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Mirror2 (CurrentHistoSvc, "RICHG4HISTOSET5/368");
   
  int curRdet =   adHit-> GetCurRichDetNum();
  int curRichSect=  adHit-> GetCurSectorNum();
  int curHitProc= adHit-> PhotonSourceProcessInfo();
  const G4ThreeVector curHitMirror1= adHit->Mirror1PhotonReflPosition();
  const G4ThreeVector curHitMirror2= adHit->Mirror2PhotonReflPosition();
  double Rich1Mirror2XC=curHitMirror2.x();
  double Rich1Mirror2YC=curHitMirror2.y();
  
  
  if( curRdet == 0 ) {
    SmartDataPtr<IHistogram1D> hHitOccpR1(CurrentHistoSvc, "RICHG4HISTOSET5/365");
    hHitOccpR1->fill(aPmtNumA*1.0);
    
    m_TotNumHitR1InCurEv++;

      if( curRichSect ==0 ) {
        //yC += 650.0;
        //yC += 450.0;
        //yC += 400.0;
        //yC += 200.0;
        yC += 400.0;
        Rich1Mirror2YC -= 345.0;
        
      }else if (curRichSect ==1 ) {

        yC -= 400.0;


            //    yC -= 200.0;  
            //    yC -= 400.0;  
        //    yC -= 450.0;  
        //yC -= 650.0;  
        Rich1Mirror2YC += 345.0;
      }      
      if(hXYHitLocationRich1Gas) {
        hXYHitLocationRich1Gas->fill (xC,yC);
      }

      if(hXYHitLocationRich1Mirror1) hXYHitLocationRich1Mirror1->fill(curHitMirror1.x(),  curHitMirror1.y());
      if(hXYHitLocationRich1Mirror2) hXYHitLocationRich1Mirror2->fill(Rich1Mirror2XC, Rich1Mirror2YC);
      
    
    
  }else if (curRdet == 1 ) {

  CherenkovG4Counters* aCherenkovG4Counters = CherenkovG4Counters::getInstance();
  int aSizeR1= aCherenkovG4Counters->NumPmtsRich1();
  int aPmtNumR2= aPmtNumA-aSizeR1;
  SmartDataPtr<IHistogram1D> hHitOccpR2(CurrentHistoSvc, "RICHG4HISTOSET5/385");
  SmartDataPtr<IHistogram1D> hHitOccpNoScintR2(CurrentHistoSvc, "RICHG4HISTOSET5/386");
  if(hHitOccpR2) hHitOccpR2->fill( aPmtNumR2*1.0);
  if(curHitProc != 2 ) {
    if(hHitOccpNoScintR2)hHitOccpNoScintR2->fill(aPmtNumR2*1.0);
    m_TotNumHitR2NoScintInCurEv++;
    
  }
  

    m_TotNumHitR2InCurEv++;
      
      if( curRichSect ==0 ) {
        xC += 450.0;
      }else if (curRichSect ==1 ) {
        xC -= 450.0;  
      }      
      if(hXYHitLocationRich2Gas) {
        if(hXYHitLocationRich2Gas)hXYHitLocationRich2Gas->fill( xC, yC);
      if(curHitProc != 2 ) {
        if(hXYHitLocationNoScintRich2Gas )
                            hXYHitLocationNoScintRich2Gas->fill(xC, yC); 
      }
      
      }
    
    
  }
  
  

}
