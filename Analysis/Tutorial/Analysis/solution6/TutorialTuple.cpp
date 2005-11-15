// $Id: TutorialTuple.cpp,v 1.2 2005-11-15 08:26:43 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCParticle.h"
#include "Event/TrgDecision.h"
// local
#include "TutorialTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialTuple
//
// 2005-11-07 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TutorialTuple>          s_factory ;
const        IAlgFactory& TutorialTupleFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialTuple::TutorialTuple( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_pAsct()
{
  declareProperty( "FillTruth", m_truth = true );
  declareProperty( "InputContainers", m_containers = std::vector<std::string>() );

}
//=============================================================================
// Destructor
//=============================================================================
TutorialTuple::~TutorialTuple() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialTuple::initialize() {

  debug() << "==> Initialize" << endmsg;

  if (m_truth) {
    info() << "Will be filling MC truth information" << endmsg ;
    m_pAsct = new Particle2MCLink(this, Particle2MCMethod::Composite,m_containers);
  }


  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialTuple::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  Tuple tuple = nTuple("TutorialTuple");
  ParticleVector Bcands = desktop()->particles();
  for ( ParticleVector::const_iterator b = Bcands.begin(); 
        b != Bcands.end() ; ++b ){
    if (!fillBs(tuple,*b)) return StatusCode::FAILURE;
    if (!fillTrigger(tuple)) return StatusCode::FAILURE;
    if (!fillTagging(tuple,*b)) return StatusCode::FAILURE;
    if (m_truth) if (!fillTruth(tuple,*b)) return StatusCode::FAILURE;
    tuple->write();
  }

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
};
//=============================================================================
// Truth
//=============================================================================
StatusCode TutorialTuple::fillTruth(Tuple& tuple,const Particle* b) {
  
  const MCParticle* MC = m_pAsct->firstMCP( b );
  if ( 0!=MC ){
    tuple->column( "TPid", MC->particleID().pid());
    tuple->column( "TP",   MC->momentum());
    info() << "Bs associated with " << MC->particleID().pid() 
            << " " <<  MC->momentum() << endmsg ;
  } else {
    tuple->column( "TPid", -1);
    tuple->column( "TP",   HepLorentzVector(-1.,-1.,-1.,-1.));
    info() << "No association for Bs " << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Tagging result
//=============================================================================
StatusCode TutorialTuple::fillTagging(Tuple& tuple,const Particle* b) {
  
  FlavourTag tag;
  flavourTagging()->tag(tag,b);
  tuple->column("Tag",        tag.decision());
  tuple->column("TagCategory",tag.category());
  debug() << "Bs is tagged as " << tag.decision() << " with category " 
          << tag.category() << endmsg ;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Trigger
//=============================================================================
StatusCode TutorialTuple::fillTrigger(Tuple& tuple) {
  
  if ( exist<TrgDecision>(TrgDecisionLocation::Default)){
    TrgDecision* trg = get<TrgDecision> (TrgDecisionLocation::Default);
    tuple->column ( "L0", trg->L0() ) ;
    tuple->column ( "L1", trg->L1() ) ;
  } else{
    tuple->column ( "L0", false ) ;
    tuple->column ( "L1", false ) ;
  }    
  // One could also access L1Score and HltScore
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Bs properties
//=============================================================================
StatusCode TutorialTuple::fillBs(Tuple& tuple,const Particle* b) {
  // store mass
  tuple->column( "Mass", b->mass()); // mass
  // store momentum
  tuple->column( "P", b->momentum()); // 4-vector!
  // store IPs
  VertexVector PVs = desktop()->primaryVertices();
  if (PVs.size()>20) {
    err() << "Cannot store more than 20 PVs. Resize ntuple" << endmsg;
    return StatusCode::FAILURE ;
  }
  std::vector<double> ips, ipes;
  for ( VertexVector::const_iterator ipv = PVs.begin() ; ipv != PVs.end() ; ++ipv ){
    double ip, ipe;
    geomDispCalculator()->calcImpactPar(*b, *(*ipv), ip, ipe);
    ips.push_back(ip);
    ipes.push_back(ipe);
  }  
  tuple->farray( "IP",  ips.begin(), ips.end(), "PVs", 20);
  tuple->farray( "IPe", ipes.begin(), ipes.end(), "PVs", 20);
  // One could think about goig down to the daughters...

  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
