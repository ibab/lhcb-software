// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/GenParticles2.h"
#include "LoKi/GenChild.h"
#include "LoKi/Trees.h"
#include "LoKi/IGenDecay.h"
// ============================================================================
/** @file 
 *  Implementation file for functirons from file  LoKi/GenParticles2.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vaya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-20
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructor from child-selectors 
// ============================================================================
LoKi::GenParticles::PolarizationAngle::PolarizationAngle
( const LoKi::GenChild::Selector& daughter     , 
  const LoKi::GenChild::Selector& parent       ,      
  const bool                     mother       )
  : LoKi::GenTypes::GFunc  () 
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::GenParticles::PolarizationAngle::PolarizationAngle
( const Decays::IGenDecay::iTree& daughter , 
  const Decays::IGenDecay::iTree& parent   , 
  const bool                   mother   )
  : LoKi::GenTypes::GFunc  () 
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// constructor from two nodes
// ============================================================================
LoKi::GenParticles::PolarizationAngle::PolarizationAngle
( const Decays::iNode&         daughter  , 
  const Decays::iNode&         parent    , 
  const bool                   mother    )
  : LoKi::GenTypes::GFunc  () 
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// constructor from two cuts 
// ============================================================================
LoKi::GenParticles::PolarizationAngle::PolarizationAngle
( const LoKi::GenTypes::GCuts& daughter  , 
  const LoKi::GenTypes::GCuts& parent    ,
  const bool                   mother    )
  : LoKi::GenTypes::GFunc  () 
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::GenParticles::PolarizationAngle::PolarizationAngle
( const std::string&           daughter     , 
  const std::string&           parent       ,      
  const bool                   mother       , 
  const std::string&           factory      ) 
  : LoKi::GenTypes::GFunc  () 
  , m_first        ( daughter , factory )
  , m_second       ( parent   , factory )
  , m_mother       ( mother       ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::PolarizationAngle::~PolarizationAngle() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::PolarizationAngle*
LoKi::GenParticles::PolarizationAngle::clone() const 
{ return new LoKi::GenParticles::PolarizationAngle ( *this ) ; }
// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode 
LoKi::GenParticles::PolarizationAngle::getComponents12
( const HepMC::GenParticle*             p   , 
  Decays::IGenDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
  //
  const HepMC::GenParticle* c1 = m_first.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid first  child : '" + m_first.printOut  () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const HepMC::GenParticle* c2 = m_second.child ( p ) ;
  if ( 0 == c2 )
  {
    Error ( "Invalid second child : '" + m_second.printOut () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c2 ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ==========================================================================
// MANDATORY: the only one essential method
// ==========================================================================
LoKi::GenParticles::PolarizationAngle::result_type 
LoKi::GenParticles::PolarizationAngle::operator() 
  ( LoKi::GenParticles::PolarizationAngle::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Decays::IGenDecay::iTree::Collection vct ;
  vct.reserve ( 2 ) ;
  StatusCode sc = getComponents12 ( p , vct );
  
  if ( sc.isFailure() )
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'" , sc ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 2 != vct.size() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'" , sc ) ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  return m_mother ? 
    LoKi::Kinematics::decayAngle 
    ( LoKi::LorentzVector ( vct [0] -> momentum () ) , 
      LoKi::LorentzVector ( vct [1] -> momentum () ) , 
      LoKi::LorentzVector ( p       -> momentum () ) ) : 
    LoKi::Kinematics::decayAngle 
    ( LoKi::LorentzVector ( vct [0] -> momentum () ) , 
      LoKi::LorentzVector ( vct [0] -> momentum () ) + 
      LoKi::LorentzVector ( vct [1] -> momentum () ) , 
      LoKi::LorentzVector ( p       -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::GenParticles::PolarizationAngle::fillStream 
( std::ostream& s ) const 
{
  s << " GCOSPOL( " << m_first << " , " << m_second ;
  //
  if  ( !m_mother ) 
  { s << " , " ; Gaudi::Utils::toStream ( m_mother , s ) ; }
  //
  return s << " ) " ;
}



// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::SinChi::SinChi
( const LoKi::GenChild::Selector& particle1 ,  
  const LoKi::GenChild::Selector& particle2 , 
  const LoKi::GenChild::Selector& particle3 , 
  const LoKi::GenChild::Selector& particle4 ) 
  : LoKi::GenParticles::PolarizationAngle ( particle1    , 
                                            particle2    , 
                                            false        )
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{
  //
  Assert ( m_tree3.valid () , "The third tree is invalid" ) ;
  Assert ( m_tree4.valid () , "The fourh tree is invalid" ) ;
  //
} 
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::SinChi::SinChi
( const Decays::IGenDecay::iTree& particle1    , 
  const Decays::IGenDecay::iTree& particle2    , 
  const Decays::IGenDecay::iTree& particle3    , 
  const Decays::IGenDecay::iTree& particle4    )
  : LoKi::GenParticles::PolarizationAngle ( particle1    , 
                                            particle2    , 
                                            false        )
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{
  //
  Assert ( m_tree3.valid () , "The third tree is invalid" ) ;
  Assert ( m_tree4.valid () , "The fourh tree is invalid" ) ;
  //
}
// ============================================================================
// constructor from the nodes 
// ============================================================================
LoKi::GenParticles::SinChi::SinChi
( const Decays::iNode& particle1    , 
  const Decays::iNode& particle2    , 
  const Decays::iNode& particle3    , 
  const Decays::iNode& particle4    ) 
  : LoKi::GenParticles::PolarizationAngle ( particle1    , 
                                            particle2    , 
                                            false        )
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{
  //
  Assert ( m_tree3.valid () , "The third tree is invalid" ) ;
  Assert ( m_tree4.valid () , "The fourh tree is invalid" ) ;
  //
} 
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::GenParticles::SinChi::SinChi
( const LoKi::GenTypes::GCuts& particle1 , 
  const LoKi::GenTypes::GCuts& particle2 , 
  const LoKi::GenTypes::GCuts& particle3 , 
  const LoKi::GenTypes::GCuts& particle4 )
  : LoKi::GenParticles::PolarizationAngle ( particle1    , 
                                            particle2    , 
                                            false        )  
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{
  //
  Assert ( m_tree3.valid () , "The third tree is invalid" ) ;
  Assert ( m_tree4.valid () , "The fourh tree is invalid" ) ;
  //
} 
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::SinChi::SinChi
( const std::string& particle1    , 
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::GenParticles::PolarizationAngle ( particle1    , 
                                            particle2    , 
                                            false        ,
                                            factory      )
  , m_tree3   ( particle3 , factory ) 
  , m_tree4   ( particle4 , factory )
{
  //
  Assert ( m_tree3.valid () , "The third tree is invalid" ) ;
  Assert ( m_tree4.valid () , "The fourh tree is invalid" ) ;
  //
}
// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode LoKi::GenParticles::SinChi::getComponents34
( const HepMC::GenParticle*              p   , 
  Decays::IGenDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::GenParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Assert ( m_tree3 . valid () , "The third  tree is invalid!" ) ;
  Assert ( m_tree4 . valid () , "The fourth tree is invalid!" ) ;
  //
  const HepMC::GenParticle* c1 = m_tree3.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid third  child : '" + m_tree3.printOut () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const HepMC::GenParticle* c2 = m_tree4.child ( p ) ;
  if ( 0 == c2 )
  {
    Error ( "Invalid fourth child : '" + m_tree4.printOut () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c2 ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode LoKi::GenParticles::SinChi::getComponents
( const HepMC::GenParticle*              p   , 
  Decays::IGenDecay::iTree::Collection& vct ) const
{
  vct.clear() ;
  StatusCode sc1 = getComponents12  ( p , vct ) ;
  if ( sc1.isFailure() ) 
  {
    Error ( "Unable to pickup 12-components" , sc1 ) ;
    return sc1 ;
  }
  //
  StatusCode sc2 = getComponents34  ( p , vct ) ;
  if ( sc2.isFailure() ) 
  {
    Error ( "Unable to pickup 34-components" , sc2 ) ;
    return sc2 ;
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::SinChi::~SinChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::SinChi*
LoKi::GenParticles::SinChi::clone() const 
{ return new LoKi::GenParticles::SinChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::SinChi::result_type 
LoKi::GenParticles::SinChi::operator() 
  ( LoKi::GenParticles::SinChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ;  
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the angle chi: 
  return LoKi::Kinematics::sinDecayAngleChi 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::SinChi::fillStream  ( std::ostream& s ) const 
{
  s << " GSINCHI( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4  () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================


// ============================================================================
// constructor from selectors 
// ============================================================================
LoKi::GenParticles::CosChi::CosChi 
( const LoKi::GenChild::Selector& particle1 ,  
  const LoKi::GenChild::Selector& particle2 , 
  const LoKi::GenChild::Selector& particle3 , 
  const LoKi::GenChild::Selector& particle4 ) 
  : LoKi::GenParticles::SinChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 )
{}
// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::GenParticles::CosChi::CosChi 
( const Decays::IGenDecay::iTree& particle1 ,  
  const Decays::IGenDecay::iTree& particle2 , 
  const Decays::IGenDecay::iTree& particle3 , 
  const Decays::IGenDecay::iTree& particle4 )
  : LoKi::GenParticles::SinChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::CosChi::CosChi 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 ) 
  : LoKi::GenParticles::SinChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 )  
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::CosChi::CosChi 
( const LoKi::GenTypes::GCuts&   particle1 ,  
  const LoKi::GenTypes::GCuts&   particle2 , 
  const LoKi::GenTypes::GCuts&   particle3 , 
  const LoKi::GenTypes::GCuts&   particle4 ) 
  : LoKi::GenParticles::SinChi ( particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::CosChi::CosChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::GenParticles::SinChi ( particle1    , 
                                 particle2    , 
                                 particle3    , 
                                 particle4    , 
                                 factory      )
{}
// ============================================================================
// MANDATORY: virtualdestructor
// ============================================================================
LoKi::GenParticles::CosChi::~CosChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::CosChi*
LoKi::GenParticles::CosChi::clone() const 
{ return new LoKi::GenParticles::CosChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::CosChi::result_type 
LoKi::GenParticles::CosChi::operator() 
  ( LoKi::GenParticles::CosChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the angle chi: 
  return LoKi::Kinematics::cosDecayAngleChi 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::CosChi::fillStream  ( std::ostream& s ) const 
{
  s << " GCOSCHI( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================


// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::AngleChi::AngleChi 
( const LoKi::GenChild::Selector& particle1 ,  
  const LoKi::GenChild::Selector& particle2 , 
  const LoKi::GenChild::Selector& particle3 , 
  const LoKi::GenChild::Selector& particle4 ) 
  : LoKi::GenParticles::CosChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::AngleChi::AngleChi 
( const Decays::IGenDecay::iTree& particle1 ,  
  const Decays::IGenDecay::iTree& particle2 , 
  const Decays::IGenDecay::iTree& particle3 , 
  const Decays::IGenDecay::iTree& particle4 )
  : LoKi::GenParticles::CosChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::AngleChi::AngleChi 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 ) 
  : LoKi::GenParticles::CosChi (  particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) 
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::AngleChi::AngleChi 
( const LoKi::GenTypes::GCuts&   particle1 ,  
  const LoKi::GenTypes::GCuts&   particle2 , 
  const LoKi::GenTypes::GCuts&   particle3 , 
  const LoKi::GenTypes::GCuts&   particle4 ) 
  : LoKi::GenParticles::CosChi ( particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::AngleChi::AngleChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::GenParticles::CosChi ( particle1    , 
                                 particle2    , 
                                 particle3    , 
                                 particle4    , 
                                 factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::AngleChi::~AngleChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::AngleChi*
LoKi::GenParticles::AngleChi::clone() const 
{ return new LoKi::GenParticles::AngleChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::AngleChi::result_type 
LoKi::GenParticles::AngleChi::operator() 
  ( LoKi::GenParticles::AngleChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the angle chi: 
  return LoKi::Kinematics::decayAngleChi 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::AngleChi::fillStream  ( std::ostream& s ) const 
{
  s << " GANGLECHI( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================


// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::CosThetaTr::CosThetaTr 
( const LoKi::GenChild::Selector&  particle1 ,  
  const LoKi::GenChild::Selector&  particle2 , 
  const LoKi::GenChild::Selector&  particle3 , 
  const LoKi::GenChild::Selector&  particle4 ) 
  : LoKi::GenParticles::AngleChi ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::CosThetaTr::CosThetaTr 
( const Decays::IGenDecay::iTree&  particle1 ,  
  const Decays::IGenDecay::iTree&  particle2 , 
  const Decays::IGenDecay::iTree&  particle3 , 
  const Decays::IGenDecay::iTree&  particle4 )
  : LoKi::GenParticles::AngleChi ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::CosThetaTr::CosThetaTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 ) 
  : LoKi::GenParticles::AngleChi ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::CosThetaTr::CosThetaTr 
( const LoKi::GenTypes::GCuts&     particle1 ,  
  const LoKi::GenTypes::GCuts&     particle2 , 
  const LoKi::GenTypes::GCuts&     particle3 , 
  const LoKi::GenTypes::GCuts&     particle4 )
  : LoKi::GenParticles::AngleChi ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::CosThetaTr::CosThetaTr 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::GenParticles::AngleChi ( particle1    , 
                                   particle2    , 
                                   particle3    , 
                                   particle4    , 
                                   factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::CosThetaTr::~CosThetaTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::CosThetaTr*
LoKi::GenParticles::CosThetaTr::clone() const 
{ return new LoKi::GenParticles::CosThetaTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::CosThetaTr::result_type 
LoKi::GenParticles::CosThetaTr::operator() 
  ( LoKi::GenParticles::CosThetaTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the angle chi: 
  return LoKi::Kinematics::cosThetaTr 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::CosThetaTr::fillStream  ( std::ostream& s ) const 
{
  s << " GCOSTHETATR( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================

// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::SinPhiTr::SinPhiTr 
( const LoKi::GenChild::Selector&    particle1 ,  
  const LoKi::GenChild::Selector&    particle2 , 
  const LoKi::GenChild::Selector&    particle3 , 
  const LoKi::GenChild::Selector&    particle4 ) 
  : LoKi::GenParticles::CosThetaTr ( particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::SinPhiTr::SinPhiTr 
( const Decays::IGenDecay::iTree&    particle1 ,  
  const Decays::IGenDecay::iTree&    particle2 , 
  const Decays::IGenDecay::iTree&    particle3 , 
  const Decays::IGenDecay::iTree&    particle4 )
  : LoKi::GenParticles::CosThetaTr ( particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::SinPhiTr::SinPhiTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 ) 
  : LoKi::GenParticles::CosThetaTr ( particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::SinPhiTr::SinPhiTr 
( const LoKi::GenTypes::GCuts&       particle1 ,  
  const LoKi::GenTypes::GCuts&       particle2 , 
  const LoKi::GenTypes::GCuts&       particle3 , 
  const LoKi::GenTypes::GCuts&       particle4 )
  : LoKi::GenParticles::CosThetaTr ( particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::SinPhiTr::SinPhiTr 
( const std::string&                 particle1    ,   
  const std::string&                 particle2    , 
  const std::string&                 particle3    , 
  const std::string&                 particle4    , 
  const std::string&                 factory      ) 
  : LoKi::GenParticles::CosThetaTr ( particle1    , 
                                     particle2    , 
                                     particle3    , 
                                     particle4    , 
                                     factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::SinPhiTr::~SinPhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::SinPhiTr*
LoKi::GenParticles::SinPhiTr::clone() const 
{ return new LoKi::GenParticles::SinPhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::SinPhiTr::result_type 
LoKi::GenParticles::SinPhiTr::operator() 
  ( LoKi::GenParticles::SinPhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the angle chi: 
  return LoKi::Kinematics::sinPhiTr 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::SinPhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " GSINPHITR( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================

// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::CosPhiTr::CosPhiTr 
( const LoKi::GenChild::Selector&  particle1 ,  
  const LoKi::GenChild::Selector&  particle2 , 
  const LoKi::GenChild::Selector&  particle3 , 
  const LoKi::GenChild::Selector&  particle4 ) 
  : LoKi::GenParticles::SinPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::CosPhiTr::CosPhiTr 
( const Decays::IGenDecay::iTree&  particle1 ,  
  const Decays::IGenDecay::iTree&  particle2 , 
  const Decays::IGenDecay::iTree&  particle3 , 
  const Decays::IGenDecay::iTree&  particle4 )
  : LoKi::GenParticles::SinPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::CosPhiTr::CosPhiTr 
( const Decays::iNode&             particle1 ,  
  const Decays::iNode&             particle2 , 
  const Decays::iNode&             particle3 , 
  const Decays::iNode&             particle4 )
  : LoKi::GenParticles::SinPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::CosPhiTr::CosPhiTr 
( const LoKi::GenTypes::GCuts&     particle1 ,  
  const LoKi::GenTypes::GCuts&     particle2 , 
  const LoKi::GenTypes::GCuts&     particle3 , 
  const LoKi::GenTypes::GCuts&     particle4 )
  : LoKi::GenParticles::SinPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::CosPhiTr::CosPhiTr 
( const std::string&               particle1    ,   
  const std::string&               particle2    , 
  const std::string&               particle3    , 
  const std::string&               particle4    , 
  const std::string&               factory      ) 
  : LoKi::GenParticles::SinPhiTr ( particle1    , 
                                   particle2    , 
                                   particle3    , 
                                   particle4    , 
                                   factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::CosPhiTr::~CosPhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::CosPhiTr*
LoKi::GenParticles::CosPhiTr::clone() const 
{ return new LoKi::GenParticles::CosPhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::CosPhiTr::result_type 
LoKi::GenParticles::CosPhiTr::operator() 
  ( LoKi::GenParticles::CosPhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the cos(phi_tr)
  return LoKi::Kinematics::cosPhiTr 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::CosPhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " GCOSPHITR( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================


// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::GenParticles::AnglePhiTr::AnglePhiTr 
( const LoKi::GenChild::Selector&  particle1 ,  
  const LoKi::GenChild::Selector&  particle2 , 
  const LoKi::GenChild::Selector&  particle3 , 
  const LoKi::GenChild::Selector&  particle4 ) 
  : LoKi::GenParticles::CosPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::GenParticles::AnglePhiTr::AnglePhiTr 
( const Decays::IGenDecay::iTree&  particle1 ,  
  const Decays::IGenDecay::iTree&  particle2 , 
  const Decays::IGenDecay::iTree&  particle3 , 
  const Decays::IGenDecay::iTree&  particle4 )
  : LoKi::GenParticles::CosPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::GenParticles::AnglePhiTr::AnglePhiTr 
( const Decays::iNode&             particle1 ,  
  const Decays::iNode&             particle2 , 
  const Decays::iNode&             particle3 , 
  const Decays::iNode&             particle4 )
  : LoKi::GenParticles::CosPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 ) 
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::GenParticles::AnglePhiTr::AnglePhiTr 
( const LoKi::GenTypes::GCuts&     particle1 ,  
  const LoKi::GenTypes::GCuts&     particle2 , 
  const LoKi::GenTypes::GCuts&     particle3 , 
  const LoKi::GenTypes::GCuts&     particle4 )
  : LoKi::GenParticles::CosPhiTr ( particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::GenParticles::AnglePhiTr::AnglePhiTr 
( const std::string&               particle1    ,   
  const std::string&               particle2    , 
  const std::string&               particle3    , 
  const std::string&               particle4    , 
  const std::string&               factory      ) 
  : LoKi::GenParticles::CosPhiTr ( particle1    , 
                                   particle2    , 
                                   particle3    , 
                                   particle4    , 
                                  factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::AnglePhiTr::~AnglePhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::AnglePhiTr*
LoKi::GenParticles::AnglePhiTr::clone() const 
{ return new LoKi::GenParticles::AnglePhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::GenParticles::AnglePhiTr::result_type 
LoKi::GenParticles::AnglePhiTr::operator() 
  ( LoKi::GenParticles::AnglePhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IGenDecay::iTree::Collection daughters ;
  daughters.reserve ( 4 ) ;
  //
  StatusCode sc = getComponents ( p , daughters ) ; 
  //
  if ( sc.isFailure() ) 
  {
    Error ("Unable to get proper decay components (1), return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  if ( 4 != daughters.size() ) 
  {
    Error ("Unable to get proper decay components (2) , return 'InvalidAngle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  // finally evaluate the phi_tr
  return LoKi::Kinematics::anglePhiTr 
    ( LoKi::LorentzVector ( daughters [0] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [1] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [2] -> momentum () ) , 
      LoKi::LorentzVector ( daughters [3] -> momentum () ) ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::GenParticles::AnglePhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " GANGLEPHITR( " 
    <<   "'" << child1 () 
    << "','" << child2 ()  
    << "','" << child3 ()  
    << "','" << child4 () 
    <<   "'" ;
  //
  return s << " ) " ;  
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
