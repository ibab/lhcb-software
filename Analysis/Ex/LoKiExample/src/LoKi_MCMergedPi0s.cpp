// $Id: LoKi_MCMergedPi0s.cpp,v 1.1 2007-04-01 15:28:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
//
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 * 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-26
 */
// ============================================================================
LOKI_MCALGORITHM(LoKi_MCMergedPi0s)
{
  using namespace LoKi ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  //
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  MCFinder finder = mcFinder("pi0->2gamma") ;
  
  MCRange mcpi0 = finder->findDecays("pi0 -> gamma gamma") ;
  if ( mcpi0.empty() )  { return Warning("No MC-pi0 are found!", SUCCESS ) ; }
  
  // get ony pi0s, which:
  // 1) large Pt
  MCCut mc1 = MCPT > 500 * Gaudi::Units::MeV  ;
  // 2) valid origin vertex 
  MCCut mc2 = MCOVALID ;
  // 2) vertex near the primary vertex 
  MCCut mc3 = abs ( MCVFASPF( MCVZ ) ) < 500 * Gaudi::Units::mm ;
  
  MCCut mccut = mc1 && mc2 && mc3 ;
  MCRange mcpi = mcselect ( "mcpi" , mcpi0 , mccut ) ;
  
  if ( mcpi.empty() )  { return Warning("No MC-pi0 are found!", SUCCESS ) ; }
  
  // get the relation table form TES 
  const LHCb::Calo2MC::IClusterTable* table = 
    get<LHCb::Calo2MC::IClusterTable>( "Relations/" + LHCb::CaloClusterLocation::Default ) ;
  
  // invert the table
  typedef Relations::RelationWeighted<LHCb::MCParticle,LHCb::CaloCluster,float> iTable;
  iTable itable = iTable( *table , 1 ) ;
  
  always() << " Sizes "
           << " Direct  " << table->relations().size() 
           << " Inverse " << itable.relations().size() << endreq ;
  
  const LHCb::CaloCluster::Container* clusters = 
    get<LHCb::CaloCluster::Container>( LHCb::CaloClusterLocation::Ecal ) ;
  
  typedef std::vector<const ContainedObject*> COBJECTS ;
  const COBJECTS* cobjects = clusters->containedObjects() ;
  
  if ( 0 != cobjects ) 
  {
    for ( COBJECTS::const_iterator ico = cobjects->begin() ; 
          cobjects->end() != ico ; ++ico ) 
    {
      const ContainedObject* co = *ico;
      if ( 0 == co ) { continue ; }
      const LHCb::CaloCluster* cl = 
        dynamic_cast<const LHCb::CaloCluster*>( co ) ;
      if ( 0 == cl ) { continue ; }
      always() << " COBJECT " << (cl) 
             << " \t#="     << table->relations(cl).size()
               << " \tfrom "  << cobjects->size()
               << " \tkey "   << cl->key()  << endreq ;
    }
  }
  
  for ( LHCb::CaloCluster::Container::const_iterator icl = 
          clusters->begin() ; clusters->end() != icl ; ++icl )
  {
    const LHCb::CaloCluster* cl = *icl ;
    if ( 0 == cl ) { continue ; }
    always() << " Custers " << (cl) 
             << " \t#="    << table->relations(cl).size()
             << " \tfrom "  << clusters->size()
             << " \tkey "   << cl->key()  << endreq ;
    
  }
    
  
  return SUCCESS ;                                                   // RETURN
};
// ============================================================================
// The END 
// ============================================================================
