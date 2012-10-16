// $Id: TutorialAlgorithm.cpp,v 1.15 2010-07-05 12:28:22 jhe Exp $
// Include files 

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from DaVinci
#include "Kernel/ParticleFilters.h"

// local
#include "TutorialAlgorithm.h"

using namespace Gaudi::Units ;
using namespace boost::lambda;

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
    , m_jPsiID(0)
    , m_jPsiMass(0.)
{
  declareProperty("MassWindow", m_jPsiMassWin = 10.*GeV);
  declareProperty("MaxChi2",    m_jPsiChi2 = 1000.);
}
//=============================================================================
// Destructor
//=============================================================================
TutorialAlgorithm::~TutorialAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialAlgorithm::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  const LHCb::ParticleProperty* mother = ppSvc()->find( "J/psi(1S)" );
  if ( !mother ) { //
    err() << "Cannot find particle property for J/psi(1S)" << endmsg ;
    return StatusCode::FAILURE;
  }
  m_jPsiID = LHCb::ParticleID(mother->pdgID());
  m_jPsiMass = mother->mass();
  info() << "Will reconstruct " << mother->particle() << " (ID=" << m_jPsiID.pid()
         << ") with mass " << m_jPsiMass << endmsg ;
  info() << "Mass window is " << m_jPsiMassWin << " MeV" << endmsg ;
  info() << "Max chi^2 is " << m_jPsiChi2 << endmsg ;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  counter("Events")++;

  // code goes here  
  const LHCb::Particle::Range muons = this->particles(); // get particles
  
  if (msgLevel(MSG::DEBUG)) debug() << "muon size:" <<  muons.size() << endmsg;

  sc = loopOnMuons(muons);
  if (!sc) return sc;
  sc = makeJpsi(muons);
  if (!sc) return sc;

  return StatusCode::SUCCESS;
}
//=============================================================================
// makeJpsi
//=============================================================================
StatusCode TutorialAlgorithm::makeJpsi(const LHCb::Particle::Range& muons){

  LHCb::Particle::ConstVector MuPlus, MuMinus;
  size_t sc = DaVinci::filter(muons, 
                              bind(&LHCb::Particle::charge,_1)<0,
                              MuMinus);
  if (sc>0) {
    sc = DaVinci::filter(muons,
                         bind(&LHCb::Particle::charge,_1)>0,
                         MuPlus);
  } else {
    Warning("Filtered no muons",StatusCode::SUCCESS,1); 
    return StatusCode::SUCCESS ;
  }

  for ( LHCb::Particle::ConstVector::const_iterator imp =  MuPlus.begin() ;
        imp != MuPlus.end() ; ++imp ){
    for ( LHCb::Particle::ConstVector::const_iterator imm =  MuMinus.begin() ;
          imm != MuMinus.end() ; ++imm ){
      // mass
      Gaudi::LorentzVector twoMu = (*imp)->momentum() + (*imm)->momentum() ;
      verbose() << "Two muon mass is " << twoMu.M()/GeV << endmsg ;
      plot(twoMu.M(), "DiMuM", "DiMu mass",2.*GeV,4.*GeV);
      if ( fabs ( twoMu.M() - m_jPsiMass ) > m_jPsiMassWin ) continue ; // mass cuts
      // vertex 
      LHCb::Vertex MuMuVertex;
      LHCb::Particle Jpsi(m_jPsiID);
      StatusCode scFit = vertexFitter()->fit(*(*imp), *(*imm), MuMuVertex, Jpsi);
      if (!scFit) {
        Warning("Fit error").ignore();
        continue;
      }
      if (msgLevel(MSG::DEBUG)) debug() << "Vertex fit at " << MuMuVertex.position()/cm
              << " with chi2 " << MuMuVertex.chi2() << endmsg;
      plot(MuMuVertex.chi2(), "DiMuChi2", "DiMu Chi^2",0.,200.);
      plot(Jpsi.measuredMass(), "Jpsi_MM", "Jpsi measured mass", 3.0*GeV, 3.2*GeV);
      if ( MuMuVertex.chi2() > m_jPsiChi2 ) continue ; // chi2 cut
      // happy -> keep
      plot(twoMu.M(),"SelDiMuChi2", "Selected DiMu mass",m_jPsiMass-m_jPsiMassWin,m_jPsiMass+m_jPsiMassWin);
      setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
      this->markTree(&Jpsi);
      plotMuon(*imp,"Selected").ignore();
      plotMuon(*imm,"Selected").ignore();
      counter("J/psi's")++ ;
    }
  }
  return StatusCode::SUCCESS;
  
}
//=============================================================================
// loop on muons
//=============================================================================
StatusCode TutorialAlgorithm::loopOnMuons(const LHCb::Particle::Range& muons)const {

  StatusCode sc = StatusCode::SUCCESS ;

  for ( LHCb::Particle::Range::const_iterator im =  muons.begin() ;
        im != muons.end() ; ++im ){
    sc = plotMuon(*im,"All");
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// plot muons
//=============================================================================
StatusCode TutorialAlgorithm::plotMuon(const LHCb::Particle* mu, const std::string& head )const {

  StatusCode sc = StatusCode::SUCCESS ;

  plot(mu->p(),  head+"MuP", head+" Muon P",  0., 50.*GeV);    // momentum
  plot(mu->pt(), head+"MuPt", head+" Muon Pt", 0., 5.*GeV );  // Pt
  if (msgLevel(MSG::DEBUG)) debug() << mu->momentum() << endmsg ;

  const LHCb::RecVertex::Range prims = this->primaryVertices() ;
  
  for ( LHCb::RecVertex::Range::const_iterator ipv = prims.begin() ; 
        ipv != prims.end() ; ++ipv )
  {
    double IP, IPchi2;
    if (msgLevel(MSG::DEBUG)) debug() << (*ipv)->position() << endmsg ;
    sc = distanceCalculator()->distance(mu, (*ipv), IP, IPchi2);
    if (sc){
      plot(IP, head+"MuIP", head+" Muon IP", 0., 10.*mm);
      if (IPchi2>0.) plot(IPchi2, head+"MuIPchi2",  head+" Muon IP chi2", 0., 10.);
      //====================================================================
      //
      //====================================================================
    } 
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialAlgorithm::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); 
} 

//=============================================================================



