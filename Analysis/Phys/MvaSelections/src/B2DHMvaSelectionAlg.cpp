// $Id: B2DHMvaSelectionAlg.cpp,v 1.1 2010/06/10 12:42:21 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/L0DUReport.h"

// local
#include "B2DHMvaSelectionAlg.h"
using namespace LHCb ;
using namespace Gaudi::Units ;

//-----------------------------------------------------------------------------
// Implementation file for class : B2DHMvaSelectionAlg
//
// 2009-07-21 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( B2DHMvaSelectionAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2DHMvaSelectionAlg::B2DHMvaSelectionAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_activateHistograms(false),
    m_activateL0Selection(false) ,
    m_fProb(std::vector<double> (0)),
    m_fIps  (std::vector<double> (0)),
    m_fCandIndex(std::vector<int> (0)),
    m_fCandNoSel(std::vector<int> (0)),
    m_fCandPrelimSel(std::vector<int> (0)),
    m_fCandFisherSel(std::vector<int> (0)),
    m_fCandAuxSel (std::vector<int> (0)),
    m_fCandAuxMassSel (std::vector<int> (0))
{
  declareProperty("ActivateHistograms",m_activateHistograms);
  declareProperty("ActivateL0Selection", m_activateL0Selection);
  declareProperty("CurrentB2DHDecayChannelNumber", m_curDecayChannel ); 
    
}
//=============================================================================
// Destructor
//=============================================================================
B2DHMvaSelectionAlg::~B2DHMvaSelectionAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode B2DHMvaSelectionAlg::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_Bs2DsHMvaUtilityTool = tool<IBs2DsHMvaUtilityTool> ("Bs2DsHMvaUtilityTool");

  m_IB2DHSelectionParamTool= tool<IB2DHSelectionParamTool> ("B2DHSelectionParamTool");  

  info()<< "Current decay channel at init "<<m_curDecayChannel<<endmsg;
  
  m_IB2DHSelectionParamTool->set_current_B2DH_channel_Number(m_curDecayChannel);
  m_IB2DHSelectionParamTool->initializeLocalParam();
  m_IB2DHSelectionParamTool->BookMvaForFisherD(m_IB2DHSelectionParamTool->get_weightFileName(),
                                               m_IB2DHSelectionParamTool->get_classifierName());
  
  m_B2DHSelectionCutsTool = tool<IB2DHSelectionCutsTool>("B2DHSelectionCutsTool");

  m_B2DHMvaMonitorHistoTool  =
                   tool<IB2DHMvaMonitorHistoTool>("B2DHMvaMonitorHistoTool");

  resetSelectionCounters();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode B2DHMvaSelectionAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute  B2DHMvaSelectionAlg " << endmsg;

  // code goes here  

  m_numTotEvCounter++;
  
   const LHCb::Particles * partsBs;
   try {
     
     partsBs =get<LHCb::Particles>(m_Bs2DsHMvaUtilityTool -> getcurrentBsLocation( m_curDecayChannel));
     
    if(partsBs) {
       if ( (int) partsBs->size() > 0 ) {
         m_numEvWithBs++;
         //info()<<" Number of Bs candidates in this event = "<< (int) partsBs->size() <<endmsg;

         const LHCb::Particle* aSelectedBparticle = performB2DHSelection () ;      

         if(aSelectedBparticle ) {  // event is selected.
            m_numSelectedEvCounter++;
            setFilterPassed(true);  
            debug()<<" This event is selected by FisherD+cuts  "<<endmsg;
            
         }else {

            m_numNotSelectedEvCounter++;
            setFilterPassed(false);  
            debug()<<" This event not Selected by FisherD+cuts "<<endmsg;
         }

        
       }else {
         info()<<" No Bs candidates in this event although the Bs Location exists "<<endmsg;
       }       
       
    }    
    
   }catch(...) {
     partsBs=0;
     info()<<" No Preselected Bs in this event , possibly no BsLocation present "<<endmsg; 
   }

   // info()<< "End of the current event in B2DHMvaSelectionAlg  "<<endmsg;
   
   //  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode B2DHMvaSelectionAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
    
  PrintEvSelCounters();
  
  return DVAlgorithm::finalize();
}

