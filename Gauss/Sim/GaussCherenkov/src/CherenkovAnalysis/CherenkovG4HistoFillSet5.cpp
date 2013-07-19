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
#include "GaussCherenkov/CherenkovG4Counters.h"



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

void CherenkovG4HistoFillSet5::FillRichG4HistoSet5Coord( CkvG4Hit* adHit, Gaudi::XYZPoint aHitCoordInPmtPlane,
         Gaudi::XYZPoint aHitCoordOnLensInPmtPlane    ){
        double xC=  aHitCoordInPmtPlane.x();
        double yC=  aHitCoordInPmtPlane.y();

        double xCH= xC;
        double yCH= yC;
        
        double xCRaw=  adHit-> GetGlobalPos().x();
        double yCRaw=  adHit-> GetGlobalPos().y();
    
        double xCLens= aHitCoordOnLensInPmtPlane.x();
        double yCLens= aHitCoordOnLensInPmtPlane.y();
 
        double xCLensH= xCLens;
        double yCLensH =yCLens;
        
       
        int aPmtNumA =  (int) ( adHit-> CurPmtNum()) ;
        int OptHorizR1 = (int) (adHit ->OptHorizontalRich1Hit());
        


        
  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
  SmartDataPtr<IHistogram2D> hXYRawHitLocationRich1Gas (CurrentHistoSvc, "RICHG4HISTOSET5/360");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Gas (CurrentHistoSvc, "RICHG4HISTOSET5/361");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich1GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/362");

  SmartDataPtr<IHistogram2D> hXYHitLocationRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/381");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich2GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/387");
  SmartDataPtr<IHistogram2D> hXYHitLocationNoScintRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/382");

  SmartDataPtr<IHistogram2D> hXYHitLocationLensRich1Gas(CurrentHistoSvc, "RICHG4HISTOSET5/391");

  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Mirror1 (CurrentHistoSvc, "RICHG4HISTOSET5/366");
  SmartDataPtr<IHistogram2D> hXYHitLocationRich1Mirror2 (CurrentHistoSvc, "RICHG4HISTOSET5/368");
 
  SmartDataPtr<IHistogram2D> hXYHitLocationPMTQWRich1Gas(CurrentHistoSvc, "RICHG4HISTOSET5/395");
  SmartDataPtr<IHistogram2D> hXYHitLocationPMTQWRich2Gas(CurrentHistoSvc, "RICHG4HISTOSET5/393");

  SmartDataPtr<IHistogram2D> hXYHitLocationCF4Rich2Gas(CurrentHistoSvc, "RICHG4HISTOSET5/394");


  
  int curRdet =   adHit-> GetCurRichDetNum();
  int curRichSect=  adHit-> GetCurSectorNum();
  int curHitProc= adHit-> PhotonSourceProcessInfo();
  const G4ThreeVector curHitMirror1= adHit->Mirror1PhotonReflPosition();
  const G4ThreeVector curHitMirror2= adHit->Mirror2PhotonReflPosition();
  double Rich1Mirror2XC=curHitMirror2.x();
  double Rich1Mirror2YC=curHitMirror2.y();
  int curRadiatorNum =  adHit-> GetRadiatorNumber();
  const bool curHitDuplicateFlag=  adHit->FlagHitAsDuplicate();
  
  
  if( curRdet == 0 ) {
    SmartDataPtr<IHistogram1D> hHitOccpR1(CurrentHistoSvc, "RICHG4HISTOSET5/365");
    SmartDataPtr<IHistogram1D> hHitOccpR1Binary(CurrentHistoSvc, "RICHG4HISTOSET5/566");

  SmartDataPtr<IHistogram1D> hPmtOccpGasQWRich1(CurrentHistoSvc, "RICHG4HISTOSET5/402");
  SmartDataPtr<IHistogram1D> hPmtOccpPmtQWRich1  (CurrentHistoSvc, "RICHG4HISTOSET5/404");


    hHitOccpR1->fill(aPmtNumA*1.0);
    if((!curHitDuplicateFlag)  && hHitOccpR1Binary ) hHitOccpR1Binary ->fill(aPmtNumA*1.0);

    if(hPmtOccpGasQWRich1 &&  (curRadiatorNum==6) ) hPmtOccpGasQWRich1->fill (aPmtNumA*1.0);
    if(hPmtOccpPmtQWRich1 && (curRadiatorNum==32) ) hPmtOccpPmtQWRich1->fill (aPmtNumA*1.0);
    
    m_TotNumHitR1InCurEv++;

      if( curRichSect ==0 ) {
          
          yC += 400.0;
          Rich1Mirror2YC -= 345.0;
          yCLens += 400.0;

          if(   OptHorizR1 == 1 ) {
            
            //       yCH += 450.0;
            // yCLensH +=450.0;

            xCH += 450.0;
            xCLensH +=450.0;
            
          }
          
         
      }else if (curRichSect ==1 ) {

        yC -= 400.0;
        yCLens -= 400.0;

        Rich1Mirror2YC += 345.0;

         if(   OptHorizR1 == 1 ) {
            
           //  yCH -= 450.0;
           // yCLensH -=450.0;


            xCH -= 450.0;
            xCLensH -=450.0;
            
          }




      }      





      if( hXYRawHitLocationRich1Gas)  hXYRawHitLocationRich1Gas->fill(xCRaw,yCRaw);



      if(hXYHitLocationRich1Gas) {

        if(OptHorizR1 == 0 ) {
          hXYHitLocationRich1Gas->fill (xC,yC);
          if((!curHitDuplicateFlag )  &&  hXYHitLocationRich1GasBinary)hXYHitLocationRich1GasBinary->fill (xC,yC);
          if(hXYHitLocationPMTQWRich1Gas && (curRadiatorNum==32)  ) {
           hXYHitLocationPMTQWRich1Gas ->fill (xC,yC);
           
          }
          
        }else if (OptHorizR1 == 1 ) {
         
          



          // hXYHitLocationRich1Gas->fill (yCH,xCH);
          hXYHitLocationRich1Gas->fill (xCH,yCH);
          if((!curHitDuplicateFlag ) && hXYHitLocationRich1GasBinary )hXYHitLocationRich1GasBinary->fill (xCH,yCH);

          if(hXYHitLocationPMTQWRich1Gas && (curRadiatorNum==32)  ) {
            // hXYHitLocationPMTQWRich1Gas ->fill (yCH,xCH);           
            //hXYHitLocationPMTQWRich1Gas ->fill (yCH,-1.0*xCH);           
           hXYHitLocationPMTQWRich1Gas ->fill (xCH,yCH);           
          }
            
        }
        
      }
      if(hXYHitLocationLensRich1Gas) {
        if(OptHorizR1 == 0 ) {
          hXYHitLocationLensRich1Gas->fill (xCLens,yCLens);
        }else {
          
          // hXYHitLocationLensRich1Gas->fill (yCLensH,xCLensH);
          // hXYHitLocationLensRich1Gas->fill (yCLensH,-1.0*xCLensH);
          hXYHitLocationLensRich1Gas->fill (xCLensH,yCLensH);
          
        }
        
        
      }


      if(hXYHitLocationRich1Mirror1) hXYHitLocationRich1Mirror1->fill(curHitMirror1.x(),  curHitMirror1.y());
      if(hXYHitLocationRich1Mirror2) hXYHitLocationRich1Mirror2->fill(Rich1Mirror2XC, Rich1Mirror2YC);
      
    
    
  }else if (curRdet == 1 ) {

  CherenkovG4Counters* aCherenkovG4Counters = CherenkovG4Counters::getInstance();
  int aSizeR1= aCherenkovG4Counters->NumPmtsRich1();
  int aPmtNumR2= aPmtNumA-aSizeR1;
  SmartDataPtr<IHistogram1D> hHitOccpR2(CurrentHistoSvc, "RICHG4HISTOSET5/385");
  SmartDataPtr<IHistogram1D> hHitOccpR2Binary(CurrentHistoSvc, "RICHG4HISTOSET5/567");

  SmartDataPtr<IHistogram1D> hHitOccpNoScintR2(CurrentHistoSvc, "RICHG4HISTOSET5/386");
  SmartDataPtr<IHistogram1D> hPmtOccpGasQWRich2(CurrentHistoSvc, "RICHG4HISTOSET5/407");
  SmartDataPtr<IHistogram1D> hPmtOccpPmtQWRich2  (CurrentHistoSvc, "RICHG4HISTOSET5/408");


  if(hHitOccpR2) hHitOccpR2->fill( aPmtNumR2*1.0);
  if((!curHitDuplicateFlag)  && hHitOccpR2Binary )  hHitOccpR2Binary->fill( aPmtNumR2*1.0);

  if(curHitProc != 2 ) {
    if(hHitOccpNoScintR2)hHitOccpNoScintR2->fill(aPmtNumR2*1.0);
    m_TotNumHitR2NoScintInCurEv++;
    
  }

  if( hPmtOccpGasQWRich2 &&   (curRadiatorNum==6) ) hPmtOccpGasQWRich2->fill( aPmtNumR2*1.0);
  if( hPmtOccpPmtQWRich2 &&   (curRadiatorNum==32) ) hPmtOccpPmtQWRich2 ->fill( aPmtNumR2*1.0);

  

    m_TotNumHitR2InCurEv++;
      
      if( curRichSect ==0 ) {
        xC += 450.0;
      }else if (curRichSect ==1 ) {
        xC -= 450.0;  
      }      
      
        if(hXYHitLocationRich2Gas)hXYHitLocationRich2Gas->fill( xC, yC);
      
      if(curHitProc != 2 ) {
        if(hXYHitLocationNoScintRich2Gas )
          hXYHitLocationNoScintRich2Gas->fill(xC, yC); 
      }
      
      if( (!curHitDuplicateFlag) && hXYHitLocationRich2GasBinary ){ 
        hXYHitLocationRich2GasBinary->fill( xC, yC);
      }
      
       if(hXYHitLocationPMTQWRich2Gas && (curRadiatorNum==32)  ) {
           hXYHitLocationPMTQWRich2Gas ->fill (xC,yC);
       }
       if(hXYHitLocationCF4Rich2Gas && (curRadiatorNum==2)  ) {
           hXYHitLocationCF4Rich2Gas->fill (xC,yC);
           
       }
       
       
       
      
      
      
       
  }
  
  
  
  
  
}

