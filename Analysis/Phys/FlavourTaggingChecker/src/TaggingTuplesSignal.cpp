// $Id: TaggingTuplesSignal.cpp,v 1.1 2008-03-14 17:02:16 pkoppenb Exp $
// Include files 

//############################################################################
// Implementation file for class :TaggingTuplesSignal
//
// 01-02-2007 : Onuora Awunor
// 
// To be run over MC truth Signal events.
// Algorithm called by ../options/DoubleTAggingAnalysis.opts to store Tagging 
// Decisions into NTUPLES to be used for DoubleTagging analysis
// NB: The ntuple is used as input in macro called ../options/DoubleTaggingAnalysis.C
//#############################################################################

// local
#include "TaggingTuplesSignal.h"
#include "Event/L0DUReport.h"
#include "Event/HltSummary.h"
#include "Event/HltEnums.h"

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( TaggingTuplesSignal );

using namespace LHCb ;
using namespace Gaudi;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TaggingTuplesSignal::TaggingTuplesSignal( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_forcedBtool(0)
{
  
}
//=============================================================================
// Destructor
//=============================================================================
TaggingTuplesSignal::~TaggingTuplesSignal() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TaggingTuplesSignal::initialize() {

  m_debugParticle = tool<IPrintDecayTreeTool> ( "PrintDecayTreeTool", this );
  if( ! m_debugParticle ) {
    fatal() << "Unable to retrieve PrintDecayTree tool "<< endreq;
    return StatusCode::FAILURE;
  }

  m_debugMCParticle = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if( ! m_debugMCParticle ) {
    fatal() << "Unable to retrieve PrintMCDecayTree tool "<< endreq;
    return StatusCode::FAILURE;
  }

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
    return StatusCode::FAILURE;
  }

    nSelected=0;
  for(int i=0; i<100; ++i) { nrt[i]=0; nwt[i]=0; }

  //  Now book ntuples
      NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/200");
  if ( !nt ) { // Check if already booked
   nt = ntupleSvc()->book ("/NTUPLES/FILE1", 200,
                          CLID_ColumnWiseTuple, "myhistos");
  
   if ( nt ) { // ntuple sucessfully booked  
     //     Add elements to ntuple definition:
    nt->addItem ("OSMuondec",    osmuondec);
    nt->addItem ("OSElectrondec",  oselectrondec);
    nt->addItem ("OSKaondec", oskaondec);
    nt->addItem ("SSKaondec", sskaondec);
    nt->addItem ("VtxChargedec", vtxchargedec);
    nt->addItem ("SSPiondec", sspiondec);
    nt->addItem ("SSJetdec", ssjetdec);
    nt->addItem ("Truetag",   Truetag);
    nt->addItem ("EvtNum", EvtNu);
    nt->addItem ("RunNum", RunNu);
    nt->addItem ("Events", Event);
    nt->addItem ("BsMass", Bsmass);
    nt->addItem ("BsID", Bsid);
    nt->addItem ("BsIDTag", BsidTag);
    nt->addItem ("MCBsID", MCBsid);
    nt->addItem ("MCBsMass", MCBsmass);
    nt->addItem ("MuMass", Mumass);
    nt->addItem ("MuID", Muid);
    nt->addItem ("DsMass", Dsmass);
    nt->addItem ("DsID", Dsid);
    nt->addItem ("L0",trig0);
    nt->addItem ("HLT", hlt0);
    // nt->addItem ("BsP", BsP);
    // nt->addItem ("BsPt", BsPt);
    // nt->addItem ("BsChi2", BsChi2);
    // nt->addItem ("DsP", DsP);
    // nt->addItem ("DsPt", DsPt);
    // nt->addItem ("DsChi2", DsChi2);
   }
  else {
  fatal() << " failed to book Ntuple" << endreq;
  return StatusCode::FAILURE;
  }
  }
  debug() << " NTuple has been produced" << endreq;
    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TaggingTuplesSignal::execute() {

  // Onuora begin

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  nSelected=0;
  /* */

 const Particle::ConstVector& parts = desktop()->particles();
 if(parts.empty()){
    debug() << "No B candidates" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  const MCParticle* B0 = m_forcedBtool->forcedB();
  MCParticle::Vector B0daughters(0);
  
  //-------------------
  int truetag = B0->particleID().pid()>0 ? 1 : -1;
 
  RunNu = 0;
  EvtNu = 0;
  Bsmass = 0;
  Bsid = 0;
  BsidTag = 0;
  MCBsid = 0;
  MCBsmass = 0;
  Muid = 0;
  Mumass = 0;
  Dsid = 0;
  Dsmass = 0;
  trig0 = 0;
  hlt0 = 0;
 
  //BsP = 0.;
  //BsPt = 0.;
  //BsChi2= 0.;
  //DsP = 0.;
  //DsPt = 0.;
  //DsChi2= 0.;

  osmuondec=0;
   oselectrondec=0;
   oskaondec=0;
   sskaondec=0;
   vtxchargedec=0;
   sspiondec = 0;
   ssjetdec = 0;
   
   L0DUReport* trg = 0;
   if ( exist<L0DUReport>(L0DUReportLocation::Default)){
     trg = get<L0DUReport> (L0DUReportLocation::Default);
   } else {
     debug() << "no L0 trigger"<<endreq;
   }
   if(NULL!=trg) {
     trig0 = trg->decision() ;
     info() << "====L0===="<< trig0 <<endreq;
   }
   
   HltSummary* hlt = 0;
   if ( exist<HltSummary>(HltSummaryLocation::Default)){
     hlt = get<HltSummary> (HltSummaryLocation::Default);
   } else {
     debug() << "no hlt trigger"<<endreq;
   }
   
   if(NULL!=hlt) {
       hlt0 = hlt->decision() ; 
       info() << "====Hlt===="<< hlt0 <<endreq;
     }

  FlavourTags*  tags = new FlavourTags;

// about MCParticle
  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );
  if ( ! mcpart ) {
    error() << "No MCParticles retrieved" << endreq;
    return StatusCode::FAILURE;
  }
  debug() << "Nr of MCParticles retrieved="<< mcpart->size()
          << endreq;

// end about MCparticle

  for(Particle::ConstVector::const_iterator icandB = parts.begin(); 
      icandB != parts.end(); ++icandB) {

// Getting info about Ds & Muons

    int pid = (*icandB)->particleID().pid();
    debug() << " Checking How many PIDs = " 
          << (*icandB)->particleID().pid() << endreq;

// Binning them

    if (abs(pid)==431){
      debug() << " We have Ds : " 
              << (*icandB)->particleID().pid() << endreq;
      Dsid  = (*icandB)->particleID().pid();
      Dsmass = (*icandB)->measuredMass(); 
      // DsP = (*icandB)-> p();
      // DsPt = (*icandB)-> pt();
      // DsChi2= (*icandB)->endVertex()->chi2();
     }

// Now retrieving Info about Bs

   if((*icandB)->particleID().hasBottom()) {
      debug() << "Running tagging on candidate with PID = " 
              << (*icandB)->particleID() << endreq;
      debug() << "About to tag candidate B of mass="
              <<(*icandB)->measuredMass() <<endreq;
      debug() << "Number of Input Particles="
              <<parts.size() <<endreq;


      Bsmass = (*icandB)->measuredMass();
      Bsid   = (*icandB)->particleID().pid();
      BsidTag = (*icandB)->particleID().pid()>0 ? 1 : -1;

      // BsP = (*icandB)-> p();
      // BsPt = (*icandB)-> pt();
      // BsChi2= (*icandB)->endVertex()->chi2();

// Get PrintTree info

      debug() << "Bsig decay tree  "<< endreq;
      m_debugParticle -> printTree(*icandB);

// End PrintTree
// getting MCPrintTree Stuff

      if(Bsmass !=0){
        SmartDataPtr<MCParticles> mcpart (eventSvc(),
                                         MCParticleLocation::Default );
        if ( ! mcpart ) {
          error() << "No MCParticles retrieved" << endreq;
          return StatusCode::FAILURE;
        }
        MCParticles::const_iterator imc;
        for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ){
          if( (*imc)->particleID().hasBottom()){
              debug() << "MC Bsig decay tree  "<< endreq;
              m_debugMCParticle -> printTree(*imc);
              MCBsid= (*imc)->particleID().pid();
              MCBsmass= (*imc)->virtualMass();
              info() << " MC PARTICLE INFO "
                     << std::setw(5) << (*imc)->particleID().pid()
                     << std::setw(8) << (*imc)->virtualMass()
                     << endreq;
          }
        }
      }
      
//end MCPrintTree

  // Retrieve informations about event
  RecHeader* evt = get<RecHeader>(RecHeaderLocation::Default);
  
  if(evt){
    debug() << "Retrieved EVENT: " << evt->evtNumber() << endreq;
    EvtNu = evt->evtNumber();
    RunNu = evt->runNumber();
      
  } else{
    fatal() << "Not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }

      FlavourTag* theTag = new FlavourTag;
      //--------------------- TAG IT ---------------------
      //use tool for tagging by just specifing the signal B
      StatusCode sc = flavourTagging() -> tag( *theTag, *icandB );
      if (!sc) {
        err() <<"Tagging Tool returned error."<< endreq;
        delete theTag;
      }
      else tags->insert(theTag);
      int tagdecision = theTag->decision();
      debug() << "Flavour guessed: " << (tagdecision>0 ? "b":"bbar")
              <<endreq;
      

      ///print Taggers information
      std::vector<Tagger> mytaggers = theTag->taggers();
      std::vector<Tagger>::iterator itag;
      for(itag=mytaggers.begin(); itag!=mytaggers.end(); ++itag) {
        std::string tts;
        switch ( itag->type() ) {
        case Tagger::none        : tts="none";        break;
        case Tagger::unknown     : tts="unknown";     break;
        case Tagger::OS_Muon     : tts="OS_Muon";     break;
        case Tagger::OS_Electron : tts="OS_Electron"; break;
        case Tagger::OS_Kaon     : tts="OS_Kaon";     break;
        case Tagger::SS_Kaon     : tts="SS_Kaon";     break;
        case Tagger::SS_Pion     : tts="SS_Pion";     break;
        case Tagger::jetCharge   : tts="jetCharge";   break;
        case Tagger::OS_jetCharge: tts="OS_jetCharge";break;
        case Tagger::SS_jetCharge: tts="SS_jetCharge";break;
        case Tagger::VtxCharge   : tts="VtxCharge";   break;
        case Tagger::Topology    : tts="Topology";    break;
        }
        debug() << "--> tagger type: " << tts <<endreq;
        debug() << "    decision = "
                << (itag->decision() > 0? "b":"bbar") <<endreq;
        debug() << "    omega    = " << itag->omega() <<endreq;
        std::vector<Particle> taggerparts = itag->taggerParts();
        std::vector<Particle>::iterator kp;
        for(kp=taggerparts.begin(); kp!=taggerparts.end(); kp++) {
            debug() << "    ID:" <<std::setw(4)<< kp->particleID().pid() 
                    << " p= "  << kp->p()<< endreq;///GeV << endreq;
	}
          if(tts=="OS_Muon"){
          osmuondec=itag->decision();
          debug() << "--> OSMuondec: " << osmuondec <<endreq;
        }
        if(tts=="OS_Electron"){
         oselectrondec=itag->decision();
         debug() << "--> OSElectrondec: " << oselectrondec <<endreq;
        }
        if(tts=="OS_Kaon"){
          oskaondec=itag->decision();
          debug() << "--> OSKaondec: " << oskaondec <<endreq;
        }
        if(tts=="SS_Kaon"){
         sskaondec=itag->decision();
         debug() << "--> SSKaondec: " << sskaondec <<endreq;
        }
        
        if(tts=="SS_Pion"){
          sspiondec=itag->decision();
          debug() << "--> SSPiondec: " << sspiondec <<endreq;
        }
        if(tts=="SS_jetCharge"){
          ssjetdec=itag->decision();
          debug() << "--> SSjetChargedec: " << ssjetdec <<endreq;       
          }
        if(tts=="VtxCharge"){
          vtxchargedec=itag->decision();
          debug() << "--> VtxChargedec " <<vtxchargedec  <<endreq;
        }
      }        
    
    nSelected++;
    Event=nSelected;
    Truetag=truetag;
    debug() << "taggers decision" << osmuondec<<oselectrondec<<oskaondec<<
      sskaondec<<vtxchargedec << endmsg;
        info() << "Taggers Decision "
               << std::setw(3) << truetag
               << std::setw(3) << osmuondec
               << std::setw(3) << oselectrondec
               << std::setw(3) << oskaondec
               << std::setw(3) << sskaondec
               << std::setw(3) << vtxchargedec
               << std::setw(3) << sspiondec
               << std::setw(3) <<  ssjetdec
               << endreq;
   }
   
  }
  
  //tuple writing

    if(Event>0) {
  NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/200");
  StatusCode sc = nt->write();
  if ( !sc )
    err() << "Cannot fill ntuple" << endreq;
  else verbose() << " Wrote ntuple "<<endreq;
  }
  

// Onuora end


  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

 



//=============================================================================
//  Finalize
//=============================================================================
StatusCode TaggingTuplesSignal::finalize() {
  debug() << "==> Finalize" << endmsg;
  return  StatusCode::SUCCESS;
}

//=============================================================================
MCParticle* TaggingTuplesSignal::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> ( MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
	if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
	  return (*imc);
	}
      }
    }
  }
  return 0;
}