//=============================================================================
const LHCb::Particle* B2DHMvaSelectionAlg::performB2DHSelection () {

      resetEventSelectionParams();
      bool aL0Selected =true;
      if(m_activateL0Selection ) aL0Selected = m_Bs2DsHMvaUtilityTool->getL0Decision ();
     
  // the following assumes that partsBs already exists with non-zero Bs candidates as verified when this method is called.

       const LHCb::Particles *  partsBs =
          get<LHCb::Particles>(m_Bs2DsHMvaUtilityTool -> getcurrentBsLocation( m_curDecayChannel));
       

       LHCb::Particles::const_iterator iParticleBs;
       LHCb::Particles::const_iterator iParticleBsBegin = partsBs->begin();
       LHCb::Particles::const_iterator iParticleBsEnd   = partsBs->end();
       
       int curIndex=0;



       // info()<< "Now in performB2DHSelection for NumBs Cand "<< (int) partsBs->size()<<endmsg;
       
       for (iParticleBs = iParticleBsBegin; iParticleBs != iParticleBsEnd; iParticleBs++) { // loop over Bs candidates
         curIndex = iParticleBs - iParticleBsBegin;
         LHCb::Particle* aB = (*iParticleBs);
         //info()<<" Current Bs candidate num "<<curIndex<<endmsg;
         
         const  LHCb::RecVertex* aPV = getBestRefittedPVForBs (aB);
         if(aPV ) {
           m_IB2DHSelectionParamTool ->  acquireSelectionParams(aB,aPV);
           m_B2DHSelectionCutsTool-> setAllCutFlags();
           
           m_PVSelCandInEv++;
           m_fIps[curIndex]=m_IB2DHSelectionParamTool->ipsBs();
           m_fProb[curIndex]=m_IB2DHSelectionParamTool->getfisherDProbValue();
           m_fCandNoSel.push_back(curIndex);
           
           if( m_B2DHSelectionCutsTool ->  ApplyPreTrainSel()) {
             m_PreLimSelCandInEv++;
             m_fCandPrelimSel.push_back(curIndex);
             // info()<<"Preselection successful for this candidate "<<endmsg;
             if(m_B2DHSelectionCutsTool  -> ApplyFisherSel(m_curDecayChannel) ) {
                m_FisherSelCandInEv++;
                m_fCandFisherSel.push_back(curIndex);

                //info()<< " Selected by FisherD "<<endmsg;
                if( m_B2DHSelectionCutsTool -> ApplyAuxCuts(m_curDecayChannel) ) {
                  m_AuxCutSelCandInEv++;
                  m_fCandAuxSel.push_back(curIndex);
                  //info()<<" Selected by Auxiliary cuts "<<endmsg;
                  if(m_B2DHSelectionCutsTool ->ApplyMassWindowCuts(m_curDecayChannel)) {
                    //info()<<"Within Mass Windows "<<endmsg;
                        m_AllSelCandBeforeL0InEv++;
                        m_fCandAuxMassSel.push_back(curIndex);
                        
                        if( aL0Selected  ) {
                          //info()<<" L0 Selected "<<endmsg;
                           m_AllSelCandWithL0InEv++;      
                           m_numCandInEvent++;
                           m_fCandIndex.push_back(curIndex);
                           
                           

                       }// end L0 sel
                       
                     }// end mass window sel
                         
                
                  
                } // end aux cut sel
                
                
             }// end fisher sel
             
              
              
           }// end of presel application
           
           
         }else {  // end check on PV
           warning () <<" No primary vertex in this event for the current preselected B candidate"<<endmsg;
         }
         
         
       } // end loop over Bs candidates.


       if(m_PVSelCandInEv > 0) m_numEvWitbBsAndPV++;
       if(m_PreLimSelCandInEv>0) m_numEvPrelimSel++;
       if(m_FisherSelCandInEv>0) m_numEvWithFisher++;
       if(m_AuxCutSelCandInEv > 0 ) m_numEvWithAuxCuts++;
       if(m_AllSelCandBeforeL0InEv>0) m_numEvAllSelBeforeL0++;
       if(m_AllSelCandWithL0InEv>0) m_numEvAllSelWithL0++;


        LHCb::Particle* bestB =0;
        bool bSelected =false;


       if(m_numCandInEvent > 0) {// at least one candidate survived. Now find the best candidate.
         m_curbestCandIndexInEvent = m_Bs2DsHMvaUtilityTool->FindBestCandidateInEv(m_fCandIndex , m_fIps, m_fProb,0);
         //The best candidate is now used

         Particles::const_iterator bestParticleBs =  iParticleBsBegin +m_curbestCandIndexInEvent;
         
         bestB = (*bestParticleBs);
         bSelected=true;
         
       }
       
        const   LHCb::Particle* aBparticle = bSelected ? bestB : 0;
       // now for histogramming
       if( m_activateHistograms) {

         m_B2DHMvaMonitorHistoTool-> fillHistoForEventSelectionSteps(partsBs, m_numCandInEvent,m_curbestCandIndexInEvent,
                                                                           m_fProb,m_fIps,m_fCandNoSel,
                                                                           m_fCandPrelimSel,m_fCandFisherSel,m_fCandAuxSel,
                                                                           m_fCandAuxMassSel,m_fCandIndex);
         
         
       }
              


       return aBparticle;
       
}

