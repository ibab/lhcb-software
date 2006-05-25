// $Id: TutorialAlgorithm.cpp,v 1.1 2006-05-25 11:58:29 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;
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
  LHCb::PrimVertex::ConstVector pvs = desktop()->primaryVertices(); // get PVs

  for ( LHCb::Particle::ConstVector::const_iterator im =  muons.begin() ;
        im != muons.end() ; ++im ){
    plot((*im)->p(),  "Muon P",  0., 50.*Gaudi::Units::GeV);    // momentum
    plot((*im)->pt(), "Muon Pt", 0., 5.*Gaudi::Units::GeV );  // Pt
    debug() << (*im)->momentum() << endmsg ;
    for ( LHCb::PrimVertex::ConstVector::const_iterator ipv = pvs.begin() ;
          ipv != pvs.end() ; ++ipv ){
      double IP, IPE;
      debug() << (*ipv)->position() << endmsg ;
      sc = geomDispCalculator()->calcImpactPar(*(*im), *(*ipv), IP, IPE);
      if (sc){
        plot(IP, "Muon IP", 0., 10.*Gaudi::Units::mm);
        if (IPE>0.) plot(IP/IPE, "Muon IP/error", 0., 10.);
      } 
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
} 

//=============================================================================
