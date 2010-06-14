// $Id: B2DHMvaFisherDMonitorHistoTool.cpp,v 1.1 2010/06/10 12:42:21 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 


// local
#include "B2DHMvaFisherDMonitorHistoTool.h"
#include <Event/Particle.h>
#include "Event/RecVertex.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/HistogramBase.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

using namespace Gaudi;

using namespace LHCb ;
using namespace Gaudi::Units ;


//-----------------------------------------------------------------------------
// Implementation file for class : B2DHMvaMonitorHistoTool
//
// 2009-07-17 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( B2DHMvaMonitorHistoTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2DHMvaMonitorHistoTool::B2DHMvaMonitorHistoTool( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IB2DHMvaMonitorHistoTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
B2DHMvaMonitorHistoTool::~B2DHMvaMonitorHistoTool() {} 

//=============================================================================
StatusCode B2DHMvaMonitorHistoTool::initialize() {
   StatusCode sc = StatusCode::SUCCESS;
   m_Bs2DsHMvaUtilityTool = tool<IBs2DsHMvaUtilityTool> ("Bs2DsHMvaUtilityTool");
   m_B2DHSelectionParamTool = tool<IB2DHSelectionParamTool>("B2DHSelectionParamTool");
   

   return sc;
}


void B2DHMvaMonitorHistoTool::fillHistoForEventSelectionSteps( const LHCb::Particles * partsBs, int anumCand, int indexOfbestCand,
                                                                     std::vector<double> aProb,std::vector<double>aIps,
                                                                     std::vector<int> aCndNoSel, 
                                                                     std::vector<int> aCandPrelimSel,
                                                                     std::vector<int> aCandFisherSel,
                                                                     std::vector<int> aCandAuxSel,
                                                                     std::vector<int> aCandMassSel,
                                                                     std::vector<int> aCandIndex ){
  


  // Note on candidates used for plots from each event:
  // For the events which passed the selection , the best selected candidate is used for the plots.

  // For the events which are not selected, for now the smallest BsIps candidate is used for the plots.
  // This may be changed to the highest fisherD candidate if needed in the future.
  // end of note
 
  debug()<<"Current   numcand candIndex size"<<   anumCand << "  "<<(int) aCandIndex.size() <<endmsg;
  

  // info()<<" B2DHMvaMonitorHistoTool Monitor histo candidates "<<anumCand<<"  "
  //                    << indexOfbestCand <<"  "<<(int) aCandIndex.size() <<endmsg;
  // info()<<" B2DHMvaMonitorHistoTool Monitor size of candauxSel CandFisherSel CandPrelimSel CandPVSel "
  //      <<(int) aCandAuxSel.size() <<"   "<<(int)  aCandFisherSel.size() <<"   "
  //      << (int) aCandPrelimSel.size() <<"  "<<(int) aCndNoSel.size()<<endmsg;
  
  //for (int i=0; i<(int) aCndNoSel.size(); ++i ) {
  //  info()<<" aIps aProb at histo "<< i<<"  "<<  aIps[i]<<"   "<<aProb[i]<<endmsg;
  // }
  
  

    if( indexOfbestCand >=0 ) {
      fillHistoAfterSelWithL0( partsBs,indexOfbestCand );

    }else if( (int) aCandMassSel.size()  > 0   ) {
      
      int aIndexAllSelBeforeL0 =  
        m_Bs2DsHMvaUtilityTool-> FindBestCandidateInEv(aCandMassSel, aIps, aProb,0);
      
      fillHistoAfterAllSelBeforeL0(partsBs,aIndexAllSelBeforeL0);

    }else if( (int) aCandAuxSel.size() > 0  ){
        int aIndexSelAux = m_Bs2DsHMvaUtilityTool-> FindBestCandidateInEv(aCandAuxSel, aIps, aProb,0);
        fillHistoAfterAuxSel(partsBs,aIndexSelAux  );
      
    }else if ( (int)  aCandFisherSel.size() > 0   ) {
       int aIndexFisher=  m_Bs2DsHMvaUtilityTool-> FindBestCandidateInEv(aCandFisherSel, aIps, aProb,0);
      fillHistoAfterFisherSel(partsBs,aIndexFisher );     
      
    }else if ( (int) aCandPrelimSel.size() > 0  ) {
      int aIndexPrelim = m_Bs2DsHMvaUtilityTool-> FindBestCandidateInEv(aCandPrelimSel, aIps, aProb,1);
      fillHistoAfterPrelimSel(partsBs,aIndexPrelim);
      
    }else if ( (int) aCndNoSel.size() > 0 ) {
      //  info()<<" Now find best cand for NoSel "<<(int) aCndNoSel.size() <<  "   "
      //      << (int) aIps.size()<< "  "<<(int) aProb.size()<< endmsg;
      
      int aIndexNoSel = m_Bs2DsHMvaUtilityTool-> FindBestCandidateInEv(aCndNoSel, aIps, aProb,1);
      fillHistoNoSel ( partsBs,aIndexNoSel);
      
    }
    
    //  info()<<" Filled all histo "<<endmsg;
    
    
}

