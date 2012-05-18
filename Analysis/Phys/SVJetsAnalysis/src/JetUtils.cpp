// $Id: $
//#ifndef JETUTILS_H
//#define JETUTILS_H 1
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
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/valid.h"
#include "LoKi/MoreFunctions.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Trees.h"
#include "LoKi/Services.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenExtract.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/GenOscillated.h"
#include "LoKi/IGenDecay.h"


#include "JetUtils.h"

LoKi::JetUtils::MCNEndVertices* 
 LoKi::JetUtils::MCNEndVertices::clone() const
 { return new LoKi::JetUtils::MCNEndVertices ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::MCNEndVertices::~MCNEndVertices() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::MCNEndVertices::result_type 
 LoKi::JetUtils::MCNEndVertices::operator() 
   ( LoKi::JetUtils::MCNEndVertices:: argument p ) const 
 {
   if ( 0 != p ) { return (int) p->endVertices().size(); }
   Error ( "LHCb::MCParticle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::MCNEndVertices::fillStream 
 ( std::ostream& s ) const { return s << "MCNENDVERTICES" ; }

LoKi::JetUtils::Info4Jet* 
 LoKi::JetUtils::Info4Jet::clone() const
 { return new LoKi::JetUtils::Info4Jet ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::Info4Jet::~Info4Jet() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::Info4Jet::result_type 
 LoKi::JetUtils::Info4Jet::operator() 
   ( LoKi::JetUtils::Info4Jet:: argument p ) const 
 {
   if ( 0 != p ) { return (int) p->info(951,-10); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::Info4Jet::fillStream 
 ( std::ostream& s ) const { return s << "INFO4JET" ; }

 // =======================================================================

 LHCb::CaloCluster::Position * CaloClusterPosition(const LHCb::Particle * p){

    LHCb::CaloHypo::Hypos hypos = p->proto()->calo();
    LHCb::CaloHypo::Hypos::iterator ih = hypos.begin();
    LHCb::CaloHypo *ch = *ih;
    LHCb::CaloCluster::Position *pos = ch->position();
    return pos;
 }

// ======================================================================

 LoKi::JetUtils::CaloPositionX* 
 LoKi::JetUtils::CaloPositionX::clone() const
 { return new LoKi::JetUtils::CaloPositionX ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionX::~CaloPositionX() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================


 LoKi::JetUtils::CaloPositionX::result_type 
 LoKi::JetUtils::CaloPositionX::operator() 
   ( LoKi::JetUtils::CaloPositionX:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->x(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionX::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONX" ; }

 // ============================================================================
 // ============================================================================

 LoKi::JetUtils::CaloPositionY* 
 LoKi::JetUtils::CaloPositionY::clone() const
 { return new LoKi::JetUtils::CaloPositionY ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionY::~CaloPositionY() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================


 LoKi::JetUtils::CaloPositionY::result_type 
 LoKi::JetUtils::CaloPositionY::operator() 
   ( LoKi::JetUtils::CaloPositionY:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->y(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionY::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONY" ; }

 // ============================================================================
 // ============================================================================

 LoKi::JetUtils::CaloPositionZ* 
 LoKi::JetUtils::CaloPositionZ::clone() const
 { return new LoKi::JetUtils::CaloPositionZ ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionZ::~CaloPositionZ() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================

 LoKi::JetUtils::CaloPositionZ::result_type 
 LoKi::JetUtils::CaloPositionZ::operator() 
   ( LoKi::JetUtils::CaloPositionZ:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->z(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionZ::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONZ" ; }

// ============================================================================
LoKi::JetUtils::GNDaughters* 
 LoKi::JetUtils::GNDaughters::clone() const
 { return new LoKi::JetUtils::GNDaughters ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::GNDaughters::~GNDaughters() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::GNDaughters::result_type 
LoKi::JetUtils::GNDaughters::operator() 
	(LoKi::GenParticles::Identifier:: argument p ) const 
{
	if ( 0 != p ) { if(p->end_vertex()) return (int) p->end_vertex()->particles_out_size ();

		Error ( "HepMC::GenParticle* -> end_vertex() points to NULL, return InvalidID" ) ;
	}
	Error ( "HepMC::GenParticle* points to NULL, return InvalidID" ) ;
	return LoKi::Constants::InvalidID ;
}
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::GNDaughters::fillStream 
 ( std::ostream& s ) const { return s << "GNDAUGHTERS" ; }

 // =======================================================================
//#endif /*JETUTILS_H*/ 
