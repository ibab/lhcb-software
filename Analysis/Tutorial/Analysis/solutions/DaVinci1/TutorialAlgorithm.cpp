// $Id: TutorialAlgorithm.cpp,v 1.9 2009-01-06 10:15:05 pkoppenb Exp $
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
  : DaVinciTupleAlgorithm ( name , pSvcLocator )
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
  StatusCode sc = DaVinciTupleAlgorithm::initialize(); 
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
  const LHCb::Particle::Range muons = this->particles(); // get particles
  sc = loopOnMuons(muons);
  if (!sc) return sc;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
// loop on muons
//=============================================================================
StatusCode TutorialAlgorithm::loopOnMuons(const LHCb::Particle::Range& muons)const {

  StatusCode sc = StatusCode::SUCCESS ;

  // code goes here  

  const LHCb::RecVertex::Range pvs = this->primaryVertices();

  counter("# PV")+= pvs.size();
  counter("# muons")+= muons.size();

  for(LHCb::Particle::Range::const_iterator im = muons.begin(); im != muons.end(); ++im)
  {
    plot((*im)->p(),  "P", "Muon P",  0., 50.*Gaudi::Units::GeV);  // momentum
    plot((*im)->pt(), "Pt", "Muon Pt", 0., 5.*Gaudi::Units::GeV );  // Pt

    if(msgLevel(MSG::DEBUG)) { debug() << "Mu Momentum: " << (*im)->momentum() << endmsg; }

    for(LHCb::RecVertex::Range::const_iterator ipv = pvs.begin(); ipv != pvs.end(); ++ipv)
    {
      double IP, IPchi2;

      if(msgLevel(MSG::DEBUG)) { debug() << (*ipv)->position() << endmsg; }

      sc = distanceCalculator()->distance((*im), (*ipv), IP, IPchi2);
      if(sc)
      {
        plot(IP, "IP", "Muon IP", 0., 10.*Gaudi::Units::mm);
        plot(IPchi2, "IPchi2", "Muon chi2 IP", 0., 10.);
        if( (*im)->pt()>2*Gaudi::Units::GeV )
        {
          plot(IP, "IP_2", "Muon IP for PT>2GeV", 0., 10.*Gaudi::Units::mm);
        }
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

  return DaVinciTupleAlgorithm::finalize(); 
} 

//=============================================================================
