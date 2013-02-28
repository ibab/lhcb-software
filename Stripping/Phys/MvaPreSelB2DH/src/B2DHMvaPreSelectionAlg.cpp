// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/L0DUReport.h"


// local
#include "B2DHMvaPreSelectionAlg.h"
using namespace LHCb ;
using namespace Gaudi::Units ;


//-----------------------------------------------------------------------------
// Implementation file for class : B2DHMvaPreSelectionAlg
//
// 2010-07-19 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( B2DHMvaPreSelectionAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
B2DHMvaPreSelectionAlg::B2DHMvaPreSelectionAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator ),
    m_activateHistograms(false),
    m_activateL0Selection(false) ,
    m_activatedPVRefit(true),
    m_searchBestCandInEv(false),
    m_fProb(std::vector<double> (0)),
    m_fIps  (std::vector<double> (0)),
    m_fCandIndex(std::vector<int> (0)),
    m_fCandNoSel(std::vector<int> (0)),
    m_fCandPrelimSel(std::vector<int> (0)),
    m_fCandFisherSel(std::vector<int> (0)),
    m_fCandAuxSel (std::vector<int> (0)),
    m_fCandAuxMassSel (std::vector<int> (0)),
    m_ActivateStrippingSelection(true)
{
  declareProperty("ActivateHistograms",m_activateHistograms);
  declareProperty("ActivateL0Selection", m_activateL0Selection);
  declareProperty("SearchBestCandidateInEvent",m_searchBestCandInEv);
  declareProperty("ActivatePrimaryVertexRefit",m_activatedPVRefit);
  declareProperty("CurrentB2DHDecayChannelNumber", m_curDecayChannel );
  declareProperty("ActivateStrippingSelection",  m_ActivateStrippingSelection);
  

}
//=============================================================================
// Destructor
//=============================================================================
B2DHMvaPreSelectionAlg::~B2DHMvaPreSelectionAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode B2DHMvaPreSelectionAlg::initialize() {
  StatusCode sc = DaVinciHistoAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_B2DHPreselMvaUtilityTool = tool<IB2DHPreselMvaUtilityTool> ("B2DHPreselMvaUtilityTool");

  m_B2DHMvaPreSelectionParamTool= tool<IB2DHMvaPreSelectionParamTool> ("B2DHMvaPreSelectionParamTool");  

  info()<< "Current decay channel at init "<<m_curDecayChannel<<endmsg;
  
  m_B2DHMvaPreSelectionParamTool->set_current_B2DH_channel_Number(m_curDecayChannel);
  m_B2DHMvaPreSelectionParamTool->initializeLocalParam();
  m_B2DHMvaPreSelectionParamTool->BookMvaForFisherD(m_B2DHMvaPreSelectionParamTool->get_weightFileName(),
                                               m_B2DHMvaPreSelectionParamTool->get_classifierName());
  
  m_B2DHMvaPreSelectionCutsTool = tool<IB2DHMvaPreSelectionCutsTool>("B2DHMvaPreSelectionCutsTool");

  m_B2DHPreselMvaFisherDMonitorHistoTool  =
                   tool<IB2DHPreselMvaFisherDMonitorHistoTool>("B2DHPreselMvaFisherDMonitorHistoTool");

  resetSelectionCounters();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode B2DHMvaPreSelectionAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  

   m_numTotEvCounter++;
   bool selCurEv=false;
   
   const LHCb::Particles * partsBs;
   try {


      for (std::vector<std::string>::const_iterator iLoc= inputLocations().begin() ; iLoc != inputLocations().end(); ++iLoc ) {     
       if(! selCurEv ) {
         
         partsBs =get<LHCb::Particles>( (*iLoc) + "/Particles" );

         // partsBs =get<LHCb::Particles>(m_B2DHPreselMvaUtilityTool -> getcurrentBsLocation( m_curDecayChannel));
           verbose()<<" Current Bs Location partsBs "<<m_B2DHPreselMvaUtilityTool -> getcurrentBsLocation( m_curDecayChannel)<<" "
                 <<partsBs<< "   "<<endmsg;

          if(partsBs) {
            int aSize= (int) partsBs->size();
            resetEventSelectionParams(aSize);
          
           debug()<<" Current Bs Location partsBs NumCand "<<m_B2DHPreselMvaUtilityTool -> getcurrentBsLocation( m_curDecayChannel)<<" "
                 <<partsBs<< "   "<< aSize<< endmsg;
          
            debug()<<" Current Bs Location partsBs and size  "<<(*iLoc) + "/Particles" <<"   "<<(int) partsBs->size()<<  endmsg;

          if ( aSize > 0 ) {
             m_numEvWithBs++;

              const LHCb::Particle* aSelectedBparticle = performB2DHSelection ( partsBs ) ;
              if(aSelectedBparticle) selCurEv=true;
            
        
          }else {
            debug()<<" No Bs candidates in this channel in this event although the Bs Location exists"<<endmsg;
          }       
         
       } // end test on partsBs
      }// end test on current event already selected or no. If already selected no further selection tested from other locations.

    } // end loop over the input locations.
     
    if(  selCurEv ) {  // event is selected.
            m_numSelectedEvCounter++;
             setFilterPassed(true);  
              debug()<<" This event is selected by FisherD+cuts  "<<endmsg;

    }else {

            m_numNotSelectedEvCounter++;
             setFilterPassed(false);  
             debug()<<" This event not Selected by FisherD+cuts "<<endmsg;
    }



     
   }catch(...) {
     partsBs=0;
     setFilterPassed(false);  // Mandatory. Set to true if event is accepted. 

      debug()<<" No Preselected Bs in this event , possibly no BsLocation present "<<endmsg; 
   }

   //    info()<< "End of the current event in B2DHMvaPreSelectionAlg  "<<endmsg;
   
   //  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 

  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode B2DHMvaPreSelectionAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  PrintEvSelCounters();

  return DaVinciHistoAlgorithm::finalize();
}
//=============================================================================

