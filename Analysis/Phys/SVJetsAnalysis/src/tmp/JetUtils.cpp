// $Id: $
#ifndef JETUTILS_H
#define JETUTILS_H 1
// ============================================================================
// LoKi
// ============================================================================
//#include "LoKi/LoKi.h"
//#include "Kernel/IParticle2MCAssociator.h"
//#include "LoKi/GenTypes.h"
//#include "LoKi/GenParticleCuts.h"

// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
//#include <Kernel/IJetMaker.h>
// Trigger Tool
//#include "Kernel/ITriggerTisTos.h"
// ===========================================================================

//#include "JetUtils.h"

/// avoid long names:
//using namespace LoKi        ;
//using namespace LoKi::Types ;
//using namespace LoKi::Cuts  ;
//using namespace LoKi::Particles  ;
//using namespace LoKi::Vertices  ;
//using namespace LHCb;

 // ============================================================================
 //  clone method (mandatory!)
 // ============================================================================
namespace fooo{
 Phi2*
 Phi2::clone() const 
 { return new Phi2( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 Phi2::~Phi2(){}
 // ============================================================================
 //  the only one essential method 
 // ============================================================================
 Phi2::result_type
 Phi2::operator() 
   ( Phi2::argument p ) const
 {
   if( 0 == p )
   {
   Error("Invalid HepMC::GenParticle, return 'InvalidMomenum'");
   return Constants::InvalidMomentum;                   // RETURN 
   }

   //return ((int) partn->info(951,-10)); // TODO - Create a functor for this
   return adjust ( phi ( p ) ) ;
 }
 // ============================================================================
 //  "SHORT" representation, @see AuxFunBase 
 // ============================================================================
 std::ostream& 
 Phi2::fillStream 
 ( std::ostream& s ) const { return s << "GPHI2" ; }


#endif /*JETUTILS_H*/ 
