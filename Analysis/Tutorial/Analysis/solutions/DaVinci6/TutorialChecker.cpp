// $Id: TutorialChecker.cpp,v 1.10 2008-04-24 12:46:16 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from LHCb
#include "Event/HltSummary.h"
#include "Event/HltEnums.h"
#include "Event/RecHeader.h"
#include "Event/L0DUReport.h"

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
  std::vector<double> ips, ipes;
  for ( LHCb::RecVertex::ConstVector::const_iterator ipv = PVs.begin() ; 
        ipv != PVs.end() ; ++ipv ){
    double ip, ipe;
    geomDispCalculator()->calcImpactPar(*b, *(*ipv), ip, ipe);
    ips.push_back(ip);
    ipes.push_back(ipe);
  }  
  tuple->farray( "IP",  ips.begin(), ips.end(), "PVs", 20);
  tuple->farray( "IPe", ipes.begin(), ipes.end(), "PVs", 20);
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
  const LHCb::HltSummary* hlt = get<LHCb::HltSummary>(LHCb::HltSummaryLocation::Default);  
  LHCb::L0DUReport* l0  = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default); // not const beacuse of bug in L0Decision
  debug() << "L0 decision is " << l0->decision() << ", HLT: " << hlt->decision() << endmsg ;
  tuple->column("L0",           l0->decision()); // total decision
  tuple->column("Hlt",          hlt->decision()); // total decision
  tuple->column("HltMuon",      hlt->checkDecisionType(LHCb::HltEnums::Muon));  
  tuple->column("HltDiMuon",    hlt->checkDecisionType(LHCb::HltEnums::DiMuon));
  tuple->column("HltJpsi",      hlt->checkDecisionType(LHCb::HltEnums::JPsi));
  tuple->column("HltMuonHadron",hlt->checkDecisionType(LHCb::HltEnums::MuonHadron));
  tuple->column("HltHadron",    hlt->checkDecisionType(LHCb::HltEnums::Hadron));
  tuple->column("HltDiHadron",  hlt->checkDecisionType(LHCb::HltEnums::DiHadron));
  tuple->column("HltElectron",  hlt->checkDecisionType(LHCb::HltEnums::Electron));
  tuple->column("HltDiElectron",hlt->checkDecisionType(LHCb::HltEnums::DiElectron));
  tuple->column("HltGamma",     hlt->checkDecisionType(LHCb::HltEnums::Gamma));

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