const LHCb::Particle* B2DHMvaPreSelectionAlg::performB2DHSelection ( const LHCb::Particles * partsBs ) {

 try {

   bool aL0Selected =true;
   if(m_activateL0Selection ) aL0Selected = m_B2DHPreselMvaUtilityTool->getL0Decision ();
     
   // the following assumes that partsBs already exists with non-zero Bs candidates as verified when this method is called.
   LHCb::RecVertex::Container* StdPVs = get<LHCb::RecVertices>(m_B2DHPreselMvaUtilityTool -> 
                                                          getcurrentStdPVLocation() );

   LHCb::Particles::const_iterator iParticleBsBegin = partsBs->begin();
   LHCb::Particles::const_iterator iParticleBsEnd   = partsBs->end();
   LHCb::Particles::const_iterator iParticleBs=iParticleBsBegin;
 
   int curIndex=0;
   bool selAccomplished=false;
   while( (iParticleBs != iParticleBsEnd) && (!selAccomplished) ) {

      // for (iParticleBs = iParticleBsBegin; iParticleBs != iParticleBsEnd; iParticleBs++) { // loop over Bs candidates
      curIndex = iParticleBs - iParticleBsBegin;
      LHCb::Particle* aB = (*iParticleBs);

         if(aB) {
           
            m_BCandinEv++;
            const  LHCb::RecVertex* aPV = 
                (m_activatedPVRefit) ? getBestRefittedPVForBs (aB,StdPVs) : AcquirePVForBsFromStdPV(aB,StdPVs)  ;
         
            if(aPV ) {

            
           m_B2DHMvaPreSelectionParamTool ->  acquireSelectionParams(aB,aPV);
           m_B2DHMvaPreSelectionCutsTool-> setAllCutFlags();

           m_PVSelCandInEv++;
           m_fIps.push_back( m_B2DHMvaPreSelectionParamTool->ipsBs());
           m_fProb.push_back( m_B2DHMvaPreSelectionParamTool->getfisherDProbValue());
           m_fCandNoSel.push_back(curIndex);

           if( m_B2DHMvaPreSelectionCutsTool ->  ApplyPreTrainSel()) {
             m_PreLimSelCandInEv++;
             m_fCandPrelimSel.push_back(curIndex);
             debug()<<"Preselection successful for this candidate "<<endmsg;
             if(m_B2DHMvaPreSelectionCutsTool  -> ApplyFisherSel(m_curDecayChannel) ) {
                m_FisherSelCandInEv++;
                m_fCandFisherSel.push_back(curIndex);

                debug()<< " Selected by FisherD "<<endmsg;
                if((m_ActivateStrippingSelection) ||  
                   ( m_B2DHMvaPreSelectionCutsTool -> ApplyAuxCuts(m_curDecayChannel)) ) {
                  m_AuxCutSelCandInEv++;
                  m_fCandAuxSel.push_back(curIndex);
                  debug()<<" Selected by Auxiliary cuts "<<endmsg;
                  if((m_ActivateStrippingSelection) ||  
                     ( m_B2DHMvaPreSelectionCutsTool  ->ApplyMassWindowCuts(m_curDecayChannel))) {
                    debug()<<"Within Mass Windows "<<endmsg;
                        m_AllSelCandBeforeL0InEv++;
                        m_fCandAuxMassSel.push_back(curIndex);
                        
                        if( (m_ActivateStrippingSelection) || ( aL0Selected ) ) {
                          debug()<<" L0 Selected "<<endmsg;


                           m_AllSelCandWithL0InEv++;      
                           m_numCandInEvent++;
                           m_fCandIndex.push_back(curIndex);
                           if(! m_searchBestCandInEv ) selAccomplished=true;
                           

                       }// end L0 sel
                       
                     }// end mass window sel
                         
                
                  
                } // end aux cut sel
                
                
             }// end fisher sel
             
              
              
           }// end of presel application
           

         }else {  // end check on PV
           warning () <<" No primary vertex in this event for the current preselected B candidate"<<endmsg;
         }
         
         }// end check on current B candiate existence

         
         iParticleBs++;
         

       } // end while loop over Bs candidates.


         
       BumpMvaStrEvCounters();
       

        LHCb::Particle* bestB =0;
        bool bSelected =false;


       if(m_numCandInEvent > 0) {// at least one candidate survived. Now find the best candidate.
         m_curbestCandIndexInEvent = m_B2DHPreselMvaUtilityTool->FindBestCandidateInEv(m_fCandIndex , m_fIps, m_fProb,0);
         //The best candidate is now used

         Particles::const_iterator bestParticleBs =  iParticleBsBegin +m_curbestCandIndexInEvent;
         
         bestB = (*bestParticleBs);
         bSelected=true;
         
       }
       

              
        const   LHCb::Particle* aBparticle = bSelected ? bestB : 0;
       
       // now for histogramming
       if( m_activateHistograms) {

         // info()<<" NumCand Current best cand   "<< m_numCandInEvent <<"   "<<m_curbestCandIndexInEvent<<endmsg;
         m_B2DHPreselMvaFisherDMonitorHistoTool ->
                    fillHistoForEventSelectionSteps(partsBs, m_numCandInEvent,m_curbestCandIndexInEvent,
                                                                           m_fProb,m_fIps,m_fCandNoSel,
                                                                           m_fCandPrelimSel,m_fCandFisherSel,m_fCandAuxSel,
                                                                           m_fCandAuxMassSel,m_fCandIndex);
         
         
       }

       return aBparticle;

 }catch (...) {
         return 0;
 }
       
}
void B2DHMvaPreSelectionAlg::BumpMvaStrEvCounters() {
        if(m_BCandinEv > 0 ) m_numEvWitbBsAllChannelsInput++;
       if(m_PVSelCandInEv > 0) m_numEvWitbBsAndPV++;
       if(m_PreLimSelCandInEv>0) m_numEvPrelimSel++;
       if(m_FisherSelCandInEv>0) m_numEvWithFisher++;
       if(m_AuxCutSelCandInEv > 0 ) m_numEvWithAuxCuts++;
       if(m_AllSelCandBeforeL0InEv>0) m_numEvAllSelBeforeL0++;
       if(m_AllSelCandWithL0InEv>0) m_numEvAllSelWithL0++;

 
}

