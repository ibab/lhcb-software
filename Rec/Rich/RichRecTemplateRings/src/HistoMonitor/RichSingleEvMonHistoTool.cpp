// $Id: RichSingleEvMonHistoTool.cpp,v 1.3 2009-07-30 11:22:55 jonrob Exp $
// Include files 

#include "TH1D.h"
#include "TH2D.h"

// from Gaudi
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/HistogramBase.h"

// local
#include "RichSingleEvMonHistoTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichSingleEvMonHistoTool
//
// 2007-06-18 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichSingleEvMonHistoTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichSingleEvMonHistoTool::RichSingleEvMonHistoTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent )
{
  declareInterface<IRichSingleEvMonHistoTool>(this);
  //  m_RichOutHistogramList=new  TObjArray(0);

}

void RichSingleEvMonHistoTool::PlotHitsAndTracksInEv(int irad ){
  int iRich = Tfm()->RichDetNumFromRadiator(irad);
  int aNumHit = tgD()->NumHitsTarget(iRich);
  int aNumTk = tgD()->NumChRdTrackValue(irad);

  debug()<<" Now in single ev histo Rich numhit numtk"<<  iRich<<"  "<< aNumHit<<"  "<< aNumTk <<endmsg;
  

  for (int ih=0; ih< aNumHit; ++ih){
    plot2D(tgD()->XHitInpValue(ih,iRich),tgD()->YHitInpValue(ih,iRich),"XYRichHit" ,"Y vs X Hits",
       -800.0, 800.0,-800.0,800.0,200,200);
  }
  
  for(int it=0; it< aNumTk; ++it){
    // debug print
    // info()<<" rad TrackNum  X Y coord RecMomentum "<<irad<<"  "<<it<<"  "
    //      << tgD()->TrackXcInpValue(it,irad)<<"   "<<tgD()-> TrackYcInpValue(it,irad)
    //       <<"   "<<tgD()->TrackRecMomValue(it,irad)<<  endmsg;
    //
    // end debug print

    plot2D(tgD()->TrackXcInpValue(it,irad),tgD()-> TrackYcInpValue(it,irad),
           "XYTrackHit","Y vs X Track Projection",
             -800.0, 800.0,-800.0,800.0,200,200);
  }
  
}
void RichSingleEvMonHistoTool::PlotTargetAndTemplate(){
  int aNumW= RConst()->NumW();
  int aNumR = RConst()->NumR();


  // first for the target  
  VD aRpTar = RLocTgC()->RpnTarget();
  VD aPpTar = RLocTgC()->PpnTarget();
  int aSize = (int) aRpTar.size();
  //info()<<" Now in PlotTargettemplate with target size  "<<  aSize<<endmsg;
  
  for(int ih=0; ih<aSize; ++ih) {
    plot2D(aPpTar[ih],aRpTar[ih],"RPLPolTarget","LogR vs Phi Target ",0.0,aNumW*1.0, 0.0,aNumR*1.0,aNumW,aNumR);
    
  }

  // now for the template

  VD aRpTem = RLocTDC()-> RpnTemplate();
  VD aPpTem = RLocTDC()-> PpnTemplate(); 
  int bSize = (int) aRpTem.size();

  //  info()<<" Now in PlotTargettemplate with template size  "<<  bSize<<endmsg;
  for(int ik=0; ik<  bSize; ++ik) {
    plot2D(aPpTem[ik], aRpTem[ik],"RPLPolTemplate","LogR vs Phi Template",0.0,aNumW*1.0, 0.0,aNumR*1.0,aNumW,aNumR);  
  }
  
  
}

void RichSingleEvMonHistoTool::PlotInvCorr( VVD aRPInv){
  int aX = (int) aRPInv.size();
  int aY =  (int) aRPInv [0].size();
    
  if( !m_HistoInvCorrSingleTrack ) {
    
   m_HistoInvCorrSingleTrack = 
    histoSvc()->book( RParam()->RingRecHistoPath()+"10","Polar Phi vs Log R Correlation ",
                      aX,0.0, aX*1.0,aY, 0.0, aY*1.0);
  }else {
    m_HistoInvCorrSingleTrack->scale(0.0);    
  }
  
  
  // the following does not compile.
  // static int histNum = 105;
  //    histoSvc()->book( histNum,"Polar Phi vs Log R Correlation ",
  //                    aX,0.0, aX*1.0,aY, 0.0, aY*1.0);
  //++histNum;

  for(int ix=0; ix<aX; ++ix) {
    for(int iy=0; iy<aY;++iy){

      if( aRPInv [ix] [iy] != 0.0 ) {  
        m_HistoInvCorrSingleTrack->fill(ix,iy,fabs(aRPInv [ix] [iy]));
      }
      
    }
    
  }
  


  
  // test histo for testing the bins

  if(! m_TestHistoMonitor ) {
    
    m_TestHistoMonitor = histoSvc()->book(RParam()->RingRecHistoPath()+"_25", "test histo", 50 , 0.0, 50.0);
  }else {
    m_TestHistoMonitor ->scale(0.0);
    
  }
  
  
     for(int i=0; i<11; i++){

       m_TestHistoMonitor ->fill(i, 25+i*1.0);
    }

     
     
  // end test histo  
     //    info ()<<" end of plotting inv corr "<<endmsg;
     

}


//void RichSingleEvMonHistoTool::InitDebugHistoSetup()
//{
//  
//  m_WriteOutDebugHistoFlag = RParam()->WriteOutAuxiliaryDebugHisto();
//  m_DebugHistoOutputFileName= RParam()->RichDebugAuxHistoFileName();  
//  info()<<"  initialised Rich RingRec debug histo setup "<<m_WriteOutDebugHistoFlag<<"  "
//        <<m_DebugHistoOutputFileName<<  endmsg;
//  
//}

//void RichSingleEvMonHistoTool::WriteOutDebugHistogramsToAuxiliaryFile() 
//{
  // if( m_WriteOutDebugHistoFlag ) {
  //  info()<<" Now write out the debug histo "<<endmsg;
  //  
  //  TFile afile( m_DebugHistoOutputFileName.data(),"recreate");
  //  m_RichOutHistogramList->Write();
  //  afile.Close();
  //  
  // }
//  
//
//}

//=============================================================================
// Destructor
//=============================================================================
RichSingleEvMonHistoTool::~RichSingleEvMonHistoTool() {} 

//=============================================================================
