// $Id: $
// Include files 

/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
//#include "AIDA/IProfile.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "GaussRICH/RichG4Counters.h"


// local
#include "RichG4HistoFillSet5.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoFillSet5
//
// 2012-03-18 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoFillSet5::RichG4HistoFillSet5(  ):
  m_NumHitInRich1InCurEv(0),
  m_NumHitInRich2InCurEv(0),
  m_NumHitInRich2NoScintInCurEv(0),
  m_NumHitInRich1InCurEvBinary(0),
  m_NumHitInRich2InCurEvBinary(0),
  m_NumHitInRich2NoScintInCurEvBinary(0)
   { }
//=============================================================================
// Destructor
//=============================================================================
RichG4HistoFillSet5::~RichG4HistoFillSet5() {} 

//=============================================================================
void RichG4HistoFillSet5::InitRichG4HistoFillSet5(){
  m_NumHitInRich1InCurEv = 0;
  m_NumHitInRich2InCurEv = 0;
  m_NumHitInRich2NoScintInCurEv = 0; 
  m_NumHitInRich1InCurEvBinary = 0;
  m_NumHitInRich2InCurEvBinary = 0;
  m_NumHitInRich2NoScintInCurEvBinary = 0; 
}
void RichG4HistoFillSet5::FillRichG4HistoSet5NumHits(){

  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
    SmartDataPtr<IHistogram1D> hNumTotHitHRich1Gas (CurrentHistoSvc, "RICHG4HISTOSET5/463");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/483");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2GasNoScint (CurrentHistoSvc, "RICHG4HISTOSET5/484");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich1GasA (CurrentHistoSvc, "RICHG4HISTOSET5/453");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2GasA (CurrentHistoSvc, "RICHG4HISTOSET5/473");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2GasNoScintA (CurrentHistoSvc, "RICHG4HISTOSET5/474");

    SmartDataPtr<IHistogram1D> hNumTotHitHRich1GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/454");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/475");
    SmartDataPtr<IHistogram1D> hNumTotHitHRich2GasNoScintBinary (CurrentHistoSvc, "RICHG4HISTOSET5/476");

    if(hNumTotHitHRich1Gas && (m_NumHitInRich1InCurEv>0 ))hNumTotHitHRich1Gas->fill(m_NumHitInRich1InCurEv);
    if(hNumTotHitHRich2Gas && (m_NumHitInRich2InCurEv>0 ))hNumTotHitHRich2Gas->fill(m_NumHitInRich2InCurEv);
    if(hNumTotHitHRich2GasNoScint && (m_NumHitInRich2NoScintInCurEv>0 ))
       hNumTotHitHRich2GasNoScint->fill(m_NumHitInRich2NoScintInCurEv);
    
    if(hNumTotHitHRich1GasA && (m_NumHitInRich1InCurEv>0 ))hNumTotHitHRich1GasA->fill(m_NumHitInRich1InCurEv);
    if(hNumTotHitHRich2GasA && (m_NumHitInRich2InCurEv>0 ))hNumTotHitHRich2GasA->fill(m_NumHitInRich2InCurEv);
    if(hNumTotHitHRich2GasNoScintA && (m_NumHitInRich2NoScintInCurEv>0 ))
       hNumTotHitHRich2GasNoScintA->fill(m_NumHitInRich2NoScintInCurEv);


    if(hNumTotHitHRich1GasBinary && (m_NumHitInRich1InCurEvBinary>0 ))
                                     hNumTotHitHRich1GasBinary->fill(m_NumHitInRich1InCurEvBinary);
    if(hNumTotHitHRich2GasBinary && (m_NumHitInRich2InCurEvBinary>0 ))
                                     hNumTotHitHRich2GasBinary->fill(m_NumHitInRich2InCurEvBinary);
    if(hNumTotHitHRich2GasNoScintBinary && (m_NumHitInRich2NoScintInCurEvBinary>0 ))
       hNumTotHitHRich2GasNoScintBinary->fill(m_NumHitInRich2NoScintInCurEvBinary);
    
}



