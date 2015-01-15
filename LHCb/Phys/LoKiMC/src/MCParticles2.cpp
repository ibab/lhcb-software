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
#include "LoKi/MCParticles2.h"
#include "LoKi/MCChild.h"
#include "LoKi/Trees.h"
#include "LoKi/IMCDecay.h"
// ============================================================================
/** @file 
 *  Implementation file for functirons from file  LoKi/MCParticles2.h
 *  @author Vaya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-05-31
 */
// ============================================================================
// constructor from child-selectors 
// ============================================================================
LoKi::MCParticles::PolarizationAngle::PolarizationAngle
( const LoKi::MCChild::Selector& daughter     , 
  const LoKi::MCChild::Selector& parent       ,      
  const bool                     mother       )
  : LoKi::AuxFunBase ( std::tie ( daughter , parent , mother ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::MCParticles::PolarizationAngle::PolarizationAngle
( const Decays::IMCDecay::iTree& daughter , 
  const Decays::IMCDecay::iTree& parent   , 
  const bool                   mother   )
  : LoKi::AuxFunBase ( std::tie ( daughter , parent , mother ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
{}
// ============================================================================
// constructor from two nodes
// ============================================================================
LoKi::MCParticles::PolarizationAngle::PolarizationAngle
( const Decays::iNode&         daughter  , 
  const Decays::iNode&         parent    , 
  const bool                   mother    , 
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( daughter , parent , mother , decayOnly ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_first        ( daughter , decayOnly )
  , m_second       ( parent   , decayOnly )
  , m_mother       ( mother   ) 
{}
// ============================================================================
// constructor from two cuts 
// ============================================================================
LoKi::MCParticles::PolarizationAngle::PolarizationAngle
( const LoKi::MCTypes::MCCuts& daughter  , 
  const LoKi::MCTypes::MCCuts& parent    ,
  const bool                   mother    , 
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( daughter , parent , mother , decayOnly ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_first        ( daughter , decayOnly )
  , m_second       ( parent   , decayOnly )
  , m_mother       ( mother   ) 
{}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::MCParticles::PolarizationAngle::PolarizationAngle
( const std::string&           daughter     , 
  const std::string&           parent       ,      
  const bool                   mother       , 
  const std::string&           factory      ) 
  : LoKi::AuxFunBase ( std::tie ( daughter , parent , mother , factory ) ) 
  , LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_first        ( daughter , factory )
  , m_second       ( parent   , factory )
  , m_mother       ( mother       ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::PolarizationAngle::~PolarizationAngle() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::PolarizationAngle*
LoKi::MCParticles::PolarizationAngle::clone() const 
{ return new LoKi::MCParticles::PolarizationAngle ( *this ) ; }
// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode 
LoKi::MCParticles::PolarizationAngle::getComponents12
( const LHCb::MCParticle*              p   , 
  Decays::IMCDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  if ( !m_first .valid () ) { m_first .validate() ; }
  if ( !m_second.valid () ) { m_second.validate() ; }
  //
  Assert ( m_first  .valid () , "The first  tree is invalid!" ) ;
  Assert ( m_second .valid () , "The second tree is invalid!" ) ;
  //
  const LHCb::MCParticle* c1 = m_first.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid first  child : '" + m_first.printOut  () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const LHCb::MCParticle* c2 = m_second.child ( p ) ;
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
LoKi::MCParticles::PolarizationAngle::result_type 
LoKi::MCParticles::PolarizationAngle::operator() 
  ( LoKi::MCParticles::PolarizationAngle::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  Decays::IMCDecay::iTree::Collection vct ;
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
std::ostream& LoKi::MCParticles::PolarizationAngle::fillStream 
( std::ostream& s ) const 
{
  s << " MCCOSPOL( " << m_first << " , " << m_second ;
  //
  if  ( !m_mother ) 
  { s << " , " ; Gaudi::Utils::toStream ( m_mother , s ) ; }
  //
  return s << " ) " ;
}
// ============================================================================
// constructor from child-selector 
// ============================================================================
LoKi::MCParticles::SinChi::SinChi
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::PolarizationAngle ( particle1    , 
                                           particle2    , 
                                           true         )
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{} 
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::SinChi::SinChi
( const Decays::IMCDecay::iTree& particle1    , 
  const Decays::IMCDecay::iTree& particle2    , 
  const Decays::IMCDecay::iTree& particle3    , 
  const Decays::IMCDecay::iTree& particle4    )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::PolarizationAngle ( particle1    , 
                                           particle2    , 
                                           true         )
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{} 
// ============================================================================
// constructor from the nodes 
// ============================================================================
LoKi::MCParticles::SinChi::SinChi
( const Decays::iNode& particle1    , 
  const Decays::iNode& particle2    , 
  const Decays::iNode& particle3    , 
  const Decays::iNode& particle4    , 
  const bool           decayOnly    ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::PolarizationAngle ( particle1    , 
                                           particle2    , 
                                           true         , 
                                           decayOnly    )
  , m_tree3   ( particle3 , decayOnly ) 
  , m_tree4   ( particle4 , decayOnly )
{} 
// ============================================================================
// constructor from the cuts 
// ============================================================================
LoKi::MCParticles::SinChi::SinChi
( const LoKi::MCTypes::MCCuts& particle1 , 
  const LoKi::MCTypes::MCCuts& particle2 , 
  const LoKi::MCTypes::MCCuts& particle3 , 
  const LoKi::MCTypes::MCCuts& particle4 ,
  const bool                   decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 , 
                                  decayOnly ) ) 
  , LoKi::MCParticles::PolarizationAngle ( particle1    , 
                                           particle2    , 
                                           true         , 
                                           decayOnly    )
  , m_tree3   ( particle3 , decayOnly ) 
  , m_tree4   ( particle4 , decayOnly )
{} 
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::SinChi::SinChi
( const std::string& particle1    , 
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 , 
                                  factory   ) ) 
  , LoKi::MCParticles::PolarizationAngle ( particle1    , 
                                           particle2    , 
                                           true         ,
                                           factory      )
  , m_tree3   ( particle3 , factory ) 
  , m_tree4   ( particle4 , factory )
{}
// ============================================================================
// get the proper decay components  
// ==========================================================================
StatusCode LoKi::MCParticles::SinChi::getComponents34
( const LHCb::MCParticle*              p   , 
  Decays::IMCDecay::iTree::Collection& vct ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::GenParticle* points to NULL") ;
    return StatusCode::FAILURE ;  //                                 RETURN 
  }
  //
  if ( !m_tree3.valid () ) { m_tree3.validate() ; }
  if ( !m_tree4.valid () ) { m_tree4.validate() ; }
  //
  Assert ( m_tree3 . valid () , "The third  tree is invalid!" ) ;
  Assert ( m_tree4 . valid () , "The fourth tree is invalid!" ) ;
  //
  const LHCb::MCParticle* c1 = m_tree3.child ( p ) ;
  if ( 0 == c1 )
  {
    Error ( "Invalid third  child : '" + m_tree3.printOut () + "'") ; 
    return StatusCode::FAILURE ;
  }
  vct.push_back ( c1 ) ;
  //
  const LHCb::MCParticle* c2 = m_tree4.child ( p ) ;
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
StatusCode LoKi::MCParticles::SinChi::getComponents
( const LHCb::MCParticle*              p   , 
  Decays::IMCDecay::iTree::Collection& vct ) const
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
LoKi::MCParticles::SinChi::~SinChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::SinChi*
LoKi::MCParticles::SinChi::clone() const 
{ return new LoKi::MCParticles::SinChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::SinChi::result_type 
LoKi::MCParticles::SinChi::operator() 
  ( LoKi::MCParticles::SinChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
LoKi::MCParticles::SinChi::fillStream  ( std::ostream& s ) const 
{
  s << " MCSINCHI( " 
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
LoKi::MCParticles::CosChi::CosChi 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::SinChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::MCParticles::CosChi::CosChi 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::SinChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::CosChi::CosChi 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::SinChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 , 
                                 decayOnly ) 
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::CosChi::CosChi 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::SinChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 , 
                                 decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::CosChi::CosChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::SinChi ( particle1    , 
                                particle2    , 
                                particle3    , 
                                particle4    , 
                                factory      )
{}
// ============================================================================
// MANDATORY: virtualdestructor
// ============================================================================
LoKi::MCParticles::CosChi::~CosChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::CosChi*
LoKi::MCParticles::CosChi::clone() const 
{ return new LoKi::MCParticles::CosChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::CosChi::result_type 
LoKi::MCParticles::CosChi::operator() 
  ( LoKi::MCParticles::CosChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
LoKi::MCParticles::CosChi::fillStream  ( std::ostream& s ) const 
{
  s << " MCCOSCHI( " 
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
LoKi::MCParticles::AngleChi::AngleChi 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::AngleChi::AngleChi 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::AngleChi::AngleChi 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 , 
                                 decayOnly )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::AngleChi::AngleChi 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosChi (  particle1 , 
                                 particle2 , 
                                 particle3 , 
                                 particle4 , 
                                 decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::AngleChi::AngleChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::CosChi ( particle1    , 
                                particle2    , 
                                particle3    , 
                                particle4    , 
                                factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::AngleChi::~AngleChi() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::AngleChi*
LoKi::MCParticles::AngleChi::clone() const 
{ return new LoKi::MCParticles::AngleChi ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::AngleChi::result_type 
LoKi::MCParticles::AngleChi::operator() 
  ( LoKi::MCParticles::AngleChi:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
LoKi::MCParticles::AngleChi::fillStream  ( std::ostream& s ) const 
{
  s << " MCANGLECHI( " 
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
LoKi::MCParticles::CosThetaTr::CosThetaTr 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::AngleChi (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::CosThetaTr::CosThetaTr 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::AngleChi (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::CosThetaTr::CosThetaTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::AngleChi (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::CosThetaTr::CosThetaTr 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::AngleChi (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::CosThetaTr::CosThetaTr 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::AngleChi ( particle1    , 
                                  particle2    , 
                                  particle3    , 
                                  particle4    , 
                                  factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::CosThetaTr::~CosThetaTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::CosThetaTr*
LoKi::MCParticles::CosThetaTr::clone() const 
{ return new LoKi::MCParticles::CosThetaTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::CosThetaTr::result_type 
LoKi::MCParticles::CosThetaTr::operator() 
  ( LoKi::MCParticles::CosThetaTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::MCParticles::CosThetaTr::fillStream  ( std::ostream& s ) const 
{
  s << " MCCOSTHETATR( " 
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
LoKi::MCParticles::SinPhiTr::SinPhiTr 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosThetaTr (  particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::SinPhiTr::SinPhiTr 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosThetaTr (  particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::SinPhiTr::SinPhiTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosThetaTr (  particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 , 
                                     decayOnly )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::SinPhiTr::SinPhiTr 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosThetaTr (  particle1 , 
                                     particle2 , 
                                     particle3 , 
                                     particle4 , 
                                     decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::SinPhiTr::SinPhiTr 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::CosThetaTr ( particle1    , 
                                    particle2    , 
                                    particle3    , 
                                    particle4    , 
                                    factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::SinPhiTr::~SinPhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::SinPhiTr*
LoKi::MCParticles::SinPhiTr::clone() const 
{ return new LoKi::MCParticles::SinPhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::SinPhiTr::result_type 
LoKi::MCParticles::SinPhiTr::operator() 
  ( LoKi::MCParticles::SinPhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::MCParticles::SinPhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " MCSINPHITR( " 
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
LoKi::MCParticles::CosPhiTr::CosPhiTr 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::SinPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::CosPhiTr::CosPhiTr 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::SinPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::CosPhiTr::CosPhiTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::SinPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::CosPhiTr::CosPhiTr 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::SinPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::CosPhiTr::CosPhiTr 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::SinPhiTr ( particle1    , 
                                  particle2    , 
                                  particle3    , 
                                  particle4    , 
                                  factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::CosPhiTr::~CosPhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::CosPhiTr*
LoKi::MCParticles::CosPhiTr::clone() const 
{ return new LoKi::MCParticles::CosPhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::CosPhiTr::result_type 
LoKi::MCParticles::CosPhiTr::operator() 
  ( LoKi::MCParticles::CosPhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::MCParticles::CosPhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " MCCOSPHITR( " 
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
LoKi::MCParticles::AnglePhiTr::AnglePhiTr 
( const LoKi::MCChild::Selector& particle1 ,  
  const LoKi::MCChild::Selector& particle2 , 
  const LoKi::MCChild::Selector& particle3 , 
  const LoKi::MCChild::Selector& particle4 ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::MCParticles::AnglePhiTr::AnglePhiTr 
( const Decays::IMCDecay::iTree& particle1 ,  
  const Decays::IMCDecay::iTree& particle2 , 
  const Decays::IMCDecay::iTree& particle3 , 
  const Decays::IMCDecay::iTree& particle4 )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ) ) 
  , LoKi::MCParticles::CosPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 )
{}
// ============================================================================
// constructor from the nodes
// ============================================================================
LoKi::MCParticles::AnglePhiTr::AnglePhiTr 
( const Decays::iNode& particle1 ,  
  const Decays::iNode& particle2 , 
  const Decays::iNode& particle3 , 
  const Decays::iNode& particle4 , 
  const bool           decayOnly )
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly )
{}
// ============================================================================
// constructor form the cuts 
// ============================================================================
LoKi::MCParticles::AnglePhiTr::AnglePhiTr 
( const LoKi::MCTypes::MCCuts&   particle1 ,  
  const LoKi::MCTypes::MCCuts&   particle2 , 
  const LoKi::MCTypes::MCCuts&   particle3 , 
  const LoKi::MCTypes::MCCuts&   particle4 , 
  const bool                     decayOnly ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  decayOnly ) ) 
  , LoKi::MCParticles::CosPhiTr (  particle1 , 
                                   particle2 , 
                                   particle3 , 
                                   particle4 , 
                                   decayOnly ) 
{}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::MCParticles::AnglePhiTr::AnglePhiTr 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const std::string& factory      ) 
  : LoKi::AuxFunBase ( std::tie ( particle1 , 
                                  particle2 , 
                                  particle3 , 
                                  particle4 ,
                                  factory   ) ) 
  , LoKi::MCParticles::CosPhiTr ( particle1    , 
                                  particle2    , 
                                  particle3    , 
                                  particle4    , 
                                  factory      )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::MCParticles::AnglePhiTr::~AnglePhiTr() {} 
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::MCParticles::AnglePhiTr*
LoKi::MCParticles::AnglePhiTr::clone() const 
{ return new LoKi::MCParticles::AnglePhiTr ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::MCParticles::AnglePhiTr::result_type 
LoKi::MCParticles::AnglePhiTr::operator() 
  ( LoKi::MCParticles::AnglePhiTr:: argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::MCParticle* points to NULL, return 'Invaild Angle'") ;
    return LoKi::Constants::InvalidAngle ;
  }
  //
  Decays::IMCDecay::iTree::Collection daughters ;
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
    ( daughters [0] -> momentum () , 
      daughters [1] -> momentum () , 
      daughters [2] -> momentum () , 
      daughters [3] -> momentum () ) ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::MCParticles::AnglePhiTr::fillStream  ( std::ostream& s ) const 
{
  s << " MCANGLEPHITR( " 
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

