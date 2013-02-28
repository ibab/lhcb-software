// $Id: LoKi_MCMergedPi0s.cpp,v 1.7 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/Math.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCVertexCuts.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/RelationWeighted1D.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// RecEvent
// ============================================================================
#include "Event/CaloCluster.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/Calo2MC.h"
// ============================================================================
/** @file
 *
 *  Simple exmaple of "merged MC-pi0" selection
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
 *  @date 2007-03-26
 */
// ============================================================================
LOKI_MCALGORITHM(MCMergedPi0s)
{
  using namespace LoKi ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  //
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  MCFinder finder = mcFinder("pi0->2gamma") ;
  
  MCRange mcpi0 = finder->findDecays("pi0 -> gamma gamma") ;
  if ( mcpi0.empty() )  { return Warning("No MC-pi0 are found!", SUCCESS ) ; }
  
  // get only pi0s, which satisfy the criteria:
  // 1) large Pt
  MCCut mc1 = MCPT > 500 * Gaudi::Units::MeV  ;
  // 2) valid origin vertex 
  MCCut mc2 = MCOVALID ;
  // 2) vertex near the primary vertex 
  MCCut mc3 = abs ( MCVFASPF( MCVZ ) ) < 500 * Gaudi::Units::mm ;
  
  MCCut mccut = mc1 && mc2 && mc3 ;
  MCRange mcpi = mcselect ( "mcpi" , mcpi0 , mccut ) ;
  
  if ( mcpi.empty() )  { return Warning ( "No MC-pi0 are found!", SUCCESS ) ; }
  
  // get the relation table form TES 
  const LHCb::Calo2MC::IClusterTable* table = 
    get<LHCb::Calo2MC::IClusterTable>( "Relations/" + LHCb::CaloClusterLocation::Default ) ;
  
  // invert the table(create the inverse table) for local usage 
  typedef Relations::RelationWeighted<LHCb::MCParticle,LHCb::CaloCluster,float> iTable;
  iTable itable = iTable( *table , 1 ) ;
  
  MCFun mcpt = MCPT / Gaudi::Units::GeV ;
  
  MCCut outer = ( abs(MCPY/MCPZ) < 3.00/12.5 ) && ( abs(MCPX/MCPZ) < 4.00/12.5 ) ;
  MCCut inner = ( abs(MCPY/MCPZ) > 0.32/12.5 ) || ( abs(MCPX/MCPZ) > 0.32/12.5 ) ; 
  MCCut acceptance = outer && inner ;

  // loop over mcpi0:
  for ( MCRange::iterator imcpi = mcpi.begin() ; mcpi.end() != imcpi ; ++imcpi ) 
  {
    const LHCb::MCParticle* pi0 = *imcpi ;
    if ( 0 == pi0 )                                 { continue ; }  // CONTINUE 
    // only 1 end vertex 
    if ( pi0->endVertices().empty()     )           { continue ; }  // CONTINUE 
    if ( 1 != pi0->endVertices().size() )           { continue ; }  // CONTINUE 
    // get daughter photons 
    const LHCb::MCVertex* mcv = pi0->endVertices().front() ;
    if ( 0 == mcv )                                 { continue ; }  // CONTINUE 
    const SmartRefVector<LHCb::MCParticle>& products = mcv->products() ;    
    // only 2 daughter particles 
    if ( 2 != products.size() )                     { continue ; }  // CONITNUE 
    const LHCb::MCParticle* gamma1 = products[0] ;
    const LHCb::MCParticle* gamma2 = products[1] ;
    if ( 0 == gamma1 || 0 == gamma2 )               { continue ; }  // CONTINUE
    
    if ( ! acceptance ( gamma1 ) )                  { continue ; }  // CONTINUE 
    if ( ! acceptance ( gamma2 ) )                  { continue ; }  // CONTINUE 
    
    const double pt   = mcpt ( pi0 ) ;
    const double mnpt = std::min( mcpt ( gamma1 ) , mcpt ( gamma2 ) ) ;
    
    plot (   pt , "ALL pi0->gamma gamma                    " , 0 ,  5 ) ;
    plot ( mnpt , "ALL pi0->gamma gamma : min pt of photon " , 0 ,  5 ) ;
    
    iTable::Range clus1 = itable.i_relations ( gamma1 ) ;
    iTable::Range clus2 = itable.i_relations ( gamma1 ) ;
    // each photon have some associated clusters in ECAL 
    if ( clus1.empty() && clus2.empty() )         { continue ; }  // CONTINUE
    
    plot (   pt , "ECAL pi0->gamma gamma                    " , 0 ,  5 ) ;
    plot ( mnpt , "ECAL pi0->gamma gamma : min pt of photon " , 0 ,  5 ) ;
    
    // select only 1 or 2-cluster pi0s 
    iTable::Range clus0 = itable.i_relations ( pi0 ) ;
    if ( 1 != clus0.size() && 2 != clus0.size() ) { continue ; }  // CONTINUE 
    //
    
    plot (   pt , " 1||2 pi0->gamma gamma                    " , 0 ,  5 ) ;
    plot ( mnpt , " 1||2 pi0->gamma gamma : min pt of photon " , 0 ,  5 ) ;
    
    // select only true 2 cluster pi0 
    if      ( 2 == clus0.size() && 
              1 == clus1.size() && 
              1 == clus2.size() &&
              clus1.front().to() != clus2.front().to() )
    {
      plot (   pt , " 2 pi0->gamma gamma                    " , 0 ,  5 ) ;
      plot ( mnpt , " 2 pi0->gamma gamma : min pt of photon " , 0 ,  5 ) ;
    }
    else if ( 1 == clus0.size() && 
              1 == clus1.size() && 
              1 == clus2.size() && 
              clus1.front().to() == clus2.front().to() )              
    {
      plot (   pt , " 1 pi0->gamma gamma                    " , 0 ,  5 ) ;
      plot ( mnpt , " 1 pi0->gamma gamma : min pt of photon " , 0 ,  5 ) ;
    }          
    
  }
  
    
  return SUCCESS ;                                                   // RETURN
}
// ============================================================================
// The END 
// ============================================================================