void RichG4HistoFillSet5::FillRichG4HistoSet5Coordinate(RichG4Hit * aHit,
                                                        Gaudi::XYZPoint aHitCoordInPhDetPlane,
                                                        int aTotNumHpdR1){


   double xC=  aHitCoordInPhDetPlane.x();
   double yC=  aHitCoordInPhDetPlane.y();

        
  //  IHistogramSvc* CurrentHistoSvc;
  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
   
  int curRdet =   aHit-> GetCurRichDetNum();
  int curRichSect=  aHit-> GetCurSectorNum();
  //int curHitProc= aHit-> PhotonSourceProcessInfo();
  int curHpdNum = aHit-> GetCurHpdNum();
  bool aDupHitFlag = aHit -> FlagHitAsDuplicate() ;
  // std::cout<<"test Rdet hpdnum aDupHitFlag "<<  curRdet <<"  "<<curHpdNum<<"  "<<aDupHitFlag<<std::endl;
  
  if( curRdet == 0 ) {
    SmartDataPtr<IHistogram2D> hXYHitLocationHRich1Gas (CurrentHistoSvc, "RICHG4HISTOSET5/461");
   SmartDataPtr<IHistogram2D> hXYHitLocationHRich1GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/451");

    SmartDataPtr<IHistogram1D> hHitHpdOccpHR1(CurrentHistoSvc, "RICHG4HISTOSET5/465");
    SmartDataPtr<IHistogram1D> hHitHpdOccpHR1Binary(CurrentHistoSvc, "RICHG4HISTOSET5/455");
    if( hHitHpdOccpHR1) hHitHpdOccpHR1->fill(curHpdNum*1.0);
    if((! aDupHitFlag)&&  (hHitHpdOccpHR1Binary) ) hHitHpdOccpHR1Binary->fill(curHpdNum*1.0);
    m_NumHitInRich1InCurEv++;
    if(! aDupHitFlag)    m_NumHitInRich1InCurEvBinary++;
    

      if( curRichSect ==0 ) {
        yC += 450.0;
      }else if (curRichSect ==1 ) {
        yC -= 450.0;  
      }      

      if(hXYHitLocationHRich1Gas) {
        hXYHitLocationHRich1Gas->fill (xC,yC);
      }

       if ((! aDupHitFlag) && ( hXYHitLocationHRich1GasBinary)) {
         hXYHitLocationHRich1GasBinary->fill (xC,yC); 
       }
       
       
        
    
  }else if (curRdet == 1 ) {

    SmartDataPtr<IHistogram2D> hXYHitLocationHRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/481");
    SmartDataPtr<IHistogram2D> hXYHitLocationHNoScintRich2Gas (CurrentHistoSvc, "RICHG4HISTOSET5/482");

    int aHpdNumR2= curHpdNum-aTotNumHpdR1;

    SmartDataPtr<IHistogram1D> hHitHpdOccpHR2(CurrentHistoSvc, "RICHG4HISTOSET5/485");
    SmartDataPtr<IHistogram1D> hHitHpdOccpHNoScintR2(CurrentHistoSvc, "RICHG4HISTOSET5/486");

    SmartDataPtr<IHistogram2D> hXYHitLocationHRich2GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/471");
    SmartDataPtr<IHistogram2D> hXYHitLocationHNoScintRich2GasBinary (CurrentHistoSvc, "RICHG4HISTOSET5/472");

    SmartDataPtr<IHistogram1D> hHitHpdOccpHR2Binary(CurrentHistoSvc, "RICHG4HISTOSET5/477");
    SmartDataPtr<IHistogram1D> hHitHpdOccpHNoScintR2Binary(CurrentHistoSvc, "RICHG4HISTOSET5/478");


    if(hHitHpdOccpHR2) hHitHpdOccpHR2->fill( aHpdNumR2*1.0);
    if((! aDupHitFlag) && (hHitHpdOccpHR2Binary) ) hHitHpdOccpHR2Binary->fill( aHpdNumR2*1.0);
    //    if(curHitProc != 2 ) {
    if( !(aHit ->isScintillation() )) {
        
         if(hHitHpdOccpHNoScintR2)hHitHpdOccpHNoScintR2->fill(aHpdNumR2*1.0);
         if((! aDupHitFlag) && (hHitHpdOccpHNoScintR2Binary))hHitHpdOccpHNoScintR2Binary->fill(aHpdNumR2*1.0);
       
         m_NumHitInRich2NoScintInCurEv++;
         if(! aDupHitFlag) m_NumHitInRich2NoScintInCurEvBinary++;
    }
    
        
        
        
    m_NumHitInRich2InCurEv++;
    if(! aDupHitFlag)m_NumHitInRich2InCurEvBinary++;
    if( curRichSect ==0 ) {
        xC += 450.0;
    }else if (curRichSect ==1 ) {
      xC -= 450.0;
    }
    

    if(hXYHitLocationHRich2Gas) {
      if(hXYHitLocationHRich2Gas)hXYHitLocationHRich2Gas->fill( xC, yC);
    }

    if((! aDupHitFlag) && (hXYHitLocationHRich2GasBinary) ) {
          if(hXYHitLocationHRich2GasBinary)hXYHitLocationHRich2GasBinary->fill( xC, yC);
     }
        
//        if(curHitProc != 2 ) {

     if( !(aHit ->isScintillation() )) {

            if(hXYHitLocationHNoScintRich2Gas )
              hXYHitLocationHNoScintRich2Gas->fill(xC, yC); 
            if((! aDupHitFlag) && (hXYHitLocationHNoScintRich2GasBinary ))
              hXYHitLocationHNoScintRich2GasBinary->fill(xC, yC); 
     }
     
        
     
  }
  
        
  
}


    
  
  
  


