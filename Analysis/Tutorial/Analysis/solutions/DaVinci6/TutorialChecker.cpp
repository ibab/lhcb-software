// $Id: TutorialChecker.cpp,v 1.2 2007-01-05 17:08:47 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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
  
  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Composite, ""); 

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
  Tuple tuple = nTuple("TutorialTuple");
  LHCb::Particle::ConstVector Bcands = desktop()->particles();
  for ( LHCb::Particle::ConstVector::const_iterator b = Bcands.begin(); 
        b != Bcands.end() ; ++b){
    sc = fillReco(tuple,*b);
    if (sc) sc = fillTruth(tuple,*b);
    if (sc) sc = fillTagging(tuple,*b);
    
    if (!sc) return sc ;
  }  
  sc = tuple->write();

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  if (NULL!=m_pLinker) delete m_pLinker ;  

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
  tuple->column( "PX", b->momentum().x());
  tuple->column( "PY", b->momentum().y());
  tuple->column( "PZ", b->momentum().z());
  tuple->column( "E", b->momentum().e());
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
StatusCode TutorialChecker::fillTruth(Tuple& tuple,const LHCb::Particle* b) {
  
  debug() << "==> fillTruth" << endmsg;
  
  IBackgroundCategory::categories cat = m_background->category(b);
  tuple->fill("category",cat);
  
  const LHCb::MCParticle* MC = m_pLinker->firstMCP( b );
  
  if ( NULL!=MC ){
    //    tuple->column( "TP", MC->momentum() ); /// @todo restore
    tuple->column( "TPX", MC->momentum().x());
    tuple->column( "TPY", MC->momentum().y());
    tuple->column( "TPZ", MC->momentum().z());
    tuple->column( "TPE", MC->momentum().e());
    tuple->column( "TPt", MC->pt());
    tuple->column( "TPID", MC->particleID().pid() );
  }
  
  // some looping in debug mode
  // that's the excerpt from the talk
  if ( msgLevel( MSG::DEBUG )  ){
    if ( m_pLinker->isAssociated( b ) )
      debug() << "Particle is associated" << endmsg;
    else debug() << "Particle is not associated" << endmsg;
    const LHCb::MCParticle *MCpart = m_pLinker->firstMCP( b );
    while ( NULL!=MCpart ){
      debug() << "Particle " << b->key() << " " << b->particleID().pid() 
              << " associated to MC part " << MCpart->key() << " " 
              << MCpart->particleID().pid() << endmsg;
      MCpart = m_pLinker->next();
    }
    
    if ( NULL!=MC ){
      const LHCb::Particle *Part = m_pLinker->firstP( MC );
      if ( NULL==Part ) debug() << "No association for MC Particle " 
                                << MC->key() << endmsg ;
      while ( NULL!=Part ){
        debug() << "MC Particle " << MC->key() << " " << MC->particleID().pid()
                << " associated to Particle " << Part->key() << " " 
                << Part->particleID().pid()<< endmsg;
        Part = m_pLinker->nextP();
      }
    }
  }
       
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Truth
//=============================================================================
StatusCode TutorialChecker::fillTagging(Tuple& tuple,const LHCb::Particle* b) {

  debug() << "==> fillTagging" << endmsg;
  
  LHCb::FlavourTag tag;
  StatusCode sc = flavourTagging()->tag(tag,b);
  tuple->column("Tag",tag.decision());

  return sc ;
}

