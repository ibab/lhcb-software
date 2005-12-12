// $Id: TutorialTuple.cpp,v 1.3 2005-12-12 12:19:13 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrgDecision.h"
#include "Event/HltScore.h"
#include "Event/L1Score.h"

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
    tuple->column ( "HLT",trg->HLT() ) ;
  } else{
    tuple->column ( "L0", false ) ;
    tuple->column ( "L1", false ) ;
    tuple->column ( "HLT", false );
  }    

  // L1 score
  L1Score* scorel1 = NULL;
  if (exist<L1Score>(L1ScoreLocation::Default) )
  {
     scorel1 = get<L1Score> (L1ScoreLocation::Default);
     if ( NULL == scorel1 )
     {
       err() << "Null L1Score at " << L1ScoreLocation::Default << endmsg ;
     }
     else
     {
      tuple->column ( "L1GEN", scorel1->decisionGen() );
      tuple->column ( "L1MU", scorel1->decisionMu() );
      tuple->column ( "L1JPSI", scorel1->decisionDiMuJPsi() );
      tuple->column ( "L1DIMU", scorel1->decisionDiMu() );
      tuple->column ( "L1PHOT", scorel1->decisionPhot() );
      tuple->column ( "L1ELEC", scorel1->decisionElec() );
     }
  } 
  else
  {
     Warning("No L1Score at "+ L1ScoreLocation::Default);
  }
  if ( NULL == scorel1 )
  {
    tuple->column ( "L1GEN", false ); 
    tuple->column ( "L1MU", false );
    tuple->column ( "L1JPSI", false );
    tuple->column ( "L1DIMU", false );
    tuple->column ( "L1PHOT", false );
    tuple->column ( "L1ELEC", false );
  }



  // HLT score
  HltScore* score = NULL;
  if (exist<HltScore>(HltScoreLocation::Default) )
  {
     score = get<HltScore> (HltScoreLocation::Default);
     if ( NULL == score )
     {
       err() << "Null HLTScore at " << HltScoreLocation::Default << endmsg ;
     }
     else
     {
      tuple->column ( "HLTGEN", score->decisionGen() );
      tuple->column ( "HLTINC", score->decisionInclusiveB() );
      tuple->column ( "HLTDIM", score->decisionDimuon() );
      tuple->column ( "HLTDST", score->decisionDstar() );
      tuple->column ( "HLTEXC", score->decisionExclusive() );
     }
  } 
  else
  {
     Warning("No HLTScore at "+ HltScoreLocation::Default);
  }
  if ( NULL == score )
  {
    tuple->column ( "HLTGEN", false ); 
    tuple->column ( "HLTINC", false );
    tuple->column ( "HLTDIM", false );
    tuple->column ( "HLTDST", false );
    tuple->column ( "HLTEXC", false );
  }

  
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
