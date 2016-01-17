// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES_H 
#define LOKI_PARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/Particles0.h"
#include "LoKi/Particles1.h"
#include "LoKi/Particles2.h"
#include "LoKi/Particles3.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles5.h"
#include "LoKi/Particles6.h"
#include "LoKi/Particles7.h"
#include "LoKi/Particles8.h"
#include "LoKi/Particles9.h"
#include "LoKi/Particles10.h"
#include "LoKi/Particles11.h"
#include "LoKi/Particles12.h"
#include "LoKi/Particles13.h"
#include "LoKi/Particles14.h"
#include "LoKi/Particles15.h"
#include "LoKi/Particles16.h"
#include "LoKi/Particles17.h"
#include "LoKi/Particles18.h"
#include "LoKi/Particles19.h"
#include "LoKi/Particles21.h"
#include "LoKi/Particles22.h"
#include "LoKi/Particles23.h"
#include "LoKi/Particles24.h"
// ============================================================================
#include "LoKi/Particles31.h"
#include "LoKi/Particles33.h"
#include "LoKi/Particles34.h"
#include "LoKi/Particles35.h"
#include "LoKi/Particles36.h"
#include "LoKi/Particles37.h"
#include "LoKi/Particles38.h"
#include "LoKi/Particles44.h"
#include "LoKi/Particles45.h"
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
 *  @date 2006-01-28 
 */
// ============================================================================
namespace LoKi
{
  // ===========================================================================
  /** @namespace LoKi::Particles Particles.h LoKi/Particles.h
   *  Namespace with colleciton of "Particle" functions for LoKi
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace Particles 
  {
    // ========================================================================
    GAUDI_API 
    std::string hex_id ( const LHCb::Particle* particle ) ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES_H
// ============================================================================
