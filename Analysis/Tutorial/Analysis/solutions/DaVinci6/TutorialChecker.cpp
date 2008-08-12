// $Id: TutorialChecker.cpp,v 1.14 2008-08-12 13:07:16 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from LHCb
#include "Event/HltSummary.h"
#include "Event/RecHeader.h"
#include "Event/L0DUReport.h"
#include "Kernel/IHltSummaryTool.h"
#include "Kernel/IANNSvc.h"

// local
#include "TutorialChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialChecker
//
// 2006-12-21 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TutorialChecker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialChecker::TutorialChecker( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_background()
    , m_summary()
{
}
//=============================================================================
// Destructor
//=============================================================================
TutorialChecker::~TutorialChecker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialChecker::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  m_background = tool<IBackgroundCategory>("BackgroundCategory",this);
  m_summary = tool<IHltSummaryTool>("HltSummaryTool");

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialChecker::execute() {

  debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  // code goes here  
  Particle2MCLinker Linker(this,Particle2MCMethod::Composite,std::vector<std::string>(1,"") ); 
  Tuple tuple = nTuple("TutorialTuple");
  LHCb::Particle::ConstVector Bcands = desktop()->particles();
  for ( LHCb::Particle::ConstVector::const_iterator b = Bcands.begin(); 
        b != Bcands.end() ; ++b){
    sc = fillHeader(tuple); // header filled for every B, not every event!
    if (sc) sc = fillReco(tuple,*b);
    if (sc) sc = fillTruth(tuple,*b, Linker);
    if (sc) sc = fillTagging(tuple,*b);
    if (sc) sc = fillTrigger(tuple);
    
    sc = tuple->write();
    if (!sc) return sc ;
  }  

  setFilterPassed(true) ;   // Mandatory. Set to true if event is accepted.
  return sc ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
//  Reco 
//=============================================================================
StatusCode TutorialChecker::fillReco(Tuple& tuple,const LHCb::Particle* b) {

  debug() << "==> fillReco" << endmsg;
  
  // store mass
  tuple->column( "Mass", b->measuredMass()); // mass
  // store momentum. This will work with the next version of Gaudi:
  //  tuple->column( "P", b->momentum()); // 4-vector!
  // in the meantime I do:
  tuple->column( "P", b->momentum());
  // store IPs
  LHCb::RecVertex::ConstVector PVs = desktop()->primaryVertices();
  if (PVs.size()>20) {
    err() << "Cannot store more than 20 PVs. Resize ntuple" << endmsg;
    return StatusCode::FAILURE ;
  }
  std::vector<double> ips, ipchi2s;
  for ( LHCb::RecVertex::ConstVector::const_iterator ipv = PVs.begin() ; 
        ipv != PVs.end() ; ++ipv ){
    double ip, ipchi2;
    distanceCalculator()->distance(b, (*ipv), ip, ipchi2);
    ips.push_back(ip);
    ipchi2s.push_back(ipchi2);
  }  
  tuple->farray( "IP",  ips.begin(), ips.end(), "PVs", 20);
  tuple->farray( "IPchi2", ipchi2s.begin(), ipchi2s.end(), "PVs", 20);
  // One could think about going down to the daughters...

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Truth
//=============================================================================
StatusCode TutorialChecker::fillTruth(Tuple& tuple,const LHCb::Particle* b, Particle2MCLinker& Link) {
  
  debug() << "==> fillTruth" << endmsg;
  
  IBackgroundCategory::categories cat = m_background->category(b);
  tuple->fill("category",(int)cat);
  
  const LHCb::MCParticle* MC = Link.firstMCP( b );
  
  if ( NULL!=MC ){
    tuple->column( "TP", MC->momentum());
    tuple->column( "TPt", MC->pt());
    tuple->column( "TPID", MC->particleID().pid() );
  } else { // all columns should be filled even if there's nothing found
    tuple->column( "TP", Gaudi::XYZTVector(-999.,-999.,-999.,-999.));
    tuple->column( "TPt", -999.);
    tuple->column( "TPID", -1);    
  }
  
  // some looping in debug mode
  // that's the excerpt from the talk
  if ( msgLevel( MSG::DEBUG )  ){
    if ( Link.isAssociated( b ) )
      debug() << "Particle is associated" << endmsg;
    else debug() << "Particle is not associated" << endmsg;
    const LHCb::MCParticle *MCpart = Link.firstMCP( b );
    while ( NULL!=MCpart ){
      debug() << "Particle " << b->key() << " " << b->particleID().pid() 
              << " associated to MC part " << MCpart->key() << " " 
              << MCpart->particleID().pid() << endmsg;
      MCpart = Link.next();
    }
    
    if ( NULL!=MC ){
      const LHCb::Particle *Part = Link.firstP( MC );
      if ( NULL==Part ) debug() << "No association for MC Particle " 
                                << MC->key() << endmsg ;
      while ( NULL!=Part ){
        debug() << "MC Particle " << MC->key() << " " << MC->particleID().pid()
                << " associated to Particle " << Part->key() << " " 
                << Part->particleID().pid()<< endmsg;
        Part = Link.nextP();
      }
    }
  }
       
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Tagging
//=============================================================================
StatusCode TutorialChecker::fillTagging(Tuple& tuple,const LHCb::Particle* b) {

  debug() << "==> fillTagging" << endmsg;
  
  LHCb::FlavourTag tag;
  StatusCode sc = flavourTagging()->tag(tag,b);
  tuple->column("Tag",tag.decision());

  return sc ;
}
//============================================================================
//  Trigger
//============================================================================
StatusCode TutorialChecker::fillTrigger(Tuple& tuple){
  debug() << "==> fillTrigger" << endmsg;
  LHCb::L0DUReport* l0  = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default); // not const beacuse of bug in L0Decision
  debug() << "L0 decision is " << l0->decision() << ", HLT: " << m_summary->decision() << endmsg ;
  tuple->column("L0",           l0->decision()); // total decision
  tuple->column("Hlt",          m_summary->decision()); // total decision

  std::string major = "Hlt2SelectionID" ; // but it could be Hlt1...

  std::vector<std::string> names = svc<IANNSvc>("HltANNSvc")->keys(major);
  for (std::vector<std::string>::const_iterator s = names.begin() ; s!=names.end() ; ++s){
    if ( ! tuple->column(*s, m_summary->selectionDecision(*s) ) ) 
      return StatusCode::FAILURE;
    if (msgLevel(MSG::VERBOSE)) verbose() << major << " :: " << *s << " says " 
                                          << m_summary->selectionDecision(*s) << endmsg ; 
  }
  return StatusCode::SUCCESS ;
}
//============================================================================
//  Event and run number 
//============================================================================
StatusCode TutorialChecker::fillHeader(Tuple& tuple){
  const LHCb::RecHeader* header = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);  
  info() << "Filling Tuple at Run " << header->runNumber() << ", Event " << header->evtNumber() << endmsg ;
  tuple->column("Event", (int)header->evtNumber());
  tuple->column("Run", (int)header->runNumber());
  return StatusCode::SUCCESS ;
}
