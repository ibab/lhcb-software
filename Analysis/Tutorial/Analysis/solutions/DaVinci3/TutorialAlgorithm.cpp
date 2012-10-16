// $Id: TutorialAlgorithm.cpp,v 1.15 2010-08-05 07:31:50 jpalac Exp $
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
    , m_motherID(0)
    , m_motherMass(0.)
{
  declareProperty("Particle",   m_motherName ="Undefined" );
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
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  
  const LHCb::ParticleProperty* mother = ppSvc()->find( m_motherName );
  if ( 0==mother ) { //
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

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  counter("Events")++;

  // code goes here  
  LHCb::Particle::Range daughters = this->particles(); // get particles
  sc = loopOnDaughters(daughters);
  if (!sc) return sc;
  sc = makeMother(daughters);
  if (!sc) return sc;

  return StatusCode::SUCCESS;
}
//=============================================================================
// makeMother
//=============================================================================
StatusCode TutorialAlgorithm::makeMother(const LHCb::Particle::Range& daughters){

  LHCb::Particle::ConstVector DaPlus, DaMinus;
  size_t _sc = DaVinci::filter(daughters,
                               bind(&LHCb::Particle::charge,_1)<0,
                               DaMinus);
  verbose() << "Filtered " << _sc << " negative particles" << endmsg;
  if (_sc>0) {
    _sc = DaVinci::filter(daughters,
                          bind(&LHCb::Particle::charge,_1)>0,
                          DaPlus);
    verbose() << "Filtered " << _sc << " positive particles" << endmsg;
  } else {
    return Warning("Filtered no negative particles", StatusCode::SUCCESS,1) ;
  }

  if (_sc <=0) {
    return Warning("Filtered no positive particles", StatusCode::SUCCESS,1);
  }
  

  StatusCode sc = StatusCode::SUCCESS;

  for ( LHCb::Particle::ConstVector::const_iterator imp =  DaPlus.begin() ;
        imp != DaPlus.end() ; ++imp ){
    for ( LHCb::Particle::ConstVector::const_iterator imm =  DaMinus.begin() ;
          imm != DaMinus.end() ; ++imm ){
      // mass
      Gaudi::LorentzVector twoDa = (*imp)->momentum() + (*imm)->momentum() ;
      verbose() << "Two daughter mass is " << twoDa.M()/GeV << endmsg ;
      plot(twoDa.M(), "Mass", "TwoP mass",m_motherMass-3*m_motherMassWin,
           m_motherMass+3*m_motherMassWin);
      if ( fabs ( twoDa.M() - m_motherMass ) > m_motherMassWin ) continue ; // mass cuts
      // vertex 
      LHCb::Vertex DaDaVertex;
      LHCb::Particle Mother(m_motherID);
      
      StatusCode scFit = vertexFitter()->fit(*(*imp), *(*imm), DaDaVertex, Mother);
      if (!scFit) {
        Warning("Fit error",StatusCode::SUCCESS).ignore();
        continue;
      }
      if (msgLevel(MSG::DEBUG)) {
        debug() << "Vertex fit at " << DaDaVertex.position()/cm
                << " with chi2 " << DaDaVertex.chi2() << endmsg;
      }
      
      plot(DaDaVertex.chi2(),"Chi2", "TwoP Chi^2",0.,200.);
      if ( DaDaVertex.chi2() > m_motherChi2 ) continue ; // chi2 cut
      // happy -> keep
      plot(twoDa.M(),"SelChi2", "Selected TwoP mass",m_motherMass-m_motherMassWin,
           m_motherMass+m_motherMassWin);
      setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
      this->markTree(&Mother);
      if (msgLevel(MSG::DEBUG)) debug() << "Saved mother " << Mother.particleID().pid() << " to local storage" << endmsg ;
      sc = plotDaughter(*imp,"Selected");
      if (sc) sc = plotDaughter(*imm,"Selected");
      if (!sc) return sc;
      counter("Mothers")++ ;
    }
  }
  if (!sc) {
    setFilterPassed(false);
    return sc ;
  }
  
  return StatusCode::SUCCESS;

}
//=============================================================================
// loop on daughters
//=============================================================================
StatusCode TutorialAlgorithm::loopOnDaughters(const LHCb::Particle::Range& daughters)const {

  for ( LHCb::Particle::Range::const_iterator im =  daughters.begin() ;
        im != daughters.end() ; ++im ){
    StatusCode sc = plotDaughter(*im,"All");
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// plot daughters
//=============================================================================
StatusCode TutorialAlgorithm::plotDaughter(const LHCb::Particle* da, const std::string& head )const {

  plot(da->p(),  head+"P", head+" Daughter P",  0., 50.*GeV);    // momentum
  plot(da->pt(), head+"Pt", head+" Daughter Pt", 0., 5.*GeV );  // Pt
  if (msgLevel(MSG::DEBUG)) debug() << da->momentum() << endmsg ;
  const LHCb::RecVertex::Range prims = this->primaryVertices() ;
  for ( LHCb::RecVertex::Range::const_iterator ipv = prims.begin() ;
        ipv != prims.end() ; ++ipv ){
    double IP, IPchi2;
    if (msgLevel(MSG::DEBUG)) debug() << (*ipv)->position() << endmsg ;
    StatusCode sc = distanceCalculator()->distance(da, (*ipv), IP, IPchi2);
    if (sc){
      plot(IP, head+"IP", head+" Daughter IP", 0., 10.*mm);
      plot(IPchi2, head+"IPchi2",  head+" Daughter IPchi2", 0., 10.);
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