const  LHCb::RecVertex* B2DHMvaPreSelectionAlg::getBestRefittedPVForBsFromSortedRelations(const LHCb::Particle* b,
                                                                      LHCb::RecVertex::Container* aStdPVs ) {
   const Particle2Vertex::WTable* table = get<Particle2Vertex::WTable>(m_B2DHPreselMvaUtilityTool ->
                                 getparticle2RefittedSortedPVRelationsLoc(m_curDecayChannel ) );
   const  Particle2Vertex::WTable::Range range =  table->relations( b );
  const LHCb::RecVertex* abestPV =  range.empty() ? 0 : dynamic_cast<const LHCb::RecVertex*>( range.back().to() );
 
   const LHCb::RecVertex* abestPVR=   ( abestPV !=0 ) ? abestPV : AcquirePVForBsFromStdPV( b,aStdPVs );  
  // if no refitted PV present, it goes to the standard non-refitted PV and finds the best PV available.
   return abestPVR;  
}


//==============================================================================================

const  LHCb::RecVertex* B2DHMvaPreSelectionAlg::getBestRefittedPVForBs(const LHCb::Particle* b,
                                                          LHCb::RecVertex::Container* aStdPVs ) {   

  // const Particle2Vertex::Table* table = 
  //      get<Particle2Vertex::Table>("Phys/PVReFitterAlgBs2DsK/Particle2VertexRelations"  );

    const Particle2Vertex::Table* table = 
        get<Particle2Vertex::Table>(m_B2DHPreselMvaUtilityTool-> getparticle2RefittedPVRelationsLoc (m_curDecayChannel ) );

    const  Particle2Vertex::Table::Range range = table->relations( b );

    
    
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
  
  const LHCb::RecVertex* abestPVR=   ( aPV !=0 ) ? aPV : AcquirePVForBsFromStdPV( b,aStdPVs  );  
  // if no refitted PV present, it goes to the standard non-refitted PV and finds the best PV available.

  return abestPVR;  
}


