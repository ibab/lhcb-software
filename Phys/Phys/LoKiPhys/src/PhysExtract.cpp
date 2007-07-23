// $Id: PhysExtract.cpp,v 1.4 2007-07-23 17:35:56 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Objects.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysExtract.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/PhysExtract.h
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
 *  @date 2006-02-06 
 */
// ============================================================================
/*  The most trivial function to extract LHCb::MuonPID object from 
 *  the basic particle
 *  @see LHCb::MuonPID 
 *  @see LHCb::Particle
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-06 
 */
// ============================================================================
const LHCb::MuonPID* LoKi::Extract::getMuon 
( const LHCb::Particle* particle ) 
{
  if ( 0 == particle ) { return 0 ; }
  const LHCb::ProtoParticle* proto = particle->proto() ;
  if ( 0 == proto    ) { return 0 ; }
  return proto->muonPID() ;
} 
// ============================================================================
/** The most trivial function to extract LHCb::RichPID object from 
 *  the basic particle
 *  @see LHCb::RichPID 
 *  @see LHCb::Particle
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-06 
 */
// ============================================================================
const LHCb::RichPID* LoKi::Extract::getRich 
( const LHCb::Particle* particle ) 
{
  if ( 0 == particle ) { return 0 ; }
  const LHCb::ProtoParticle* proto = particle->proto() ;
  if ( 0 == proto    ) { return 0 ; }
  return proto->richPID() ;
} 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

