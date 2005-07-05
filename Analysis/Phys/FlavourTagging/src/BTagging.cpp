// Include files 
#include "BTagging.h"

//-----------------------------------------------------------------------
// Implementation file for class : BTagging
//
// Author: Marco Musy
//-----------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BTagging>          s_factory ;
const        IAlgFactory& BTaggingFactory = s_factory ; 

//=======================================================================
BTagging::BTagging(const std::string& name,
                   ISvcLocator* pSvcLocator):
  DVAlgorithm(name, pSvcLocator){
  
  declareProperty( "WriteToTES", m_WriteToTES = true );
  declareProperty( "TagOutputLocation", m_TagLocation = FlavourTagLocation::Default );
}
BTagging::~BTagging() {}; 

//=======================================================================
StatusCode BTagging::initialize() {
  
  return StatusCode::SUCCESS; 
}

//=======================================================================
// Main execution
//=======================================================================
StatusCode BTagging::execute() {

  setFilterPassed( false );
  debug() << "Entering BTagging" <<endreq;

  //look in location where Selection has put the B candidates
  ParticleVector parts = desktop()->particles();
  if( parts.empty()  ) return StatusCode::SUCCESS;

  //-------------- loop on signal B candidates from selection
  FlavourTag theTag;
  ParticleVector::const_iterator icandB;
  for ( icandB = parts.begin(); icandB != parts.end(); icandB++){
    if((*icandB)->particleID().hasBottom()) {
      debug() << "About to tag candidate B of mass=" 
              << (*icandB)->momentum().mag()/GeV <<endreq;

      //--------------------- TAG THEM -------------------
      //use tool for tagging by just specifing the signal B
      theTag = flavourTagging() -> tag( *icandB );

      //use tool for tagging if you want to specify the Primary Vtx
      //theTag = flavourTagging() -> tag( *icandB, PVertex );

      //use tool for tagging if you want to specify a list of particles
      //theTag = flavourTagging() -> tag( *icandB, PVertex, vtags );
      //--------------------------------------------------

      //--- PRINTOUTS ---
      //print the information in theTag
      int tagdecision = theTag.decision();
      debug() << "Flavour guessed: " << (tagdecision>0 ? "b":"bbar")<<endreq;
      debug() << "estimated omega= " << theTag.omega() <<endreq;
      Particle* tagB = theTag.taggedB();
      if( tagB ) debug() << "taggedB p="<< tagB->p()/GeV <<endreq;

      ///print Taggers information
      std::vector<Tagger> mytaggers = theTag.taggers();
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
                  << " p= "  << kp->p()/GeV << endreq;
        }
      }
    }
  }

  //-------------------------------------------
  ///Output to TES (for backward compatibility) 
  if( m_WriteToTES ) if( theTag.decision() ) {
    FlavourTags* tags = new FlavourTags;
    tags->insert(&theTag);
    StatusCode sc = put(tags,m_TagLocation);
    if( !sc ) err() <<"Unable to register tags"<< endreq;
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//=========================================================================
StatusCode BTagging::finalize() { return StatusCode::SUCCESS; }

//==========================================================================
