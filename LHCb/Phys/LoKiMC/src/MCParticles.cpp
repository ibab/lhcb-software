// $Id: MCParticles.cpp,v 1.1.1.1 2006-01-26 16:13:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCParticles.h"
#include "LoKi/Kinematics.h"
#include "LoKi/MCTrees.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Kernel/IMCDecayFinder.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::MCParticles 
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
LoKi::MCParticles::Momentum::result_type 
LoKi::MCParticles::Momentum::operator() 
  ( LoKi::MCParticles::Momentum::argument p ) const
{
  if ( 0 != p ) { return p -> momentum ().P() ; }    // RETURN 
  Error (" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::Momentum*
LoKi::MCParticles::Momentum::clone() const 
{ return new LoKi::MCParticles::Momentum(*this) ; }
// ============================================================================


// ============================================================================
std::ostream&
LoKi::MCParticles::Momentum::fillStream( std::ostream& s ) const 
{ return s << "MCP" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::Energy::result_type 
LoKi::MCParticles::Energy::operator() 
  ( LoKi::MCParticles::Energy::argument p ) const
{
  if ( 0 != p ) { return p -> momentum () .e ()  ; }          // RETURN 
  Error (" Invalid Particle, return 'InvalidEnergy'");
  return LoKi::Constants::InvalidEnergy;                     // RETURN 
};
// ============================================================================
LoKi::MCParticles::Energy*
LoKi::MCParticles::Energy::clone() const 
{ return new LoKi::MCParticles::Energy(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::Energy::fillStream( std::ostream& s ) const 
{ return s << "MCE" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::TransverseMomentum::result_type 
LoKi::MCParticles::TransverseMomentum::operator() 
  ( LoKi::MCParticles::TransverseMomentum::argument p ) const
{
  if( 0 != p ) { return p -> momentum().Pt() ; }           // RETURN 
  Error(" Invalid Particle, return 'InvalidMomentum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::TransverseMomentum*
LoKi::MCParticles::TransverseMomentum::clone() const 
{ return new LoKi::MCParticles::TransverseMomentum(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::TransverseMomentum::fillStream
( std::ostream& s ) const 
{ return s << "MCPT" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::MomentumX::result_type 
LoKi::MCParticles::MomentumX::operator() 
  ( LoKi::MCParticles::MomentumX::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . px () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::MomentumX*
LoKi::MCParticles::MomentumX::clone() const 
{ return new LoKi::MCParticles::MomentumX(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumX::fillStream
( std::ostream& s ) const 
{ return s << "MCPX" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::MomentumY::result_type 
LoKi::MCParticles::MomentumY::operator() 
  ( LoKi::MCParticles::MomentumY::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . py () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::MomentumY*
LoKi::MCParticles::MomentumY::clone() const 
{ return new LoKi::MCParticles::MomentumY(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumY::fillStream
( std::ostream& s ) const 
{ return s << "MCPY" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::MomentumZ::result_type 
LoKi::MCParticles::MomentumZ::operator() 
  ( LoKi::MCParticles::MomentumZ::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . pz () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::MomentumZ*
LoKi::MCParticles::MomentumZ::clone() const 
{ return new LoKi::MCParticles::MomentumZ(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::MomentumZ::fillStream
( std::ostream& s ) const 
{ return s << "MCPZ" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::PseudoRapidity::result_type 
LoKi::MCParticles::PseudoRapidity::operator() 
  ( LoKi::MCParticles::PseudoRapidity::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . Eta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::PseudoRapidity*
LoKi::MCParticles::PseudoRapidity::clone() const 
{ return new LoKi::MCParticles::PseudoRapidity(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::PseudoRapidity::fillStream
( std::ostream& s ) const 
{ return s << "MCETA" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::Theta::result_type 
LoKi::MCParticles::Theta::operator() 
  ( LoKi::MCParticles::Theta::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . theta () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================

// ============================================================================
LoKi::MCParticles::Theta*
LoKi::MCParticles::Theta::clone() const 
{ return new LoKi::MCParticles::Theta(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::Theta::fillStream
( std::ostream& s ) const 
{ return s << "MCTHETA" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::Phi::result_type 
LoKi::MCParticles::Phi::operator() 
  ( LoKi::MCParticles::Phi::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . phi () ; }    // RETURN 
  Error(" Invalid Particle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::Phi*
LoKi::MCParticles::Phi::clone() const 
{ return new LoKi::MCParticles::Phi(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::Phi::fillStream
( std::ostream& s ) const 
{ return s << "MCPHI" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::Mass::result_type 
LoKi::MCParticles::Mass::operator() 
  ( LoKi::MCParticles::Mass::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . M  () ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidMass'");
  return LoKi::Constants::InvalidMass;                   // RETURN 
};
// ============================================================================
LoKi::MCParticles::Mass*
LoKi::MCParticles::Mass::clone() const 
{ return new LoKi::MCParticles::Mass(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::Mass::fillStream
( std::ostream& s ) const 
{ return s << "MCMASS" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::Identifier::result_type 
LoKi::MCParticles::Identifier::operator() 
  ( LoKi::MCParticles::Identifier::argument p ) const  
{
  if( 0 != p ) { return p -> particleID() . pid () ; }         // RETURN 
  Error(" Invalid Particle, return 'InvalidID'");
  return LoKi::Constants::InvalidID;                           // RETURN 
};
// ============================================================================
LoKi::MCParticles::Identifier*
LoKi::MCParticles::Identifier::clone() const 
{ return new LoKi::MCParticles::Identifier(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::Identifier::fillStream
( std::ostream& s ) const 
{ return s << "MCID" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::AbsIdentifier::result_type 
LoKi::MCParticles::AbsIdentifier::operator() 
  ( LoKi::MCParticles::AbsIdentifier::argument p ) const  
{
  if( 0 != p ) { return p -> particleID() . abspid () ; }     // RETURN 
  Error(" Invalid Particle, return 'InvalidID'");
  return LoKi::Constants::InvalidID;                           // RETURN 
};
// ============================================================================
LoKi::MCParticles::AbsIdentifier*
LoKi::MCParticles::AbsIdentifier::clone() const 
{ return new LoKi::MCParticles::AbsIdentifier(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::AbsIdentifier::fillStream
( std::ostream& s ) const 
{ return s << "MCABSID" ; }
// ============================================================================


// ============================================================================
LoKi::MCParticles::ThreeCharge::result_type 
LoKi::MCParticles::ThreeCharge::operator() 
  ( LoKi::MCParticles::ThreeCharge::argument p ) const  
{
  if( 0 != p ) { return p -> particleID().threeCharge()  ; } // RETURN 
  Error(" Invalid Particle, return 'InvalidCharge'");
  return LoKi::Constants::InvalidCharge;                     // RETURN 
};
// ============================================================================
LoKi::MCParticles::ThreeCharge*
LoKi::MCParticles::ThreeCharge::clone() const 
{ return new LoKi::MCParticles::ThreeCharge(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::ThreeCharge::fillStream
( std::ostream& s ) const 
{ return s << "MC3Q" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::ProperLifeTime::result_type 
LoKi::MCParticles::ProperLifeTime::operator() 
  ( LoKi::MCParticles::ProperLifeTime::argument p ) const
{
  if ( 0 == p ) 
  {
    Error(" Invalid Particle, return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  // 
  if ( p->endVertices().empty() ) 
  { return LoKi::Constants::InfiniteTime ; }                // RETURN}
  //
  const LHCb::MCVertex* endVertex = p->endVertices()[0] ;
  if ( 0 == endVertex ) 
  {
    Error (" Invalid 'endVertex' , return 'InfiniteTime'");
    return LoKi::Constants::InfiniteTime ;                  // RETURN 
  };
  const LHCb::MCVertex* originVertex = p->originVertex() ;
  if ( 0 == originVertex ) 
  {
    Error(" Invalid 'originVertex' , return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  // evaluate the distance 
  const double dist = ( endVertex     -> position() - 
                        originVertex  -> position() ).R();
  // recalculate distance to the time 
  const double mass     = p->momentum().M() ;
  const double mom      = p->momentum().P() ;
  
  if ( 0 >= mass ) 
  { 
    Error (" Mass is invalid, return 'InvalidTime' ");  
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  if ( 0 >= mom  ) 
  { 
    Error (" |Momentum| is invalid, return 'InvalidTime' ");
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  const double betagamma   = mom / mass ;
  //
  if ( 0 >= betagamma  ) 
  { 
    Error (" betagamma is invalid, return 'InvalidTime' ");
    return  LoKi::Constants::InvalidTime ;              // RETURN 
  }  
  //
  return dist/betagamma ;
};
// ============================================================================
LoKi::MCParticles::ProperLifeTime*
LoKi::MCParticles::ProperLifeTime::clone() const 
{ return new LoKi::MCParticles::ProperLifeTime(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::ProperLifeTime::fillStream
( std::ostream& s ) const 
{ return s << "MCCTAU" ; }
// ============================================================================

// ============================================================================
/// constructor from Quark 
// ============================================================================
LoKi::MCParticles::HasQuark::HasQuark ( const LHCb::ParticleID::Quark quark ) 
  : LoKi::Predicate<const LHCb::MCParticle*>() 
  , m_quark ( quark  )
{};
// ============================================================================
LoKi::MCParticles::HasQuark::HasQuark 
( const LoKi::MCParticles::HasQuark& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right ) 
  , m_quark ( right.m_quark  )
{};
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::HasQuark::result_type 
LoKi::MCParticles::HasQuark::operator() 
  ( const LoKi::MCParticles::HasQuark::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().hasQuark( quark() ) ;
};
// ============================================================================
LoKi::MCParticles::HasQuark*
LoKi::MCParticles::HasQuark::clone() const 
{ return new LoKi::MCParticles::HasQuark(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::HasQuark::fillStream
( std::ostream& s ) const 
{
  switch ( quark() ) 
  {
  case LHCb::ParticleID::top      :
    return s << "TOP"     ;
    break                 ;
  case LHCb::ParticleID::bottom   :
    return s << "BEAUTY"  ;
    break                 ;
  case LHCb::ParticleID::charm    :
    return s << "CHARM"   ;
    break                 ;
  case LHCb::ParticleID::strange  :
    return s << "STRANGE" ;
    break                 ;
  default                 :
    return s << "MCQUARK[" << quark() << "]" ;
    break                 ;
  }
  //
  return s ; 
};
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsCharged::result_type 
LoKi::MCParticles::IsCharged::operator() 
  ( const LoKi::MCParticles::IsCharged::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 != p->particleID().threeCharge() ;
};
// ============================================================================
LoKi::MCParticles::IsCharged*
LoKi::MCParticles::IsCharged::clone() const 
{ return new LoKi::MCParticles::IsCharged(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsCharged::fillStream
( std::ostream& s ) const 
{ return s << "CHARGED" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsNeutral::result_type 
LoKi::MCParticles::IsNeutral::operator() 
  ( const LoKi::MCParticles::IsNeutral::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 == p->particleID().threeCharge() ;
};
// ============================================================================
LoKi::MCParticles::IsNeutral*
LoKi::MCParticles::IsNeutral::clone() const 
{ return new LoKi::MCParticles::IsNeutral(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsNeutral::fillStream
( std::ostream& s ) const 
{ return s << "NEUTRAL" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsLepton::result_type 
LoKi::MCParticles::IsLepton::operator() 
  ( const LoKi::MCParticles::IsLepton::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isLepton() ;
};
// ============================================================================
LoKi::MCParticles::IsLepton*
LoKi::MCParticles::IsLepton::clone() const 
{ return new LoKi::MCParticles::IsLepton(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsLepton::fillStream
( std::ostream& s ) const 
{ return s << "LEPTON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsMeson::result_type 
LoKi::MCParticles::IsMeson::operator() 
  ( const LoKi::MCParticles::IsMeson::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isMeson() ;
};
// ============================================================================
LoKi::MCParticles::IsMeson*
LoKi::MCParticles::IsMeson::clone() const 
{ return new LoKi::MCParticles::IsMeson(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsMeson::fillStream
( std::ostream& s ) const 
{ return s << "MESON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsBaryon::result_type 
LoKi::MCParticles::IsBaryon::operator() 
  ( const LoKi::MCParticles::IsBaryon::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isBaryon() ;
};
// ============================================================================
LoKi::MCParticles::IsBaryon*
LoKi::MCParticles::IsBaryon::clone() const 
{ return new LoKi::MCParticles::IsBaryon(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsBaryon::fillStream
( std::ostream& s ) const 
{ return s << "BARYON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsHadron::result_type 
LoKi::MCParticles::IsHadron::operator() 
  ( const LoKi::MCParticles::IsHadron::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isHadron() ;
};
// ============================================================================
LoKi::MCParticles::IsHadron*
LoKi::MCParticles::IsHadron::clone() const 
{ return new LoKi::MCParticles::IsHadron(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsHadron::fillStream
( std::ostream& s ) const 
{ return s << "HADRON" ; }
// ============================================================================


// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::MCParticles::IsNucleus::result_type 
LoKi::MCParticles::IsNucleus::operator() 
  ( const LoKi::MCParticles::IsNucleus::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("MCParticle* point to NULL, returs 'false'");
    return false ;
  }
  return p->particleID().isNucleus() ;
};
// ============================================================================
LoKi::MCParticles::IsNucleus*
LoKi::MCParticles::IsNucleus::clone() const 
{ return new LoKi::MCParticles::IsNucleus(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsNucleus::fillStream
( std::ostream& s ) const 
{ return s << "NUCLEUS" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LHCb::MCParticle* mcp )
  : LoKi::Predicate<const LHCb::MCParticle*>()
  , m_cont ( 1 , mcp ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCTypes::MCRange& cnt  )
  : LoKi::Predicate<const LHCb::MCParticle*>()
  , m_cont ( cnt.begin() , cnt.end() ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCTypes::MCContainer& cnt  )
  : LoKi::Predicate<const LHCb::MCParticle*>()
  , m_cont ( cnt.begin() , cnt.end() ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LHCb::MCParticles* cnt  )
  : LoKi::Predicate<const LHCb::MCParticle*>()
  , m_cont () 
{
  if ( 0 == cnt ) { Error(" ::MCParticles* points to NULL!");}
  else            
  { 
    m_cont.reserve ( cnt->size() ) ;
    std::copy( cnt->begin() , cnt->end() , std::back_inserter ( m_cont ) ) ; 
  }
};
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::FromMCDecayTree
( const LoKi::MCParticles::FromMCDecayTree& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right )
  , m_cont ( right.m_cont ) {}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::~FromMCDecayTree(){}
// ============================================================================
LoKi::MCParticles::FromMCDecayTree* 
LoKi::MCParticles::FromMCDecayTree::clone() const 
{ return new FromMCDecayTree(*this) ; }
// ============================================================================
LoKi::MCParticles::FromMCDecayTree::result_type 
LoKi::MCParticles::FromMCDecayTree::operator() 
  ( LoKi::MCParticles::FromMCDecayTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error( "MCParticle* points to NULL, return 'false'");
    return false ;
  }  
  for ( MCCont::const_iterator it = m_cont.begin() ; 
        m_cont.end() != it ; ++it ) 
  {
    const LHCb::MCParticle* parent = *it ;
    if ( 0 == parent ) 
    { Error("MCParticle* point to NULL, skip it ") ;    continue ; }
    if ( LoKi::MCTrees::fromMCTree( p , parent ) ) { return true ; } // RETURN
  }
  return false ;
};
// ============================================================================
/** the add MCparticle into the list of tested particles 
 *  @param p pointet to the particle 
 *  @return status code 
 */
// ============================================================================
StatusCode  LoKi::MCParticles::FromMCDecayTree::add ( const LHCb::MCParticle* p ) 
{
  if ( 0 == p ) { return Error("add(): MCParticle* points to NULL") ; }
  m_cont.push_back( p ) ;
  return StatusCode::SUCCESS ;
};
// ============================================================================
/** remove MCParticle from the list of tested MCParticles 
 *  @param p pointe to the MCParticle 
 *  @return status code 
 */
// ============================================================================
StatusCode  LoKi::MCParticles::FromMCDecayTree::remove ( const LHCb::MCParticle* p )
{
  MCCont::iterator it = 
    std::remove ( m_cont.begin() , m_cont.end() , p ) ;
  m_cont.erase ( it , m_cont.end() ) ;
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::FromMCDecayTree::fillStream
( std::ostream& s ) const 
{ return s << "FROMMCTREE" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::NinMCdownTree::NinMCdownTree
( const LoKi::Predicate<const LHCb::MCParticle*>& cut ) 
  : LoKi::Function<const LHCb::MCParticle*>() 
  , m_cut ( cut )
{};
// ============================================================================
LoKi::MCParticles::NinMCdownTree::~NinMCdownTree(){};
// ============================================================================
LoKi::MCParticles::NinMCdownTree*
LoKi::MCParticles::NinMCdownTree::clone() const
{ return new LoKi::MCParticles::NinMCdownTree(*this); };
// ============================================================================
LoKi::MCParticles::NinMCdownTree::result_type
LoKi::MCParticles::NinMCdownTree::operator() 
  ( LoKi::MCParticles::NinMCdownTree::argument p ) const 
{
  if ( 0 == p ) { Warning ( "MCParticle* points to NULL" ) ; }
  
  double number = 0 ;
  // check the particle itself 
  if ( m_cut( p ) ) { number += 1 ; }
  
  // traverse the tree 
  
  // the traversal is not possible 
  if ( 0 == p     ) { return number ; }
  
  typedef SmartRefVector<LHCb::MCVertex>   EndVertices ;
  typedef SmartRefVector<LHCb::MCParticle> Products    ;
  
  const EndVertices& endVertices = p->endVertices() ;
  for ( EndVertices::const_iterator iv = endVertices.begin() ; 
        endVertices.end() != iv ; ++iv ) 
  {
    const LHCb::MCVertex* vertex = *iv ;
    if ( 0 == vertex ) { continue ; }
    const Products& products = vertex->products() ;
    for ( Products::const_iterator ip = products.begin() ; 
          products.end() != ip ; ++ip ) 
    {
      const LHCb::MCParticle* mcp = *ip ;
      if ( 0 == mcp ) { continue ; }
      // start the recursion 
      number += (*this)(mcp) ;
    };
  }
  return number ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::NinMCdownTree::fillStream
( std::ostream& s ) const 
{ return s << "NINMCDOWN" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::MCMotherFunction::MCMotherFunction
( const LoKi::MCParticles::MCMotherFunction& right ) 
  : LoKi::Function<const LHCb::MCParticle*>( right ) 
  , m_val ( right.m_val ) 
  , m_fun ( right.m_fun ) 
{};
// ============================================================================
LoKi::MCParticles::MCMotherFunction::MCMotherFunction
( const LoKi::Function<const LHCb::MCParticle*>& fun , 
  const double                             val )
  : LoKi::Function<const LHCb::MCParticle*>() 
  , m_val ( val ) 
  , m_fun ( fun ) 
{};
// ============================================================================
LoKi::MCParticles::MCMotherFunction::~MCMotherFunction(){};
// ============================================================================
LoKi::MCParticles::MCMotherFunction*
LoKi::MCParticles::MCMotherFunction::clone() const 
{ return new LoKi::MCParticles::MCMotherFunction( *this ) ; }
// ============================================================================
LoKi::MCParticles::MCMotherFunction::result_type 
LoKi::MCParticles::MCMotherFunction::operator() 
  ( LoKi::MCParticles::MCMotherFunction::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( ": MCParticle* point to NULL, return -1.e+6" ) ; return -1.e+6 ; }
  const LHCb::MCParticle* mother = p->mother() ;
  if ( 0 == mother ) { return m_val ; }
  return m_fun ( mother ) ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::MCMotherFunction::fillStream
( std::ostream& s ) const 
{ return s << "MCMOTHER[" << m_fun << "," << m_val<< "]" ; }
// ============================================================================

// ============================================================================
LoKi::MCParticles::MCMotherPredicate::MCMotherPredicate
( const LoKi::Predicate<const LHCb::MCParticle*>& cut , 
  const bool                                val ) 
  : LoKi::Predicate<const LHCb::MCParticle*>() 
    , m_val ( val ) 
  , m_cut ( cut ) 
{};
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::MCMotherPredicate
( const LoKi::MCParticles::MCMotherPredicate& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right ) 
  , m_val ( right.m_val ) 
  , m_cut ( right.m_cut ) 
{};
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::~MCMotherPredicate(){}
// ============================================================================
LoKi::MCParticles::MCMotherPredicate*
LoKi::MCParticles::MCMotherPredicate::clone() const 
{ return new LoKi::MCParticles::MCMotherPredicate( *this ) ; }
// ============================================================================
LoKi::MCParticles::MCMotherPredicate::result_type 
LoKi::MCParticles::MCMotherPredicate::operator() 
  ( LoKi::MCParticles::MCMotherPredicate::argument p ) const 
{
  if ( 0 == p ) 
  { Error ( ": MCParticle* point to NULL, return 'false'" ) ; return false ; }
  const LHCb::MCParticle* mother = p->mother() ;
  if ( 0 == mother ) { return m_val ; }
  return m_cut ( mother ) ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::MCMotherPredicate::fillStream
( std::ostream& s ) const 
{ return s << "MCMOTHCUT[" << m_cut << ( m_val ? ",true" : ",false" ) << "]" ; }
// ============================================================================

// ============================================================================
/** constructor from 1 particle 
 *  @param object object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LHCb::MCParticle* object ) 
  : LoKi::Predicate<const LHCb::MCParticle*>() 
  , m_objects ( 1 , object )
{}
// ============================================================================
/** constructor from range of particles 
 *  @param range range of object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LoKi::MCTypes::MCRange& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*>() 
  , m_objects ( range.begin() , range.end() )
{}
// ============================================================================
/** constructor from range of particles 
 *  @param range range object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LoKi::MCTypes::MCRange::Base& range ) 
  : LoKi::Predicate<const LHCb::MCParticle*>() 
  , m_objects ( range.begin() , range.end() )
{}
// ============================================================================
/** copy constructor
 *  @param right  object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsParticle::IsParticle
( const LoKi::MCParticles::IsParticle& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right ) 
  , m_objects ( right.m_objects )
{}
// ============================================================================
/// destructor (virtual)
// ============================================================================
LoKi::MCParticles::IsParticle::~IsParticle() { m_objects.clear() ; }
// ============================================================================
/// MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::MCParticles::IsParticle*
LoKi::MCParticles::IsParticle::clone() const 
{ return new LoKi::MCParticles::IsParticle( *this ) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::IsParticle::result_type
LoKi::MCParticles::IsParticle::operator() 
  ( LoKi::MCParticles::IsParticle::argument p ) const 
{
  Objects::const_iterator it = 
    std::find( m_objects.begin() , m_objects.end() , p ) ;
  return m_objects.end() != it ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsParticle::fillStream
( std::ostream& s ) const 
{ return s << "ISMCPART" ; }
// ============================================================================


// ============================================================================
/** constructor from contained object
 *  @param object object to be compared 
 */
// ============================================================================
LoKi::MCParticles::IsContainedObject::IsContainedObject
( const ContainedObject* object )
  : LoKi::Predicate<const LHCb::MCParticle*>() 
  , m_objects ( 1 , object ) 
{}
// ============================================================================
/** copy constructor
 *  @param right  object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsContainedObject::IsContainedObject
( const LoKi::MCParticles::IsContainedObject& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right ) 
  , m_objects ( right.m_objects ) 
{}
// ============================================================================
/// destructor (virtual)
// ============================================================================
LoKi::MCParticles::IsContainedObject::~IsContainedObject() { m_objects.clear() ; }
// ============================================================================
/// MANDATORY: clone method ('virtual constructor')
// ============================================================================
LoKi::MCParticles::IsContainedObject*
LoKi::MCParticles::IsContainedObject::clone() const
{ return new LoKi::MCParticles::IsContainedObject( *this ) ; }
// ============================================================================
LoKi::MCParticles::IsContainedObject::result_type 
LoKi::MCParticles::IsContainedObject::operator() 
  ( LoKi::MCParticles::IsContainedObject::argument p ) const 
{
  Objects::const_iterator it = 
    std::find( m_objects.begin() , m_objects.end() , p ) ;
  return m_objects.end() != it ;
};
// ============================================================================

// ============================================================================
std::ostream&
LoKi::MCParticles::IsContainedObject::fillStream
( std::ostream& s ) const 
{ return s << "ISMCCO" ; }
// ============================================================================

// ============================================================================  
/** constructor
 *  @param vct the reference 4-momentum 
 */
// ============================================================================  
LoKi::MCParticles::MomentumDistance::MomentumDistance
( const LoKi::LorentzVector& vct ) 
  : LoKi::Function<const LHCb::MCParticle*>() 
  , m_vct ( vct ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::MCParticles::MomentumDistance::MomentumDistance
( const LoKi::MCParticles::MomentumDistance& right ) 
  : LoKi::Function<const LHCb::MCParticle*>( right ) 
  , m_vct ( right.m_vct ) 
{};
// ============================================================================

// ============================================================================  
/// MANDATORY::virtual destructor
// ============================================================================  
LoKi::MCParticles::MomentumDistance::~MomentumDistance(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MomentumDistance*
LoKi::MCParticles::MomentumDistance::clone() const 
{ return new LoKi::MCParticles::MomentumDistance(*this) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MomentumDistance::result_type 
LoKi::MCParticles::MomentumDistance::operator() 
  ( LoKi::MCParticles::MomentumDistance::argument p ) const
{
  if ( 0 != p ) 
  { return LoKi::Kinematics::delta2euclidian ( p->momentum() , m_vct ) ; }
  Error ( "Invalid MCParticle*, return (InvalidMomentum)^2") ; 
  return 
    LoKi::Constants::InvalidMomentum * 
    LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MomentumDistance::fillStream 
( std::ostream& s ) const { return s << "MCMOMDIST" ; };
// ============================================================================


// ============================================================================
/** constructor
 *  @param vct direction vector 
 *  @see LoKi::ThreeVector 
 */
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::ThreeVector& vct ) 
  : LoKi::Function<const LHCb::MCParticle*>() 
  , m_vct ( vct ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::MCParticles::TransverseMomentumRel& right ) 
  : LoKi::Function<const LHCb::MCParticle*>( right ) 
  , m_vct ( right.m_vct ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::~TransverseMomentumRel(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel*
LoKi::MCParticles::TransverseMomentumRel::clone() const
{ return new LoKi::MCParticles::TransverseMomentumRel(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::TransverseMomentumRel::result_type 
LoKi::MCParticles::TransverseMomentumRel::operator() 
  ( LoKi::MCParticles::TransverseMomentumRel::argument p ) const
{
  if ( 0 != p ) 
  { return LoKi::Kinematics::transverseMomentumDir( p->momentum() , m_vct ) ; }
  Error ( "Invalid MCParticle*, return 'InvalidMomentum'") ; 
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::TransverseMomentumRel::fillStream
( std::ostream& s ) const { return s << "MCPTDIR" ; }
// ============================================================================

// ============================================================================
/** constructor from the phi
 *  @param phi angle phi 
 */
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi
( const double phi ) 
  : LoKi::Function<const LHCb::MCParticle*> ()
  , m_phi ( phi ) 
{};
// ============================================================================

// ============================================================================
/** constructor from the direction 
 *  @param vct 3D-vector 
 */
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi
( const LoKi::ThreeVector& vct ) 
  : LoKi::Function<const LHCb::MCParticle*> ()
  , m_phi ( vct.phi() ) 
{};
// ============================================================================

// ============================================================================
/** constructor from 4-momentum
 *  @param vct 4-momentum
 */
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi
( const LoKi::LorentzVector& vct ) 
  : LoKi::Function<const LHCb::MCParticle*> () 
  , m_phi ( vct.phi() ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param rigth object to be copied 
 */
// ============================================================================
LoKi::MCParticles::DeltaPhi::DeltaPhi
( const LoKi::MCParticles::DeltaPhi& right ) 
  : LoKi::Function<const LHCb::MCParticle*> ( right )
  , m_phi ( right.m_phi ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::DeltaPhi::~DeltaPhi(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor") 
// ============================================================================
LoKi::MCParticles::DeltaPhi*
LoKi::MCParticles::DeltaPhi::clone() const 
{ return new LoKi::MCParticles::DeltaPhi( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::DeltaPhi::result_type 
LoKi::MCParticles::DeltaPhi::operator()
  ( LoKi::MCParticles::DeltaPhi::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  };
  //
  double dphi = p->momentum().phi() - m_phi ;
  // adjust the value 
  while ( dphi >   180 * degree ) { dphi -= 360 * degree ; }
  while ( dphi <= -180 * degree ) { dphi += 360 * degree ; }
  //
  return dphi ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::DeltaPhi::fillStream
( std::ostream& s ) const { return s << "MCDPHI" ; }
// ============================================================================

// ============================================================================
/** constructor from pseudorapidity
 *  @param eta pseudorapidity
 */
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta 
( const double eta ) 
  : LoKi::Function<const LHCb::MCParticle*> () 
  , m_eta ( eta ) 
{};
// ============================================================================

// ============================================================================
/** constructor from the momentum
 *  @param vct momentum
 */
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta 
( const LoKi::LorentzVector& vct ) 
  : LoKi::Function<const LHCb::MCParticle*> () 
  , m_eta ( vct.Eta()  ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param rigth object to be copied 
 */
// ============================================================================
LoKi::MCParticles::DeltaEta::DeltaEta 
( const LoKi::MCParticles::DeltaEta& right ) 
  : LoKi::Function<const LHCb::MCParticle*> ( right ) 
  , m_eta ( right.m_eta ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::DeltaEta::~DeltaEta(){}
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::DeltaEta*
LoKi::MCParticles::DeltaEta::clone() const  
{ return new LoKi::MCParticles::DeltaEta( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::DeltaEta::result_type
LoKi::MCParticles::DeltaEta::operator() 
  ( LoKi::MCParticles::DeltaEta::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  };
  //
  return p->momentum().Eta() - m_eta ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::DeltaEta::fillStream
( std::ostream& s ) const { return s << "MCDETA" ; }
// ============================================================================


// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::ValidOrigin::~ValidOrigin(){}
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::ValidOrigin*
LoKi::MCParticles::ValidOrigin::clone() const 
{ return new LoKi::MCParticles::ValidOrigin(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::ValidOrigin::result_type 
LoKi::MCParticles::ValidOrigin::operator()
  ( LoKi::MCParticles::ValidOrigin::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'false' ") ;
    return false ;
  };
  return 0 != p->originVertex() ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::ValidOrigin::fillStream
( std::ostream& s ) const { return s << "MCOVALID" ; }
// ============================================================================

// ============================================================================
/** constructor from vertex funtion 
 *  @param vfunc vertex function 
 *  @param err1  value to be returned 
 *         for invalid argument or 
 */
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::MCVertexFunAdapter
( const LoKi::MCTypes::MCVFunc& vfunc , 
  const double                  err   ) 
  : LoKi::Function<const LHCb::MCParticle*>() 
  , m_fun ( vfunc ) 
  , m_err ( err   ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY : virtual destructor 
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::~MCVertexFunAdapter(){}
// ============================================================================

// ============================================================================
/// MANDATORY : clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter*
LoKi::MCParticles::MCVertexFunAdapter::clone() const 
{ return new LoKi::MCParticles::MCVertexFunAdapter(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MCVertexFunAdapter::result_type 
LoKi::MCParticles::MCVertexFunAdapter::operator() 
  ( LoKi::MCParticles::MCVertexFunAdapter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " MCParticle* points to NULL, return 'm_err'") ;
    return m_err ;
  };
  return m_fun ( p->originVertex() ) ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCVertexFunAdapter::fillStream
( std::ostream& s ) const { return s << "MCVFASPF" ; }
// ============================================================================

// ============================================================================
/** constructor 
 *  @param decay  decay descriptor 
 *  @param finder decay finder tool 
 *  @see IMCDecayFinder
 */
// ============================================================================
LoKi::MCParticles::MCDecayPattern::MCDecayPattern 
( const std::string& decay  ,
  IMCDecayFinder*    finder ) 
  : LoKi::Predicate<const LHCb::MCParticle*>()
  , m_finder ( finder ) 
  , m_decay  ( decay  ) 
{
  // add reference 
  if ( 0 != m_finder ) { m_finder->addRef() ; } 
} ;
// ============================================================================

// ============================================================================
/** copy constructor
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::MCParticles::MCDecayPattern::MCDecayPattern 
( const LoKi::MCParticles::MCDecayPattern& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*>( right )
  , m_finder ( right.m_finder ) 
  , m_decay  ( right.m_decay  ) 
{
  // add reference 
  if ( 0 != m_finder ) { m_finder->addRef() ; } 
} ;
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::MCDecayPattern::~MCDecayPattern()
{ if ( 0 != m_finder ) { m_finder->release() ; m_finder = 0  ; } }
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::MCDecayPattern*
LoKi::MCParticles::MCDecayPattern::clone() const 
{ return new LoKi::MCParticles::MCDecayPattern(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::MCDecayPattern::result_type 
LoKi::MCParticles::MCDecayPattern::operator() 
  ( LoKi::MCParticles::MCDecayPattern::argument p ) const 
{
  const LHCb::MCParticle* mc = p ;
  if ( 0 == p || 0 == mc ) 
  {
    Error ( " MCParticle* points to NULL, return 'false'") ;
    return false ;
  }
  if ( 0 == m_finder ) 
  {
    Error ( " IMCDecayFinder* points to NULL, return 'false'") ;
    return false ;
  }
  if ( m_finder->decay() != m_decay ) 
  {
    StatusCode sc = m_finder->setDecay( m_decay ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Error from IMCDecayFinder::setDecay('"
              + m_decay + "') , return 'false' " , sc ) ;
      return false ;
    }
  }
  // prepare helper vector 
  std::vector<LHCb::MCParticle*> 
    vMC ( 1 , const_cast<LHCb::MCParticle*> ( mc ) ) ;
  // use the tool 
  return m_finder->hasDecay( vMC ) ;
} ;
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::MCDecayPattern::fillStream
( std::ostream& s ) const { return s << "MCDECAY['" << m_decay << "']" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
