// $Id: GenParticles.cpp,v 1.8 2006-06-13 09:05:13 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2006/05/26 17:32:12  ibelyaev
//  update to allow HepMCParticleMaker to be OK
//
// Revision 1.6  2006/05/02 14:30:28  ibelyaev
//  censored
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenParticles.h"
#include "LoKi/GenExtract.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation of the functions form namespace LoKi::GenParticles 
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
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::BarCode*
LoKi::GenParticles::BarCode::clone() const 
{ return new LoKi::GenParticles::BarCode( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::BarCode::~BarCode(){}
// ============================================================================

// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::BarCode::result_type
LoKi::GenParticles::BarCode::operator() 
  ( LoKi::GenParticles::BarCode::argument p ) const
{
  if ( 0 != p ) { return p -> barcode() ; }    // RETURN 
  Error("Invalid HepMC::GenParticle, return 0 ");
  return 0 ;                   // RETURN 
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::BarCode::fillStream 
( std::ostream& s ) const { return s << "GBAR" ; };
// ============================================================================



// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::Identifier* 
LoKi::GenParticles::Identifier::clone() const
{ return new LoKi::GenParticles::Identifier ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Identifier::~Identifier() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Identifier::result_type 
LoKi::GenParticles::Identifier::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LHCb::ParticleID( p->pdg_id() ).pid() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidID" ) ;
  return LoKi::Constants::InvalidID ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Identifier::fillStream 
( std::ostream& s ) const { return s << "GID" ; };
// ============================================================================


// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::Momentum* 
LoKi::GenParticles::Momentum::clone() const
{ return new LoKi::GenParticles::Momentum ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Momentum::~Momentum() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Momentum::result_type 
LoKi::GenParticles::Momentum::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).P() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Momentum::fillStream 
( std::ostream& s ) const { return s << "GP" ; };
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::TransverseMomentum* 
LoKi::GenParticles::TransverseMomentum::clone() const
{ return new LoKi::GenParticles::TransverseMomentum ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::TransverseMomentum::~TransverseMomentum() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::TransverseMomentum::result_type 
LoKi::GenParticles::TransverseMomentum::operator() 
  ( LoKi::GenParticles::TransverseMomentum:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).Pt() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::TransverseMomentum::fillStream 
( std::ostream& s ) const { return s << "GPT" ; };
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::Energy* 
LoKi::GenParticles::Energy::clone() const
{ return new LoKi::GenParticles::Energy ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Energy::~Energy() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Energy::result_type 
LoKi::GenParticles::Energy::operator() 
  ( LoKi::GenParticles::Energy:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).E() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Energy::fillStream 
( std::ostream& s ) const { return s << "GE" ; };
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::Mass* 
LoKi::GenParticles::Mass::clone() const
{ return new LoKi::GenParticles::Mass ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Mass::~Mass() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Mass::result_type 
LoKi::GenParticles::Mass::operator() 
  ( LoKi::GenParticles::Mass:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).M() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Mass::fillStream 
( std::ostream& s ) const { return s << "GM" ; };
// ============================================================================


// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::MomentumX* 
LoKi::GenParticles::MomentumX::clone() const
{ return new LoKi::GenParticles::MomentumX ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumX::~MomentumX() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumX::result_type 
LoKi::GenParticles::MomentumX::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).X() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumX::fillStream 
( std::ostream& s ) const { return s << "GPX" ; };
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::MomentumY* 
LoKi::GenParticles::MomentumY::clone() const
{ return new LoKi::GenParticles::MomentumY ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumY::~MomentumY() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumY::result_type 
LoKi::GenParticles::MomentumY::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).Y() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumY::fillStream 
( std::ostream& s ) const { return s << "GPY" ; };
// ============================================================================



// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::MomentumZ* 
LoKi::GenParticles::MomentumZ::clone() const
{ return new LoKi::GenParticles::MomentumZ ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::MomentumZ::~MomentumZ() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumZ::result_type 
LoKi::GenParticles::MomentumZ::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LoKi::LorentzVector( p -> momentum() ).Z() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidMomentum" ) ;
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumZ::fillStream 
( std::ostream& s ) const { return s << "GPZ" ; };
// ============================================================================




// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenParticles::AbsIdentifier* 
LoKi::GenParticles::AbsIdentifier::clone() const
{ return new LoKi::GenParticles::AbsIdentifier ( *this ) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::AbsIdentifier::~AbsIdentifier() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::AbsIdentifier::result_type 
LoKi::GenParticles::AbsIdentifier::operator() 
  ( LoKi::GenParticles::Identifier:: argument p ) const 
{
  if ( 0 != p ) { return LHCb::ParticleID( p->pdg_id() ).abspid() ; }
  Error ( "HepMC::GenParticle* points to NULL, return InvalidID" ) ;
  return LoKi::Constants::InvalidID ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::AbsIdentifier::fillStream 
( std::ostream& s ) const { return s << "GABSID" ; };
// ============================================================================

// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::PseudoRapidity*
LoKi::GenParticles::PseudoRapidity::clone() const 
{ return new LoKi::GenParticles::PseudoRapidity( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::PseudoRapidity::~PseudoRapidity(){}
// ============================================================================

// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::PseudoRapidity::result_type
LoKi::GenParticles::PseudoRapidity::operator() 
  ( LoKi::GenParticles::PseudoRapidity::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . pseudoRapidity () ; }    // RETURN 
  Error("Invalid HepMC::GenParticle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::PseudoRapidity::fillStream 
( std::ostream& s ) const { return s << "GETA" ; };
// ============================================================================

// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::Phi*
LoKi::GenParticles::Phi::clone() const 
{ return new LoKi::GenParticles::Phi( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Phi::~Phi(){}
// ============================================================================

// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::Phi::result_type
LoKi::GenParticles::Phi::operator() 
  ( LoKi::GenParticles::Phi::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . phi() ; }    // RETURN 
  Error("Invalid HepMC::GenParticle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Phi::fillStream 
( std::ostream& s ) const { return s << "GPHI" ; };
// ============================================================================

// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::Theta*
LoKi::GenParticles::Theta::clone() const 
{ return new LoKi::GenParticles::Theta( *this ) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Theta::~Theta(){}
// ============================================================================

// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::Theta::result_type
LoKi::GenParticles::Theta::operator() 
  ( LoKi::GenParticles::Theta::argument p ) const
{
  if( 0 != p ) { return p -> momentum () . theta() ; }    // RETURN 
  Error("Invalid HepMC::GenParticle, return 'InvalidMomenum'");
  return LoKi::Constants::InvalidMomentum;                   // RETURN 
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::Theta::fillStream 
( std::ostream& s ) const { return s << "GTHETA" ; };
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::ValidEndVertex*
LoKi::GenParticles::ValidEndVertex::clone() const 
{ return new LoKi::GenParticles::ValidEndVertex(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::ValidEndVertex::~ValidEndVertex() {};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::ValidEndVertex::result_type 
LoKi::GenParticles::ValidEndVertex::operator() 
  ( LoKi::GenParticles::ValidEndVertex::argument p ) const 
{
  if ( 0 != p ) { return 0 != p->end_vertex() ; }
  Error ( "HepMC::GenParticle* points to NULL, return 'false'" ) ;
  return false ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::ValidEndVertex::fillStream 
( std::ostream& s ) const { return s << "GVEV" ; };
// ============================================================================

// ============================================================================  
/** constructor
 *  @param vct the reference 4-momentum 
 */
// ============================================================================  
LoKi::GenParticles::MomentumDistance::MomentumDistance
( const LoKi::LorentzVector& vct ) 
  : LoKi::Function<const HepMC::GenParticle*>() 
  , m_vct ( vct ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::MomentumDistance::MomentumDistance
( const LoKi::GenParticles::MomentumDistance& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right ) 
  , m_vct ( right.m_vct ) 
{};
// ============================================================================

// ============================================================================  
/// MANDATORY::virtual destructor
// ============================================================================  
LoKi::GenParticles::MomentumDistance::~MomentumDistance(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::GenParticles::MomentumDistance*
LoKi::GenParticles::MomentumDistance::clone() const 
{ return new LoKi::GenParticles::MomentumDistance(*this) ; };
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::MomentumDistance::result_type 
LoKi::GenParticles::MomentumDistance::operator() 
  ( LoKi::GenParticles::MomentumDistance::argument p ) const
{
  if ( 0 != p ) 
  {
    const double de = ( p->momentum().e() - m_vct.e() ) ;
    const double dx = ( p->momentum().x() - m_vct.x() ) ;
    const double dy = ( p->momentum().y() - m_vct.y() ) ;
    const double dz = ( p->momentum().z() - m_vct.z() ) ;    
    return de * de + dx * dx + dy * dy + dz * dz ;
  }
  Error("Invalid HepMC::GenParticle*, return (InvalidMomentum)^2") ; 
  return 
    LoKi::Constants::InvalidMomentum * 
    LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::MomentumDistance::fillStream 
( std::ostream& s ) const { return s << "GMOMDIST" ; };
// ============================================================================

// ============================================================================
/** constructor 
 *  @param cut    predicate to be used for counting
 *  @param range  "iterator range", @see HepMC::IteratorRange
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenParticles::NInTree::NInTree 
( const LoKi::GenParticles::NInTree::GCut&  cut   , 
  HepMC::IteratorRange                      range ) 
  : LoKi::Function<const HepMC::GenParticle*>()
  , m_cut( cut )
  , m_range ( range )
{};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::NInTree::NInTree 
( const LoKi::GenParticles::NInTree& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right )
  , m_cut   ( right.m_cut )
  , m_range ( right.m_range )
{};
// ============================================================================

// ============================================================================
/// MANDATORY: virtual contructor 
// ============================================================================
LoKi::GenParticles::NInTree::~NInTree(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::GenParticles::NInTree*
LoKi::GenParticles::NInTree::clone() const 
{ return new LoKi::GenParticles::NInTree( *this ) ; }
// ============================================================================

// ============================================================================
/** count the particles in the tree according 
 *  the predicat eand iterator range 
 *  @see HepMC::GenVertex
 *  @param vertex  root of the tree 
 *  @return number of particles 
 */
// ============================================================================
size_t LoKi::GenParticles::NInTree::count
( HepMC::GenVertex* vertex ) const 
{
  if ( 0 == vertex ) { return 0 ; }
  return std::count_if ( vertex->particles_begin( m_range ) , 
                         vertex->particles_end  ( m_range ) , m_cut ) ;
};
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::NInTree::result_type 
LoKi::GenParticles::NInTree::operator() 
  ( LoKi::GenParticles::NInTree::argument p ) const
{
  if ( 0 == p ) 
  { 
    Error ( "HepMC::GenParticle* points to NULL, return 0" ) ;
    return 0 ;                                                    // RETURN 
  };
  if      ( HepMC::parents  == m_range || HepMC::ancestors   == m_range ) 
  { return count ( p -> production_vertex() ) ; }  
  else if ( HepMC::children == m_range || HepMC::descendants == m_range )
  { return count ( p -> end_vertex       () ) ; }
  
  Error ("Unimplemented case: HepMC::IteratorRange, return 0 ;") ;
  
  return 0 ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::NInTree::fillStream( std::ostream& s ) const 
{ return s << "GNINTREE" ; }
// ============================================================================

// ============================================================================
/** constructor
 *  @param vct direction vector 
 *  @see LoKi::ThreeVector 
 */
// ============================================================================
LoKi::GenParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::ThreeVector& vct ) 
  : LoKi::Function<const HepMC::GenParticle*>() 
  , m_vct ( vct ) 
{};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::TransverseMomentumRel::TransverseMomentumRel 
( const LoKi::GenParticles::TransverseMomentumRel& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right ) 
  , m_vct ( right.m_vct ) 
{};
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::TransverseMomentumRel::~TransverseMomentumRel(){};
// ============================================================================

// ============================================================================
/// MANDATORY: clone function ("virtual constructor")
// ============================================================================
LoKi::GenParticles::TransverseMomentumRel*
LoKi::GenParticles::TransverseMomentumRel::clone() const
{ return new LoKi::GenParticles::TransverseMomentumRel(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::TransverseMomentumRel::result_type 
LoKi::GenParticles::TransverseMomentumRel::operator() 
  ( LoKi::GenParticles::TransverseMomentumRel::argument p ) const
{
  if ( 0 != p ) 
  {
    const double b2 = m_vct.mag2() ;
    const LoKi::ThreeVector va 
      ( p -> momentum().x() , 
        p -> momentum().y() , 
        p -> momentum().z() ) ;
    if ( 0 == b2 ) { return ::sqrt( va.mag2() ) ; }
    const double ab = va.Dot( m_vct ) / b2 ;
    const LoKi::ThreeVector vd = va - m_vct * ab ;
    return ::sqrt( vd.mag2() ) ; 
  }
  Error("Invalid HepMC::GenParticle*, return 'InvalidMomentum'") ; 
  return LoKi::Constants::InvalidMomentum ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::TransverseMomentumRel::fillStream
( std::ostream& s ) const { return s << "GPTDIR" ; }
// ============================================================================

// ============================================================================
/** constructor from particle ("head")
 *  @param p pointer to HepMC::GenParticle 
 */
// ============================================================================
LoKi::GenParticles::FromHepMCTree::FromHepMCTree 
( const HepMC::GenParticle* p ) 
  : LoKi::Predicate<const HepMC::GenParticle*> ()
  , m_vertices () { _add ( p ) ; }
// ============================================================================

// ============================================================================
/** constructor from vertex  ("head")
 *  @param v pointer to HepMC::GenParticle 
 */
// ============================================================================
LoKi::GenParticles::FromHepMCTree::FromHepMCTree 
( const HepMC::GenVertex* v ) 
  : LoKi::Predicate<const HepMC::GenParticle*> ()
  , m_vertices() { _add ( v ) ; } ;
// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::FromHepMCTree::FromHepMCTree 
( const LoKi::GenParticles::FromHepMCTree& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Predicate<const HepMC::GenParticle*> ( right )
  , m_vertices( right.m_vertices ) 
{};  
// ============================================================================
void LoKi::GenParticles::FromHepMCTree::_add 
( const HepMC::GenParticle* p ) 
{ if ( 0 != p ) { _add ( p->end_vertex() ) ; } }
void LoKi::GenParticles::FromHepMCTree::_add 
( const HepMC::GenVertex*   v ) 
{
  if ( 0 != v ) 
  { m_vertices.push_back( const_cast<HepMC::GenVertex*> ( v ) ) ; }
}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::FromHepMCTree*
LoKi::GenParticles::FromHepMCTree::clone() const 
{ return new LoKi::GenParticles::FromHepMCTree(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::FromHepMCTree::result_type 
LoKi::GenParticles::FromHepMCTree::operator() 
  ( LoKi::GenParticles::FromHepMCTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " HepMC::GenParticle* points to NULL, return 'false' " ) ;
    return false ;                                                  // RETURN 
  } 
  if ( m_vertices.empty() ) 
  {
    Error ( " empty list of vertices , return 'false' " ) ;
    return false ;                                                  // RETURN 
  }
  // loop over vertices 
  for ( VERTICES::const_iterator iv = m_vertices.begin() ; 
        m_vertices.end() != iv ; ++iv ) 
  {
    HepMC::GenVertex* vertex = *iv ;
    if ( 0 == vertex ) { Warning("NULL vertex is detected, skip") ; continue ; }
    // explict loop over all descendants 
    HepMC::GenVertex::particle_iterator begin = 
      vertex->particles_begin ( HepMC::descendants ) ;
    HepMC::GenVertex::particle_iterator end   = 
      vertex->particles_end   ( HepMC::descendants ) ;
    if ( std::find ( begin , end , p ) != end ) { return true ; }    // RETURN 
    //
  }
  return false ;                                                    // RETURN 
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::FromHepMCTree::fillStream
( std::ostream& s ) const { return s << "GFROMTREE" ; }
// ============================================================================

// ============================================================================
/** constructor from particle ("head")
 *  @param p pointer to HepMC::GenParticle 
 */
// ============================================================================
LoKi::GenParticles::IsAnAncestor::IsAnAncestor
( const HepMC::GenParticle* p ) 
  : LoKi::Predicate<const HepMC::GenParticle*> () 
  , m_particle ( p ) 
{}
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::IsAnAncestor::IsAnAncestor
( const LoKi::GenParticles::IsAnAncestor& right )
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Predicate<const HepMC::GenParticle*> ( right ) 
  , m_particle ( right.m_particle  ) 
{}
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::IsAnAncestor::~IsAnAncestor(){}
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::IsAnAncestor*
LoKi::GenParticles::IsAnAncestor::clone() const 
{ return new LoKi::GenParticles::IsAnAncestor(*this) ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::IsAnAncestor::result_type
LoKi::GenParticles::IsAnAncestor::operator() 
  ( LoKi::GenParticles::IsAnAncestor::argument p ) const 
{
  if ( 0 == p || 0 == m_particle ) 
  {
    Error ( " HepMC::GenParticle* points to NULL, return 'false' " ) ;
    return false ;                                                  // RETURN 
  } 
  HepMC::GenVertex* vertex = p->end_vertex() ;
  if ( 0 == vertex ) { return false ; }                             // RETURN  
  // explict loop over all descendants 
  for ( HepMC::GenVertex::particle_iterator ip = 
          vertex->particles_begin( HepMC::descendants ) ; 
        vertex->particles_end( HepMC::descendants ) != ip ; ++ip )
  { if ( m_particle == (*ip) ) { return true ; } }                 // RETURN
  
  return false ;
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::IsAnAncestor::fillStream
( std::ostream& s ) const { return s << "GANCESTOR" ; }
// ============================================================================

// ============================================================================
/// constructor from Quark 
// ============================================================================
LoKi::GenParticles::HasQuark::HasQuark ( const LHCb::ParticleID::Quark quark ) 
  : LoKi::Predicate<const HepMC::GenParticle*>() 
  , m_quark ( quark  )
{};
// ============================================================================
LoKi::GenParticles::HasQuark::HasQuark 
( const LoKi::GenParticles::HasQuark& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Predicate<const HepMC::GenParticle*>( right ) 
  , m_quark ( right.m_quark  )
{};
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::HasQuark::result_type 
LoKi::GenParticles::HasQuark::operator() 
  ( const LoKi::GenParticles::HasQuark::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ("HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p -> pdg_id() ).hasQuark( quark() ) ;
};
// ============================================================================
LoKi::GenParticles::HasQuark*
LoKi::GenParticles::HasQuark::clone() const 
{ return new LoKi::GenParticles::HasQuark(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::HasQuark::fillStream
( std::ostream& s ) const 
{
  switch ( quark() ) 
  {
  case LHCb::ParticleID::top      :
    return s << "GTOP"     ;
    break                  ;
  case LHCb::ParticleID::bottom   :
    return s << "GBEAUTY"  ;
    break                  ;
  case LHCb::ParticleID::charm    :
    return s << "GCHARM"   ;
    break                  ;
  case LHCb::ParticleID::strange  :
    return s << "GSTRANGE" ;
    break                  ;
  default                  :
    return s << "GQUARK[" << quark() << "]" ;
    break                  ;
  }
  //
  return s ; 
};
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsCharged::result_type 
LoKi::GenParticles::IsCharged::operator() 
  ( const LoKi::GenParticles::IsCharged::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 != LHCb::ParticleID( p->pdg_id() ) .threeCharge() ;
};
// ============================================================================
LoKi::GenParticles::IsCharged*
LoKi::GenParticles::IsCharged::clone() const 
{ return new LoKi::GenParticles::IsCharged(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsCharged::fillStream
( std::ostream& s ) const 
{ return s << "GCHARGED" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsNeutral::result_type 
LoKi::GenParticles::IsNeutral::operator() 
  ( const LoKi::GenParticles::IsNeutral::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return 0 == LHCb::ParticleID( p->pdg_id() ) .threeCharge() ;
};
// ============================================================================
LoKi::GenParticles::IsNeutral*
LoKi::GenParticles::IsNeutral::clone() const 
{ return new LoKi::GenParticles::IsNeutral(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsNeutral::fillStream
( std::ostream& s ) const 
{ return s << "GNEUTRAL" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsLepton::result_type 
LoKi::GenParticles::IsLepton::operator() 
  ( const LoKi::GenParticles::IsLepton::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p->pdg_id() ) .isLepton() ;
};
// ============================================================================
LoKi::GenParticles::IsLepton*
LoKi::GenParticles::IsLepton::clone() const 
{ return new LoKi::GenParticles::IsLepton(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsLepton::fillStream
( std::ostream& s ) const 
{ return s << "GLEPTON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsMeson::result_type 
LoKi::GenParticles::IsMeson::operator() 
  ( const LoKi::GenParticles::IsMeson::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p->pdg_id() ) .isMeson() ;
};
// ============================================================================
LoKi::GenParticles::IsMeson*
LoKi::GenParticles::IsMeson::clone() const 
{ return new LoKi::GenParticles::IsMeson(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsMeson::fillStream
( std::ostream& s ) const 
{ return s << "GMESON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsBaryon::result_type 
LoKi::GenParticles::IsBaryon::operator() 
  ( const LoKi::GenParticles::IsBaryon::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p->pdg_id() ) .isBaryon() ;
};
// ============================================================================
LoKi::GenParticles::IsBaryon*
LoKi::GenParticles::IsBaryon::clone() const 
{ return new LoKi::GenParticles::IsBaryon(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsBaryon::fillStream
( std::ostream& s ) const 
{ return s << "GBARYON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsHadron::result_type 
LoKi::GenParticles::IsHadron::operator() 
  ( const LoKi::GenParticles::IsHadron::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p->pdg_id() ) .isHadron() ;
};
// ============================================================================
LoKi::GenParticles::IsHadron*
LoKi::GenParticles::IsHadron::clone() const 
{ return new LoKi::GenParticles::IsHadron(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsHadron::fillStream
( std::ostream& s ) const 
{ return s << "GHADRON" ; }
// ============================================================================

// ============================================================================
// the only one essential method 
// ============================================================================
LoKi::GenParticles::IsNucleus::result_type 
LoKi::GenParticles::IsNucleus::operator() 
  ( const LoKi::GenParticles::IsNucleus::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("HepMC::GenParticle* point to NULL, returs 'false'");
    return false ;
  }
  return LHCb::ParticleID( p->pdg_id() ) .isNucleus() ;
};
// ============================================================================
LoKi::GenParticles::IsNucleus*
LoKi::GenParticles::IsNucleus::clone() const 
{ return new LoKi::GenParticles::IsNucleus(*this) ; }
// ============================================================================

// ============================================================================
std::ostream&
LoKi::GenParticles::IsNucleus::fillStream
( std::ostream& s ) const 
{ return s << "GNUCLEUS" ; }
// ============================================================================

// ============================================================================
LoKi::GenParticles::ProperLifeTime::ProperLifeTime
( const double bad ) 
  : LoKi::Function<const HepMC::GenParticle*>() 
  , m_bad ( bad ) 
{} ;
// ============================================================================
LoKi::GenParticles::ProperLifeTime::ProperLifeTime
( const LoKi::GenParticles::ProperLifeTime& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right )
  , m_bad ( right.m_bad ) {}
// ============================================================================
LoKi::GenParticles::ProperLifeTime::~ProperLifeTime(){} ;
// ============================================================================
LoKi::GenParticles::ProperLifeTime*
LoKi::GenParticles::ProperLifeTime::clone() const 
{ return new LoKi::GenParticles::ProperLifeTime(*this) ; }
// ============================================================================
std::ostream& 
LoKi::GenParticles::ProperLifeTime::fillStream 
( std::ostream& s ) const  
{ return s << "GTIME" ; }
// ============================================================================
LoKi::GenParticles::ProperLifeTime::result_type 
LoKi::GenParticles::ProperLifeTime::operator() 
  ( LoKi::GenParticles::ProperLifeTime::argument p ) const 
{  
  if ( 0 == p ) 
  {
    Error(" Invalid HepMC::GenParticle, return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  // end-vertex 
  const HepMC::GenVertex* end = p->end_vertex() ;
  if ( 0 == end ) { return m_bad ; } 
  // production vertex 
  const HepMC::GenVertex* origin = p->production_vertex() ;
  if ( 0 == origin ) 
  {
    Error(" Invalid production_vertex, return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  const double dist = 
    ( LoKi::ThreeVector ( end    -> position() ) - 
      LoKi::ThreeVector ( origin -> position() ) ) .R() ;
  const double mass = 
    LoKi::LorentzVector( p->momentum() ) .M() ;
  const double mom   = 
    LoKi::LorentzVector( p->momentum() ) .P() ;
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


// ============================================================================
LoKi::GenParticles::NominalLifeTime* 
LoKi::GenParticles::NominalLifeTime::clone() const 
{ return new LoKi::GenParticles::NominalLifeTime ( *this ) ; }
// ============================================================================
LoKi::GenParticles::NominalLifeTime::result_type 
LoKi::GenParticles::NominalLifeTime::operator() 
  ( LoKi::GenParticles::NominalLifeTime::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Invalid HepMC::GenParticle, return 'InvalidTime'");
    return LoKi::Constants::InvalidTime;                     // RETURN 
  }
  return LoKi::Particles::lifeTime ( LHCb::ParticleID ( p->pdg_id() ) ) ;
};
// ============================================================================
std::ostream& 
LoKi::GenParticles::NominalLifeTime::fillStream 
( std::ostream& s ) const  
{ return s << "GNLT" ; }
// ============================================================================

// ============================================================================
/** constructor from vertex function and "bad" value 
 *  @param fun verted function to be used 
 *  @param bad the value to be returnedd for invalid vertex 
 */
// ============================================================================
LoKi::GenParticles::AdapterToProductionVertex::AdapterToProductionVertex 
( const LoKi::Types::GVFunc& fun , 
  const double               bad ) 
  : LoKi::Function<const HepMC::GenParticle*>() 
  , m_fun ( fun ) 
  , m_bad ( bad ) 
{};
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::GenParticles::AdapterToProductionVertex::AdapterToProductionVertex 
( const LoKi::GenParticles::AdapterToProductionVertex& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right ) 
  , m_fun ( right.m_fun ) 
  , m_bad ( right.m_bad ) 
{};
// ============================================================================
/// destructor 
// ============================================================================
LoKi::GenParticles::AdapterToProductionVertex::~AdapterToProductionVertex(){};
// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::AdapterToProductionVertex*
LoKi::GenParticles::AdapterToProductionVertex::clone() const
{ return new AdapterToProductionVertex(*this); }
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenParticles::AdapterToProductionVertex::fillStream
( std::ostream& stream ) const 
{ return stream << "GFAPVX[" << m_fun << "]" ; }
// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::AdapterToProductionVertex::result_type 
LoKi::GenParticles::AdapterToProductionVertex::operator() 
  ( LoKi::GenParticles::AdapterToProductionVertex::argument p ) const 
{
  if ( 0 == p ) { return m_bad ; }
  HepMC::GenVertex* v = p->production_vertex() ;
  if ( 0 == v ) { return m_bad ; }
  return m_fun ( v ) ;
} ;
// ============================================================================

// ============================================================================
/** constructor from vertex function and "bad" value 
 *  @param fun verted function to be used 
 *  @param bad the value to be returnedd for invalid vertex 
 */
// ============================================================================
LoKi::GenParticles::AdapterToEndVertex::AdapterToEndVertex 
( const LoKi::Types::GVFunc& fun , 
  const double               bad ) 
  : LoKi::Function<const HepMC::GenParticle*>() 
  , m_fun ( fun ) 
  , m_bad ( bad ) 
{};
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::GenParticles::AdapterToEndVertex::AdapterToEndVertex 
( const LoKi::GenParticles::AdapterToEndVertex& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Function<const HepMC::GenParticle*>( right ) 
  , m_fun ( right.m_fun ) 
  , m_bad ( right.m_bad ) 
{};
// ============================================================================
/// destructor 
// ============================================================================
LoKi::GenParticles::AdapterToEndVertex::~AdapterToEndVertex(){};
// ============================================================================
/// clone method (mandatory!)
// ============================================================================
LoKi::GenParticles::AdapterToEndVertex*
LoKi::GenParticles::AdapterToEndVertex::clone() const
{ return new AdapterToEndVertex(*this); }
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenParticles::AdapterToEndVertex::fillStream
( std::ostream& stream ) const 
{ return stream << "GFAEVX[" << m_fun << "]" ; }
// ============================================================================
/// the only one essential method 
// ============================================================================
LoKi::GenParticles::AdapterToEndVertex::result_type 
LoKi::GenParticles::AdapterToEndVertex::operator() 
  ( LoKi::GenParticles::AdapterToEndVertex::argument p ) const 
{
  if ( 0 == p ) { return m_bad ; }
  HepMC::GenVertex* v = p->end_vertex() ;
  if ( 0 == v ) { return m_bad ; }
  return m_fun ( v ) ;
} ;
// ============================================================================


// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::ThreeCharge::result_type 
LoKi::GenParticles::ThreeCharge::operator() 
  ( LoKi::GenParticles::ThreeCharge::argument p ) const
{
  if ( 0 == p )
  {
    Error(" Invalid Particle, return 'InvalidCharge'");
    return LoKi::Constants::InvalidCharge;                     // RETURN 
  }
  return LHCb::ParticleID( p->pdg_id() ).threeCharge() ;  
};
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenParticles::ThreeCharge::fillStream
( std::ostream& stream ) const 
{ return stream << "G3Q" ; }

// ============================================================================
// The END 
// ============================================================================
