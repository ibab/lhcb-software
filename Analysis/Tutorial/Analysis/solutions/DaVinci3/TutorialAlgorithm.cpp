// $Id: TutorialAlgorithm.cpp,v 1.5 2006-12-07 12:19:14 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "TutorialAlgorithm.h"

using namespace Gaudi::Units ;

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialAlgorithm
//
// 2006-05-24 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TutorialAlgorithm );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialAlgorithm::TutorialAlgorithm( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_motherID(0)
    , m_motherMass(0.)
    , m_nMothers(0)
    , m_nEvents(0)
{
  declareProperty("Particle",   m_motherName );
  declareProperty("MassWindow", m_motherMassWin = 10.*GeV);
  declareProperty("MaxChi2",    m_motherChi2 = 1000.);
}
//=============================================================================
// Destructor
//=============================================================================
TutorialAlgorithm::~TutorialAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialAlgorithm::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  ParticleProperty* mother = ppSvc()->find( m_motherName );
  if ( !mother ) { //
    err() << "Cannot find particle property for " << m_motherName << endmsg ;
    return StatusCode::FAILURE;
  }
  m_motherID = LHCb::ParticleID(mother->pdgID());
  m_motherMass = mother->mass();
  info() << "Will reconstruct " << mother->particle() << " (ID=" << m_motherID.pid()
         << ") with mass " << m_motherMass << endmsg ;
  info() << "Mass window is " << m_motherMassWin << " MeV" << endmsg ;
  info() << "Max chi^2 is " << m_motherChi2 << endmsg ;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++m_nEvents;

  // code goes here  
  LHCb::Particle::ConstVector daughters = desktop()->particles(); // get particles
  sc = loopOnDaughters(daughters);
  if (!sc) return sc;
  sc = makeMother(daughters);
  if (!sc) return sc;

  return StatusCode::SUCCESS;
}
//=============================================================================
// makeMother
//=============================================================================
StatusCode TutorialAlgorithm::makeMother(const LHCb::Particle::ConstVector& daughters){
  StatusCode sc = StatusCode::SUCCESS ;

  LHCb::Particle::ConstVector DaPlus, DaMinus;
  sc = particleFilter()->filterNegative(daughters,DaMinus);
  if (sc) sc = particleFilter()->filterPositive(daughters,DaPlus);
  if (!sc) {
    err() << "Error while filtering" << endmsg ;
    return sc ;
  }
  for ( LHCb::Particle::ConstVector::const_iterator imp =  DaPlus.begin() ;
        imp != DaPlus.end() ; ++imp ){
    for ( LHCb::Particle::ConstVector::const_iterator imm =  DaMinus.begin() ;
          imm != DaMinus.end() ; ++imm ){
      // mass
      Gaudi::LorentzVector twoDa = (*imp)->momentum() + (*imm)->momentum() ;
      verbose() << "Two daughter mass is " << twoDa.M()/GeV << endmsg ;
      plot(twoDa.M(),"TwoP mass",m_motherMass-3*m_motherMassWin,
           m_motherMass+3*m_motherMassWin);
      if ( fabs ( twoDa.M() - m_motherMass ) > m_motherMassWin ) continue ; // mass cuts
      // vertex 
      LHCb::Vertex DaDaVertex;
      LHCb::Particle Mother(m_motherID);
      
      StatusCode scFit = vertexFitter()->fit(*(*imp),*(*imm),Mother,DaDaVertex);
      if (!scFit) {
        Warning("Fit error");
        continue;
      }
      debug() << "Vertex fit at " << DaDaVertex.position()/cm
              << " with chi2 " << DaDaVertex.chi2() << endmsg;
      plot(DaDaVertex.chi2(),"TwoP Chi^2",0.,200.);
      if ( DaDaVertex.chi2() > m_motherChi2 ) continue ; // chi2 cut
      // happy -> save
      plot(twoDa.M(),"Selected TwoP mass",m_motherMass-m_motherMassWin,
           m_motherMass+m_motherMassWin);
      setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
      desktop()->save(&Mother);
      debug() << "Saved mother " << Mother.particleID().pid() << " to desktop" << endmsg ;
      plotDaughter(*imp,"Selected ");
      plotDaughter(*imm,"Selected ");
      ++m_nMothers ;
    }
  }
  return desktop()->saveDesktop() ; // save them all
}
//=============================================================================
// loop on daughters
//=============================================================================
StatusCode TutorialAlgorithm::loopOnDaughters(const LHCb::Particle::ConstVector& daughters)const {

  StatusCode sc = StatusCode::SUCCESS ;

  for ( LHCb::Particle::ConstVector::const_iterator im =  daughters.begin() ;
        im != daughters.end() ; ++im ){
    sc = plotDaughter(*im,"All");
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// plot daughters
//=============================================================================
StatusCode TutorialAlgorithm::plotDaughter(const LHCb::Particle* da, const std::string& head )const {

  StatusCode sc = StatusCode::SUCCESS ;

  plot(da->p(),  head+" Daughter P",  0., 50.*GeV);    // momentum
  plot(da->pt(), head+" Daughter Pt", 0., 5.*GeV );  // Pt
  debug() << da->momentum() << endmsg ;
  const LHCb::RecVertex::ConstVector& prims = desktop()->primaryVertices() ;
  for ( LHCb::RecVertex::ConstVector::const_iterator ipv = prims.begin() ;
        ipv != prims.end() ; ++ipv ){
    double IP, IPE;
    debug() << (*ipv)->position() << endmsg ;
    sc = geomDispCalculator()->calcImpactPar(*da, *(*ipv), IP, IPE);
    if (sc){
      plot(IP, head+" Daughter IP", 0., 10.*mm);
      if (IPE>0.) plot(IP/IPE,  head+" Daughter IP/error", 0., 10.);
    } 
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Found " << m_nMothers << " " << m_motherName << " in " << m_nEvents << " events" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
} 

//=============================================================================