//================================================================================================

const  LHCb::RecVertex* B2DHMvaPreSelectionAlg::AcquirePVForBsFromStdPV ( const LHCb::Particle* b,
                                                               LHCb::RecVertex::Container* aStdPVs ) 
{

  // info()<<" current Pv location "<<m_B2DHPreselMvaUtilityTool ->getcurrentStdPVLocation()<<endmsg;
  
  try {
    
    //  LHCb::RecVertex::Container* PVs = get<LHCb::RecVertices>(m_B2DHPreselMvaUtilityTool -> 
    //                                                      getcurrentStdPVLocation() );

    LHCb::RecVertex::Container* PVs =   aStdPVs;

  int numPvs= (PVs) ? ( (int) PVs->size())  : 0;
  


  if(numPvs > 0 ) {
    double ipsMinQ=10000.0;  
    int ipvSaveQ=0;
    if( numPvs > 1 ) {
      
      for(LHCb::RecVertex::Container::iterator ive = PVs->begin(); ive != PVs->end(); ive++) { // loop over Std PV
               double fomq=-100.0;
               double fomSq= -100.0;
               double chisqq=-100.0;
               StatusCode sc= distanceCalculator()->distance(b, (*ive), fomq, chisqq);
               if(chisqq >0 ) fomSq=sqrt(chisqq);
               if( fomSq < ipsMinQ ){
                       ipsMinQ=fomSq;
                       ipvSaveQ=   ive -  PVs->begin();
               }
               
      }// end loop over vertices.
    }// end check on numPV
    
    LHCb::RecVertex::Container::iterator ivea = ( PVs->begin() )  + ipvSaveQ;
    const LHCb::RecVertex* aPVF = (*ivea);

    return aPVF;


  } else {

    
    LHCb::RecVertex * apv =0;
    return apv  ;
  }
  }catch (...){

    LHCb::RecVertex * apv =0;
    return apv  ;
      
  }
  
  
  
}

