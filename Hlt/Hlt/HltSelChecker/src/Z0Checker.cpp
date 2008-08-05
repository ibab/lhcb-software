// $Id: Z0Checker.cpp,v 1.1 2008-08-05 09:37:43 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/MCParticle.h" 
#include "Kernel/Particle2MCLinker.h"

// local
#include "Z0Checker.h"

using namespace LHCb ;
//-----------------------------------------------------------------------------
// Implementation file for class : Z0Checker
//
// 2008-06-18 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Z0Checker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Z0Checker::Z0Checker( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty("Locations", m_locations);
}
//=============================================================================
// Destructor
//=============================================================================
Z0Checker::~Z0Checker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Z0Checker::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Z0Checker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  MCParticle::ConstVector mup, mum ;

  setFilterPassed(false);

  // code goes here
  MCParticles* mcps = get<MCParticles>( MCParticleLocation::Default );
  for ( MCParticles::const_iterator m = mcps->begin() ; m != mcps->end() ; ++m){
    if ( (*m)->particleID().abspid() != 13 ) continue ;
    if ( 0!=(*m)->mother() ){
      if ( (*m)->mother()->particleID().abspid()==211 ) continue ;
      if ( (*m)->mother()->particleID().abspid()==321 ) continue ;
    }
    if ( fabs((*m)->originVertex()->position().z()-(*m)->primaryVertex()->position().z()) > 1*Gaudi::Units::mm ) continue ;
    debug() << (*m)->particleID().pid() << " with " 
            << (*m)->momentum() << " from " 
            << (*m)->originVertex()->position() << " ";
    if ( 0==(*m)->mother() ) debug() << "No mother " << endmsg ;
    else debug() << (*m)->mother()->particleID().pid() << endmsg ; 
    if ( (*m)->particleID().pid() > 0 ) mum.push_back(*m);
    else mup.push_back(*m);
  }
  if ( mum.size()*mup.size()>0){
    Particle2MCLinker* linker = new Particle2MCLinker( this, Particle2MCMethod::Links, m_locations);  
    for ( MCParticle::ConstVector::const_iterator imp = mup.begin() ;
          imp != mup.end() ; ++imp){
      for ( MCParticle::ConstVector::const_iterator imm = mum.begin() ;
            imm != mum.end() ; ++imm){
        double m = ((*imp)->momentum()+(*imm)->momentum()).mass();
        if ( m > 80*Gaudi::Units::GeV ){
          info() << "Dimuon of mass " << m << endmsg ;
          double w;
          const Particle* Pp = linker->firstP( *imp, w );
          const Particle* Pm = linker->firstP( *imm, w );
          counter("Found Z0")++;
          info() << "    Mu+ " << (*imp)->momentum() ;
          if ( 0!=Pp ) {
            info() << " -> " << Pp->particleID().pid() 
                              << " " << Pp->momentum() << endmsg ;
            counter("Found Reco Mu+")++;
          } else info() << endmsg ;
          info() << "    Mu- " << (*imm)->momentum() ;
          if ( 0!=Pm ) {
            info() << " -> " << Pm->particleID().pid() 
                              << " " << Pm->momentum() << endmsg ;
            counter("Found Reco Mu-")++;
            if (0!=Pp) {
              counter("Found Both Reco Mu")++;
              setFilterPassed(true);
            }
          } else info() << endmsg ;
          if ((( 0!=Pp ) && ( Pp->particleID().threeCharge()!=
                              (*imp)->particleID().threeCharge())) || 
              (( 0!=Pm ) && (  Pm->particleID().threeCharge()!=
                               (*imm)->particleID().threeCharge() )))
            Warning("Charge violation",StatusCode::SUCCESS,0) ;
        }
      }
    }
    delete linker ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Z0Checker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