const  LHCb::RecVertex* B2DHMvaSelectionAlg::getBestRefittedPVForBsFromSortedRelations(const LHCb::Particle* b) {
   const Particle2Vertex::WTable* table = get<Particle2Vertex::WTable>(m_Bs2DsHMvaUtilityTool ->
                                 getparticle2RefittedSortedPVRelationsLoc(m_curDecayChannel ) );
   const  Particle2Vertex::WTable::Range range =  table->relations( b );
  const LHCb::RecVertex* abestPV =  range.empty() ? 0 : dynamic_cast<const LHCb::RecVertex*>( range.back().to() );
 
   const LHCb::RecVertex* abestPVR=   ( abestPV !=0 ) ? abestPV : AcquirePVForBsFromStdPV( b );  
  // if no refitted PV present, it goes to the standard non-refitted PV and finds the best PV available.
   return abestPVR;  
}


//==============================================================================================

const  LHCb::RecVertex* B2DHMvaSelectionAlg::getBestRefittedPVForBs(const LHCb::Particle* b) {   

  // const Particle2Vertex::Table* table = 
  //      get<Particle2Vertex::Table>("Phys/PVReFitterAlgBs2DsK/Particle2VertexRelations"  );

    const Particle2Vertex::Table* table = 
        get<Particle2Vertex::Table>(m_Bs2DsHMvaUtilityTool-> getparticle2RefittedPVRelationsLoc (m_curDecayChannel ) );

    const  Particle2Vertex::Table::Range range =  table->relations( b );
    
    LHCb::RecVertex* aPV = 0;
      int indexSave=-1;
    if(! (range.empty() )) {
      double aIpsBsSave=10000.0;
      int aS=(int) range.size();
      for(int i=0; i<aS; ++i ) {
        aPV= dynamic_cast<LHCb::RecVertex*>( (range.begin()+i)->to() );
        
        double fom=0.0;
        double chisq=0.0;
        double aIpsBs=0.0;
        StatusCode sc =  distanceCalculator()-> distance(b, aPV , fom, chisq);
        if( chisq > 0 ) aIpsBs =sqrt(chisq);
        // info()<<" Current i  PV  IPS "<<i<<"  "<<aPV->position()<<"   "
        //      << aIpsBs<< endmsg;

        if(  aIpsBs <   aIpsBsSave ) { 
           aIpsBsSave=aIpsBs;
           indexSave=i;
      }
        
      } // end loop over i
      if(indexSave > -1 )  aPV=dynamic_cast<LHCb::RecVertex*>( (range.begin()+indexSave)->to() ); 
    }
    
    // test print
    //info()<<"  abestPVIndex "<<indexSave<<endmsg;
    // if(indexSave > -1 ) {
    //  info()<<" abest PVpos "<<aPV->position()<<endmsg;
    //  }
    // end test print
  
  const LHCb::RecVertex* abestPVR=   ( aPV !=0 ) ? aPV : AcquirePVForBsFromStdPV( b );  
  // if no refitted PV present, it goes to the standard non-refitted PV and finds the best PV available.

  return abestPVR;  
}


//================================================================================================

