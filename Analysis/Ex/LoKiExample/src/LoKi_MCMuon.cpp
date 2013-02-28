// $Id: LoKi_MCMuon.cpp,v 1.10 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2003-03-27 
 */
// ============================================================================
LOKI_MCALGORITHM( MCMuons ) 
{
  using namespace LoKi::Cuts ;
  using namespace LoKi::Types ;
  
  // get all reconstructed muons
  Range   muons   = select   ( "Muons" , ABSID == "mu+" ) ;
  
  // get all MC muons 
  MCRange mcmuons = mcselect ( "MCmu" , MCABSID == "mu+" ) ;
  
  // create the matching  object 
  MCMatch mc = mcTruth();
  
  // check for matching with ALL muons 
  Cut mccut1 = MCTRUTH( mc , mcmuons.begin() , mcmuons.end() ) ;
  
  // check for matching with muon with highest PT 
  MCRange::iterator imu = 
    select_max ( mcmuons.begin() , mcmuons.end() , MCPT ) ;

  Cut mccut2 = ( mcmuons.end() == imu ) ? 
    Cut( MCTRUTH( mc , *imu ) ) : Cut( PNONE ) ;
  
  Tuple tuple = nTuple ( "Muons" ) ;
  
  const double GeV = Gaudi::Units::GeV ;
  
  // loop over all recontructed muons 
  for ( Range::iterator imuon = muons.begin() ; 
        muons.end() != imuon ; ++imuon ) 
  {
    const LHCb::Particle* muon = *imuon ;
    if( 0 == muon ) { continue ; }
    // ID variable 
    tuple -> column ( "DLLmu" , PIDmu  ( muon )  / GeV ) ;
    tuple -> column ( "PT"    , PT     ( muon )  / GeV ) ;
    tuple -> column ( "RCM"   , muon->momentum() / GeV ) ; //4-momentum 
    tuple -> column ( "mc1"   , mccut1 ( muon )  ) ; // true muon?
    tuple -> column ( "mc2"   , mccut2 ( muon )  ) ; // true muon with highest PT
    
    // find MC muon which 'matches' with the recontructed muon
    MCRange::iterator im = 
      mc->match ( muon , mcmuons.begin() , mcmuons.end() ) ;
    
    // 4-momentum of MC muon
    LoKi::LorentzVector lv ;
    
    if( mcmuons.end() != im &&  0 != *im ) 
    { lv    = (*im)->momentum() / GeV ; }
    
    tuple -> column ( "MCM" , lv    ).ignore () ; 
    
    tuple -> write  () .ignore()  ;
  }
  //
  return StatusCode::SUCCESS ;  
} 
// ============================================================================
// The END 
// ============================================================================