//=====================================================================================

void B2DHMvaPreSelectionAlg::resetEventSelectionParams( int aNumCandSize ) {

  m_BCandinEv=0;
  m_PVSelCandInEv=0;
  m_PreLimSelCandInEv=0;
  m_FisherSelCandInEv=0;
  m_AuxCutSelCandInEv=0;
  m_AllSelCandBeforeL0InEv=0;
  m_AllSelCandWithL0InEv=0;
  m_numCandInEvent=0;
  


         m_curbestCandIndexInEvent=-10;  // default set to unphyical value .

         m_fProb.clear(); m_fProb.reserve(aNumCandSize); // for monitor histo
         m_fIps.clear();  m_fIps.reserve(aNumCandSize); // for monitor histo
         
         m_fCandIndex.clear(); m_fCandIndex.reserve(aNumCandSize);

         m_fCandNoSel.clear(); m_fCandNoSel.reserve(aNumCandSize);
         m_fCandPrelimSel.clear(); m_fCandPrelimSel.reserve(aNumCandSize);
         m_fCandFisherSel.clear(); m_fCandFisherSel.reserve(aNumCandSize);
         m_fCandAuxSel.clear(); m_fCandAuxSel.reserve(aNumCandSize);
         m_fCandAuxMassSel.clear(); m_fCandAuxMassSel.reserve(aNumCandSize);

  
  
}
void B2DHMvaPreSelectionAlg::resetSelectionCounters(){
  
  m_numTotEvCounter=0;
  m_numEvWithBs =0;

  m_numEvWitbBsAllChannelsInput=0;
  m_numEvWitbBsAndPV=0;
  m_numEvPrelimSel=0;
  m_numEvWithFisher=0;
  m_numEvWithAuxCuts=0;
  m_numEvAllSelBeforeL0=0;
  m_numEvAllSelWithL0 = 0;
  m_numSelectedEvCounter=0;
  m_numNotSelectedEvCounter=0;
}

void B2DHMvaPreSelectionAlg::PrintEvSelCounters(){
  
  info() <<" Is L0 activated in this job ? :    " <<  m_activateL0Selection <<endmsg;
  info()<<" Begin B2DHMvaPreSelectionAlg Counters "<<endmsg;
  info()<< "NumEvTot  NumSelEv NumNotSelEv "<< m_numTotEvCounter<<"   "
        <<  m_numSelectedEvCounter<<"   "<<m_numNotSelectedEvCounter<<endmsg;

  info()<< "NumEvTotInput InChannels InputNumEv WithPV PrelimSel FisherD AuxCuts AllSelbeforeL0 AllSelAfterL0 "<<m_numTotEvCounter<< "   "
        << m_numEvWitbBsAllChannelsInput<<"   "
        << m_numEvWitbBsAndPV<<"   " << m_numEvPrelimSel<<"   "
        <<m_numEvWithFisher<<"   "<<m_numEvWithAuxCuts<<"   "
        <<m_numEvAllSelBeforeL0<<"   "<< m_numEvAllSelWithL0<<endmsg;

  info()<<" End of B2DHMvaPreSelectionAlg Counters "<<endmsg;
  
}

