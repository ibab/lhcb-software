// $Id: Particles33.cpp,v 1.2 2010-05-30 17:11:02 ibelyaev Exp $
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
#include "LoKi/Particles33.h"
#include "LoKi/Child.h"
#include "LoKi/Trees.h"
#include "LoKi/IDecay.h"
// ============================================================================
/** @file 
 *  Implementation file for functirons from file  LoKi/Particles33.h
 *  @author Vaya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2010-02-21
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  // invalid decay
  const Decays::Trees::Types_<const LHCb::Particle*>::Invalid s_INVALID     ;
  // ==========================================================================
}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::Particles::PolarizationAngle::PolarizationAngle
( const Decays::IDecay::iTree& daughter , 
  const Decays::IDecay::iTree& parent   , 
  const bool                   mother   )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
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
LoKi::Particles::PolarizationAngle::PolarizationAngle
( const std::string&           daughter     , 
  const std::string&           parent       ,      
  const bool                   mother       , 
  const std::string&           factory      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_first        ( daughter , factory )
  , m_second       ( parent   , factory )
  , m_mother       ( mother       ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// constructor from chisl-selectors 
// ============================================================================
LoKi::Particles::PolarizationAngle::PolarizationAngle
( const LoKi::Child::Selector& daughter     , 
  const LoKi::Child::Selector& parent       ,      
  const bool                   mother       )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PolarizationAngle::~PolarizationAngle() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PolarizationAngle*
LoKi::Particles::PolarizationAngle::clone() const 
{ return new LoKi::Particles::PolarizationAngle ( *this ) ; }
// ============================================================================

// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode LoKi::Particles::PolarizationAngle::getComponents12
( const LHCb::Particle*              p   , 
  Decays::IDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
  //
  const LHCb::Particle* c1 = m_first.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid first  child : '" + m_first.printOut  () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const LHCb::Particle* c2 = m_second.child ( p ) ;
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
LoKi::Particles::PolarizationAngle::result_type 
LoKi::Particles::PolarizationAngle::operator() 
  ( LoKi::Particles::PolarizationAngle::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Decays::IDecay::iTree::Collection vct ;
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
    ( vct [0] -> momentum () , 
      vct [1] -> momentum () , 
      p       -> momentum () ) : 
    LoKi::Kinematics::decayAngle 
    ( vct [0] -> momentum () , 
      vct [0] -> momentum () + 
      vct [1] -> momentum () , 
      p       -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Particles::PolarizationAngle::fillStream 
( std::ostream& s ) const 
{
  s << " COSPOL( " << m_first << " , " << m_second ;
  //
  if  ( !m_mother ) 
  { s << " , " ; Gaudi::Utils::toStream ( m_mother , s ) ; }
  //
  return s << " ) " ;
}
// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::Particles::SinChi::SinChi
( const LoKi::Child::Selector& particle1 ,  
  const LoKi::Child::Selector& particle2 , 
  const LoKi::Child::Selector& particle3 , 
  const LoKi::Child::Selector& particle4 ) 
  : LoKi::Particles::PolarizationAngle ( particle1    , 
                                         particle2    , 
                                         true         )
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
LoKi::Particles::SinChi::SinChi
( const Decays::IDecay::iTree& particle1    , 
  const Decays::IDecay::iTree& particle2    , 
  const Decays::IDecay::iTree& particle3    , 
  const Decays::IDecay::iTree& particle4    )
  : LoKi::Particles::PolarizationAngle ( particle1    , 
                                         particle2    , 
                                         true         )
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
LoKi::Particles::SinChi::SinChi
( const std::string& particle1    , 
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::Particles::PolarizationAngle ( particle1    , 
                                         particle2    , 
                                         true         ,
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
StatusCode LoKi::Particles::SinChi::getComponents34
( const LHCb::Particle*              p   , 
  Decays::IDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Assert ( m_tree3 . valid () , "The third  tree is invalid!" ) ;
  Assert ( m_tree4 . valid () , "The fourth tree is invalid!" ) ;
  //
  const LHCb::Particle* c1 = m_tree3.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid third  child : '" + m_tree3.printOut () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const LHCb::Particle* c2 = m_tree4.child ( p ) ;
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
StatusCode LoKi::Particles::SinChi::getComponents
( const LHCb::Particle*              p   , 
  Decays::IDecay::iTree::Collection& vct ) const
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
LoKi::Particles::SinChi::~SinChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::SinChi*
LoKi::Particles::SinChi::clone() const 
{ return new LoKi::Particles::SinChi(*this); }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::SinChi::result_type 
LoKi::Particles::SinChi::operator() 
  ( LoKi::Particles::SinChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::SinChi::fillStream  ( std::ostream& s ) const 
{
  s << " SINCHI( " 
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
// constructor from child-selector 
// ============================================================================
LoKi::Particles::CosChi::CosChi 
( const LoKi::Child::Selector& particle1 ,  
  const LoKi::Child::Selector& particle2 , 
  const LoKi::Child::Selector& particle3 , 
  const LoKi::Child::Selector& particle4 ) 
  : LoKi::Particles::SinChi (  particle1 , 
                               particle2 , 
                               particle3 , 
                               particle4 )
{}
// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::Particles::CosChi::CosChi 
( const Decays::IDecay::iTree& particle1 ,  
  const Decays::IDecay::iTree& particle2 , 
  const Decays::IDecay::iTree& particle3 , 
  const Decays::IDecay::iTree& particle4 )
  : LoKi::Particles::SinChi (  particle1 , 
                               particle2 , 
                               particle3 , 
                               particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::Particles::CosChi::CosChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::Particles::SinChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              factory      )
{}
// ============================================================================
// MANDATORY: virtualdestructor
// ============================================================================
LoKi::Particles::CosChi::~CosChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::CosChi*
LoKi::Particles::CosChi::clone() const 
{ return new LoKi::Particles::CosChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::CosChi::result_type 
LoKi::Particles::CosChi::operator() 
  ( LoKi::Particles::CosChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::CosChi::fillStream  ( std::ostream& s ) const 
{
  s << " COSCHI( " 
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
LoKi::Particles::AngleChi::AngleChi 
( const LoKi::Child::Selector& particle1 ,  
  const LoKi::Child::Selector& particle2 , 
  const LoKi::Child::Selector& particle3 , 
  const LoKi::Child::Selector& particle4 ) 
  : LoKi::Particles::CosChi (  particle1 , 
                               particle2 , 
                               particle3 , 
                               particle4 )
{}
// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::Particles::AngleChi::AngleChi 
( const Decays::IDecay::iTree& particle1 ,  
  const Decays::IDecay::iTree& particle2 , 
  const Decays::IDecay::iTree& particle3 , 
  const Decays::IDecay::iTree& particle4 )
  : LoKi::Particles::CosChi (  particle1 , 
                               particle2 , 
                               particle3 , 
                               particle4 )
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::Particles::AngleChi::AngleChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::Particles::CosChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::AngleChi::~AngleChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::AngleChi*
LoKi::Particles::AngleChi::clone() const 
{ return new LoKi::Particles::AngleChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::AngleChi::result_type 
LoKi::Particles::AngleChi::operator() 
  ( LoKi::Particles::AngleChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::AngleChi::fillStream  ( std::ostream& s ) const 
  {
    s << " ANGLECHI( " 
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