const  LHCb::RecVertex* B2DHMvaSelectionAlg::AcquirePVForBsFromStdPV ( const LHCb::Particle* b ) 
{
  
  LHCb::RecVertex::Container* PVs = get<LHCb::RecVertices>(m_Bs2DsHMvaUtilityTool -> 
                                                          getcurrentStdPVLocation() );

  if(PVs) {
    double ipsMinQ=10000.0;  
    int ipvSaveQ=0;
    for(LHCb::RecVertex::Container::iterator ive = PVs->begin(); ive != PVs->end(); ive++) { // loop over Std PV
               double fomq=-100.0;
               double fomSq= -100.0;
               double chisqq=-100.0;
               StatusCode sc= distanceCalculator()->distance(b, (*ive), fomq, chisqq);
               if(chisqq >0 ) fomSq=sqrt(chisqq);
               if( fomSq < ipsMinQ ){
                       ipsMinQ=fomSq;
                       ipvSaveQ=   ive-  PVs->begin();
               }
               
    }// end loop over vertices.
  
    LHCb::RecVertex::Container::iterator ivea = ( PVs->begin() )  + ipvSaveQ;
    const LHCb::RecVertex* aPVF = (*ivea);
    return aPVF;


  } else {
    LHCb::RecVertex * apv =0;
    return apv  ;
  }
  
}

//=====================================================================================

void B2DHMvaSelectionAlg::resetEventSelectionParams() {

  m_PVSelCandInEv=0;
  m_PreLimSelCandInEv=0;
  m_FisherSelCandInEv=0;
  m_AuxCutSelCandInEv=0;
  m_AllSelCandBeforeL0InEv=0;
  m_AllSelCandWithL0InEv=0;
  m_numCandInEvent=0;
  


     m_curbestCandIndexInEvent=-10;  // default set to unphyical value .

       if( m_activateHistograms) {
         const LHCb::Particles * partsBs = 
            get<LHCb::Particles>(m_Bs2DsHMvaUtilityTool -> getcurrentBsLocation(m_curDecayChannel));
         int aSize= (int) partsBs->size();
         m_fProb.resize(aSize); m_fProb.assign(aSize,-1.0);  // for monitor histogramming
         m_fIps.resize(aSize);  m_fIps.assign(aSize,-10.0);   // for monitor histogramming
         m_fCandIndex.clear(); m_fCandIndex.reserve(aSize);

         m_fCandNoSel.clear(); m_fCandNoSel.reserve(aSize);
         m_fCandPrelimSel.clear(); m_fCandPrelimSel.reserve(aSize);
         m_fCandFisherSel.clear(); m_fCandFisherSel.reserve(aSize);
         m_fCandAuxSel.clear(); m_fCandAuxSel.reserve(aSize);
         m_fCandAuxMassSel.clear(); m_fCandAuxMassSel.reserve(aSize);
         
         
       }

  
  
}
void B2DHMvaSelectionAlg::resetSelectionCounters(){
  
  m_numTotEvCounter=0;
  m_numEvWithBs =0;
  m_numEvWitbBsAndPV=0;
  m_numEvPrelimSel=0;
  m_numEvWithFisher=0;
  m_numEvWithAuxCuts=0;
  m_numEvAllSelBeforeL0=0;
  m_numEvAllSelWithL0 = 0;
  m_numSelectedEvCounter=0;
  m_numNotSelectedEvCounter=0;
}

void B2DHMvaSelectionAlg::PrintEvSelCounters(){
  
  info() <<" Is L0 activated in this job ? :    " <<  m_activateL0Selection <<endmsg;
  info()<<" Begin B2DHMvaSelectionAlg Counters "<<endmsg;
  info()<< "NumEvTot  NumSelEv NumNotSelEv "<< m_numTotEvCounter<<"   "
        <<  m_numSelectedEvCounter<<"   "<<m_numNotSelectedEvCounter<<endmsg;

  info()<< "NumEvTot WithPV PrelimSel FisherD AuxCuts AllSelbeforeL0 AllSelAfterL0 "<<m_numTotEvCounter<< "   "
        << m_numEvWitbBsAndPV<<"   " << m_numEvPrelimSel<<"   "
        <<m_numEvWithFisher<<"   "<<m_numEvWithAuxCuts<<"   "
        <<m_numEvAllSelBeforeL0<<"   "<< m_numEvAllSelWithL0<<endmsg;

  info()<<" End of B2DHMvaSelectionAlg Counters "<<endmsg;
  
}

