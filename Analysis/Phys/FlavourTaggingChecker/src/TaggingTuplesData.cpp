// $Id: TaggingTuplesData.cpp,v 1.3 2008-07-24 08:21:09 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Event Model
#include "Event/FlavourTag.h"
#include "Event/L0DUReport.h"
#include "Event/MCParticle.h"
#include "Event/RecHeader.h"

// local
#include "TaggingTuplesData.h"

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( TaggingTuplesData );

using namespace LHCb ;
using namespace Gaudi;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TaggingTuplesData::TaggingTuplesData( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  
}
//=============================================================================
// Destructor
//=============================================================================
TaggingTuplesData::~TaggingTuplesData() {}; 
//=============================================================================
// Initialization
//=============================================================================
StatusCode TaggingTuplesData::initialize() {
  
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

  m_back = tool<IBackgroundCategory>("BackgroundCategory",this);
  if(!m_back){
    fatal() << "Unable to retrieve BackgroundCategory tool" << endreq;
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
    //  nt->addItem ("Truetag",   Truetag);
    nt->addItem ("EvtNum", EvtNu);
    nt->addItem ("RunNum", RunNu);
    nt->addItem ("Events", Event);
    nt->addItem ("BsMass", Bsmass);
    nt->addItem ("BsID", Bsid);
    nt->addItem ("BsIDTag", BsidTag);
    nt->addItem ("MCBsID", MCBsid);
    nt->addItem ("MCBsMass", MCBsmass);
    nt->addItem ("DsMass", Dsmass);
    nt->addItem ("DsID", Dsid);
    nt->addItem ("L0",trig0);
    nt->addItem ("HLT", trigHLT);
    nt->addItem ("BackgroundCat",m_background);
    nt->addItem ("BkgtoolTag",  m_bkgtooltag );
    nt->addItem ("BkgtoolTagID",  m_bkgtooltagid );
    nt->addItem ("BkgtoolIDmass",  bkgtoolIDmass );
    // nt->addItem ("BsP", BsP);
    //nt->addItem ("BsPt", BsPt);
    //nt->addItem ("BsChi2", BsChi2);
    //nt->addItem ("DsP", DsP);
    //nt->addItem ("DsPt", DsPt);
    //nt->addItem ("DsChi2", DsChi2);
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
StatusCode TaggingTuplesData::execute() {

  // Onuora begin

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  nSelected=0;
  /* */

  // const ParticleVector& parts = desktop()->particles();
 const Particle::ConstVector& parts = desktop()->particles();
 if(parts.empty()){
    debug() << "No B candidates" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

 // initializing Variables
  RunNu = 0;
  EvtNu = 0;
  Bsmass = 0;
  Bsid = 0;
  BsidTag= 0;
  MCBsid = 0;
  MCBsmass = 0;
  Dsid = 0;
  Dsmass = 0;
  trig0 = 0;
  trigHLT = 0;
  m_background =0;
  m_bkgtooltagid = 0;
  m_bkgtooltag = 0;
  bkgtoolIDmass = 0;

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
    debug() << "no trigger"<<endreq;
  }

  if(NULL!=trg) {
    trig0 = trg->decision() ;
    }

  FlavourTags*  tags = new FlavourTags;

  MCParticles* mcpart = get<MCParticles>( MCParticleLocation::Default );
  debug() << "Nr of MCParticles retrieved="<< mcpart->size() << endmsg;


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

      Bsmass = (*icandB)-> measuredMass();
      Bsid   = (*icandB)->particleID().pid();
      BsidTag = (*icandB)->particleID().pid()>0 ? 1 : -1;

      // BsP = (*icandB)-> p();
      // BsPt = (*icandB)-> pt();
      // BsChi2= (*icandB)->endVertex()->chi2();

// Get PrintTree info
      debug() << "Bsig decay tree  "<< endreq;
      m_debugParticle -> printTree(*icandB);

// End PrintTree

// getting MCPrintTree info

      if(Bsmass !=0){
        MCParticles* mcpart = get<MCParticles>( MCParticleLocation::Default );
        MCParticles::const_iterator imc;
        for ( imc = mcpart->begin(); imc != mcpart->end(); imc++ ){
          if( (*imc)->particleID().hasBottom()){
              debug() << "MC Bsig decay tree  "<< endreq;
              m_debugMCParticle -> printTree(*imc);
              MCBsid= (*imc)->particleID().pid();
              MCBsmass= (*imc)->virtualMass();
              info() << "TEST MC PARTICLE INFO "
                     << std::setw(5) << (*imc)->particleID().pid()
                     << std::setw(8) << (*imc)->virtualMass()
                     << endreq;
          }
        }
      }
      
//end MCPrintTree info

// Now retrieving BackgroundCategory info:

      if(m_back->category(*icandB)!= 0){
        m_background = m_back->category(*icandB);
        debug() << "BackGround Category="
                << m_back->category(*icandB) <<endreq;
        info() << " BKGTOOL MCPARTICLE INFO: Category => "
               << std::setw(8) <<  m_back->category(*icandB)
               << endreq;
        }
      if(m_back->origin(*icandB)!= 0){
        m_bkgtooltagid = ( m_back->origin(*icandB))->particleID().pid();
        m_bkgtooltag = ( m_back->origin(*icandB))->particleID().pid()>0 ? 1 : -1;
        debug() << "BackGround Category MC PID="
                << ( m_back->origin(*icandB))->particleID().pid() <<endreq;
        info() << " BKGTOOL MCPARTICLE INFO: ID , Mass => "
               << std::setw(5) << ( m_back->origin(*icandB))->particleID().pid()
               << std::setw(8) << ( m_back->origin(*icandB))->virtualMass()
               << endreq;
        bkgtoolIDmass= ( m_back->origin(*icandB))->virtualMass();
      }
      
// end retrieving BackgroundCategory :

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


// Retrieve Tagging information

      FlavourTag* theTag = new FlavourTag;

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
        switch ( itag->type() ){
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
                           << " p= "  << kp->p()/Gaudi::Units::GeV << endmsg;
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
      debug() << "taggers decision" << osmuondec<<oselectrondec<<oskaondec<<
      sskaondec<<vtxchargedec << endmsg;
        info() << "Taggers Decision "
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
StatusCode TaggingTuplesData::finalize() {
  debug() << "==> Finalize" << endmsg;
  return  StatusCode::SUCCESS;
}

//=============================================================================


