// $Id: PhysMCTypes.h,v 1.5 2007-11-06 10:53:15 cattanem Exp $
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
// LoKiPhysMC
// ============================================================================
#include "Kernel/Track2MC.h"
#include "Kernel/Particle2MC.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi 
{
  namespace  PhysMCTypes 
  {
    /// the actual type of Particle->MCParticle relation  
    typedef LHCb::P2MC                                            TableP2MC   ;
    /// the actual type of Particle->MCParticle relation with weight 
    typedef LHCb::P2MCW                                           TableP2MCW  ;
    /// the actual type of ProtoParticle->MCParticle relation with weight 
    typedef IRelationWeighted
    <LHCb::ProtoParticle,LHCb::MCParticle,double>                  TablePP2MC ;
    /// the actual type of Track->MCParticle relation
    typedef IRelation
    <LHCb::Track,LHCb::MCParticle>                                 TableT2MC  ;
    /// the actual type of Track->MCParticle relation with weight 
    typedef LHCb::Track2MC                                        TableT2MCW  ;
  }  // end of namespace LoKi::PhysMCTypes 
  
  namespace Types 
  {    
    /// the actual type of Particle->MCParticle relation  
    typedef LoKi::PhysMCTypes::TableP2MC                           TableP2MC ; 
    /// the actual type of Particle->MCParticle relation with weight  
    typedef LoKi::PhysMCTypes::TableP2MCW                         TableP2MCW ; 
    /// the actual type of ProtoParticle->MCParticle relation with weight  
    typedef LoKi::PhysMCTypes::TablePP2MC                         TablePP2MC ; 
    /// the actual type of Track->MCParticle relations 
    typedef LoKi::PhysMCTypes::TableT2MC                           TableT2MC ; 
    /// the actual type of Track->MCParticle relations with weight
    typedef LoKi::PhysMCTypes::TableT2MCW                         TableT2MCW ; 
  } ; // end of namespace LoKi::Types
}  // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCTYPES_H
// ============================================================================
