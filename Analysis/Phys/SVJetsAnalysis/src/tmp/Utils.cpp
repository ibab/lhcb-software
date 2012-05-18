// $Id: $
#ifndef UTILS_H
#define UTILS_H 1
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"

// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
#include <Kernel/IJetMaker.h>
// Trigger Tool
#include "Kernel/ITriggerTisTos.h"
// ===========================================================================

#include "Utils.h"

/// avoid long names:
using namespace LoKi        ;
using namespace LoKi::Types ;
using namespace LoKi::Cuts  ;
using namespace LoKi::Particles  ;
using namespace LoKi::Vertices  ;
using namespace LHCb;

 // ============================================================================
 //  clone method (mandatory!)
 // ============================================================================
 LoKi::GenParticles::Phi2*
 LoKi::GenParticles::Phi2::clone() const 
 { return new LoKi::GenParticles::Phi2( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::GenParticles::Phi2::~Phi2(){}
 // ============================================================================
 //  the only one essential method 
 // ============================================================================
 LoKi::GenParticles::Phi2::result_type
 LoKi::GenParticles::Phi2::operator() 
   ( LoKi::GenParticles::Phi2::argument p ) const
 {
   if( 0 == p )
   {
   Error("Invalid HepMC::GenParticle, return 'InvalidMomenum'");
   return LoKi::Constants::InvalidMomentum;                   // RETURN 
   }

   //return ((int) partn->info(951,-10)); // TODO - Create a functor for this
   return adjust ( phi ( p ) ) ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::GenParticles::Phi2::fillStream 
 ( std::ostream& s ) const { return s << "GPHI" ; }


#endif /*UTILS_H*/ 