double B2DHMvaMonitorHistoTool::getBsMassDiffValue(const LHCb::Particles * partsBs, int aIndex) {

  //  info()<<" Now get Bs Mass diff for histo "<< aIndex<<endmsg;
  
  //    const LHCb::Particles * partsBs = 
  //      get<LHCb::Particles>(m_Bs2DsHMvaUtilityTool -> 
  //       getcurrentBsLocation( m_B2DHSelectionParamTool-> get_current_B2DH_channel_Number()));
    

    // begin test
    int aNum=m_B2DHSelectionParamTool-> get_current_B2DH_channel_Number();

    //info()<<"Test Bslocation "<<m_Bs2DsHMvaUtilityTool ->getcurrentBsLocation(aNum)<<endmsg;

    //info()<< "Test Chnum partsBs "<< aNum <<"   "<<partsBs<<endmsg;
    // info()<< "Test partsBs Size " <<(int) partsBs->size()<<endmsg;
  
    //end test  
   LHCb::Particles::const_iterator iParticleBs = partsBs->begin() +aIndex;
   LHCb::Particle* aB = (*iParticleBs);
   double  aBsMassDiffWithSign = aB-> measuredMass()- (m_Bs2DsHMvaUtilityTool ->massStdBsValue() );
   // info()<<" Acquired Bs mass diff for histo "<<aBsMassDiffWithSign<<endmsg;
   

   return  aBsMassDiffWithSign;
}

void B2DHMvaMonitorHistoTool::fillHistoAfterSelWithL0(const LHCb::Particles * partsBs,int aIndex ) {
  fillHistoAfterAllSelBeforeL0( partsBs,aIndex );
  double aBsMassDiff = getBsMassDiffValue(partsBs,aIndex);
  plot1D(aBsMassDiff, "BsMassAfterSelWithL0", "Bs Mass After All Selection,  with L0", -60.0, 60.0, 120);  
     
}
void B2DHMvaMonitorHistoTool::fillHistoAfterAllSelBeforeL0(const LHCb::Particles * partsBs,int aIndex){
  fillHistoAfterAuxSel( partsBs,aIndex );
  double aBsMassDiff = getBsMassDiffValue( partsBs,aIndex);
  plot1D(aBsMassDiff, "BsMassAfterAllSelNoL0", "Bs Mass After All Selection,  before L0", -60.0, 60.0, 120);  
  
}

void B2DHMvaMonitorHistoTool::fillHistoAfterAuxSel(const LHCb::Particles * partsBs,int aIndex ) {
   fillHistoAfterFisherSel(partsBs,aIndex );     
   double aBsMassDiff = getBsMassDiffValue(partsBs,aIndex);
   plot1D(aBsMassDiff, "BsMassAfterFisher", "Bs Mass After Aux Cuts ", -500.0, 500.0, 100); 
}
void B2DHMvaMonitorHistoTool::fillHistoAfterFisherSel(const LHCb::Particles * partsBs,int aIndex ) {
   fillHistoAfterPrelimSel( partsBs,aIndex );
   double aBsMassDiff = getBsMassDiffValue(partsBs,aIndex);
   plot1D(aBsMassDiff, "BsMassAfterPrelimSel", "Bs Mass After FisherD Sel Cut", -500.0, 500.0, 100);  
}
void B2DHMvaMonitorHistoTool::fillHistoAfterPrelimSel(const LHCb::Particles * partsBs,int aIndex ) {
   fillHistoNoSel ( partsBs,aIndex ) ;
  
   double aBsMassDiff = getBsMassDiffValue(partsBs,aIndex);
   plot1D(aBsMassDiff, "BsMassAtSelBegin", "Bs Mass After Prelim Sel Cuts ", -500.0, 500.0, 100);  
}
void B2DHMvaMonitorHistoTool::fillHistoNoSel (const LHCb::Particles * partsBs,int aIndex ) 
{
   double aBsMassDiff = getBsMassDiffValue(partsBs,aIndex);
  
   plot1D(aBsMassDiff, "BsMassAtSelBegin", "Bs Mass At Selection Beginning ", -500.0, 500.0, 100);   
}



