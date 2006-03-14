// $Id: PhysMCTypes.h,v 1.1.1.1 2006-03-14 19:12:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_PHYSMCTYPES_H 
#define LOKI_PHYSMCTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/MCParticle.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace PhysMCTypes PhysMCTypes.h LoKi/PhysMCTypes.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  namespace  PhysMCTypes 
  {
    /// the actual type of Particle->MCParticle relation  
    typedef IRelation
    <LHCb::Particle,LHCb::MCParticle>                              TableP2MC ;
    /// the actual type of Particle->MCParticle relation with weight 
    typedef IRelationWeighted
    <LHCb::Particle,LHCb::MCParticle,double>                      TableWP2MC ;
    /// the actual type of ProtoParticle->MCParticle relation with weight 
    typedef IRelationWeighted
    <LHCb::ProtoParticle,LHCb::MCParticle,double>                TableWPP2MC ;
    /// the actual type of Track->MCParticle relation
    typedef IRelation
    <LHCb::Track,LHCb::MCParticle>                                 TableT2MC ;
    /// the actual type of Track->MCParticle relation with weight 
    typedef IRelationWeighted
    <LHCb::Track,LHCb::MCParticle,double>                        TableWDT2MC ;
    /// the actual type of Track->MCParticle relation with weight 
    typedef IRelationWeighted
    <LHCb::Track,LHCb::MCParticle,int>                           TableWIT2MC ;
    
  } ; // end of namespace LoKi::PhysMCTypes 
  
  namespace Types 
  {
    
    /// the actual type of Particle->MCParticle relation  
    typedef LoKi::PhysMCTypes::TableP2MC                           TableP2MC ; 
    /// the actual type of Particle->MCParticle relation with weight  
    typedef LoKi::PhysMCTypes::TableWP2MC                         TableWP2MC ; 
    /// the actual type of ProtoParticle->MCParticle relation with weight  
    typedef LoKi::PhysMCTypes::TableWPP2MC                       TableWPP2MC ; 
    /// the actual type of Track->MCParticle relations 
    typedef LoKi::PhysMCTypes::TableT2MC                           TableT2MC ; 
    /// the actual type of Track->MCParticle relations with weight
    typedef LoKi::PhysMCTypes::TableWDT2MC                       TableWDT2MC ; 
    /// the actual type of Track->MCParticle relations with weight
    typedef LoKi::PhysMCTypes::TableWIT2MC                       TableWIT2MC ;
    
  } ; // end of namespace LoKi::Types
} ; // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCTYPES_H
// ============================================================================
