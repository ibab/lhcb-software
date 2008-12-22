// $Id: TutorialAlgorithm.cpp,v 1.8 2008-12-22 18:08:36 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TutorialAlgorithm.h"

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
{

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

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;

  // code goes here  
  LHCb::Particle::ConstVector muons = desktop()->particles(); // get particles
  sc = loopOnMuons(muons);
  if (!sc) return sc;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
// loop on muons
//=============================================================================
StatusCode TutorialAlgorithm::loopOnMuons(const LHCb::Particle::ConstVector& muons)const {

  StatusCode sc = StatusCode::SUCCESS ;

  // code goes here  
  const LHCb::RecVertex::Container* pvs = desktop()->primaryVertices(); // get PVs

  for ( LHCb::Particle::ConstVector::const_iterator im =  muons.begin() ;
        im != muons.end() ; ++im ){
    plot((*im)->p(),  "P", "Muon P",  0., 50.*Gaudi::Units::GeV);  // momentum
    plot((*im)->pt(), "Pt", "Muon Pt", 0., 5.*Gaudi::Units::GeV );  // Pt
    if (msgLevel(MSG::DEBUG)) debug() << "Mu Momentum: " << (*im)->momentum() << endmsg ;
    for ( LHCb::RecVertex::Container::const_iterator ipv = pvs->begin() ;
          ipv != pvs->end() ; ++ipv ){
      double IP, IPchi2;
      if (msgLevel(MSG::DEBUG)) debug() << (*ipv)->position() << endmsg ;
      sc = distanceCalculator()->distance((*im), (*ipv), IP, IPchi2);
      if (sc){
        plot(IP, "IP", "Muon IP", 0., 10.*Gaudi::Units::mm);
        plot(IP/IPchi2, "IPchi2", "Muon chi2 IP", 0., 10.);
        if ( (*im)->pt()>2*Gaudi::Units::GeV) 
          plot(IP, "IP_2", "Muon IP for PT>2GeV", 0., 10.*Gaudi::Units::mm);
      } 
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
