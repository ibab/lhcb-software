// $Id: Particles33.cpp,v 1.1 2010-02-22 09:55:48 ibelyaev Exp $
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
  const bool                   mother   , 
  const bool                   validate ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_first        ( daughter )
  , m_second       ( parent   )
  , m_mother       ( mother   ) 
  , m_autovalidate ( validate ) 
  , m_factory () 
{
  Assert ( tree1().marked() , "The first  tree is non-marked!" ) ;
  Assert ( tree2().marked() , "The second tree is non-marked!" ) ;
}
// ============================================================================
// constructor from two trees 
// ============================================================================
LoKi::Particles::PolarizationAngle::PolarizationAngle
( const std::string&           daughter     , 
  const std::string&           parent       ,      
  const bool                   mother       , 
  const bool                   autovalidate ,
  const std::string&           factory      ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_first        ( s_INVALID    )
  , m_second       ( s_INVALID    )
  , m_mother       ( mother       ) 
  , m_autovalidate ( autovalidate ) 
  , m_factory      ( factory      ) 
{
  //
  m_first  = getTree ( daughter ) ;
  if ( !m_first.valid()  ) { validate  ( m_first ) ; }
  //
  m_second = getTree ( parent   ) ;
  if ( !m_second.valid() ) { validate  ( m_second ) ; }
  //
  Assert ( tree1().marked() , "The first  tree is non-marked!" ) ;
  Assert ( tree2().marked() , "The second tree is non-marked!" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::PolarizationAngle::~PolarizationAngle()
{
  m_first  . reset () ;
  m_second . reset () ;
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PolarizationAngle*
LoKi::Particles::PolarizationAngle::clone() const 
{ return new LoKi::Particles::PolarizationAngle ( *this ) ; }
// ============================================================================
// get the tree from the descriptor 
// ============================================================================
Decays::IDecay::Tree LoKi::Particles::PolarizationAngle::getTree
( const std::string& decay ) const 
{
  // decode the decay descriptors 
  LoKi::ILoKiSvc* loki = lokiSvc () ;
  Assert ( 0 != loki         , "LoKi::ILokiSvc* points to NULL!" ) ;
  // 
  // decode the decay descriptions
  SmartIF<IToolSvc> toolSvc ( loki ) ;
  Assert ( toolSvc.isValid() , "IToolSvc* points to NULL!"       ) ;
  // 
  Decays::IDecay* tool = 0 ;
  StatusCode sc = toolSvc -> retrieveTool ( m_factory , tool ) ;
  Assert ( sc.isSuccess() , "Unable to retrieve '" + m_factory + "'" , sc ) ;
  Assert ( 0 != tool      , "Decays::IDecay* points to NULL" ) ;
  //
  Decays::IDecay::Tree _tree = tool -> tree ( decay ) ;
  //
  toolSvc -> releaseTool ( tool ) ; // do nto need the tool anymore 
  //
  return _tree ;
}
// ============================================================================
// validate the tree 
// ============================================================================
void LoKi::Particles::PolarizationAngle::validate 
( const Decays::IDecay::iTree& tree ) const 
{
  LoKi::ILoKiSvc* loki = lokiSvc () ;
  Assert ( 0 != loki       , "LoKi::ILoKiSvc* points to NULL!" ) ;
  // 
  SmartIF<LHCb::IParticlePropertySvc> ppSvc ( loki ) ;
  Assert ( ppSvc.isValid() , "LHCb::IParticlePropertySvc* points to NULL!" ) ;
  //
  StatusCode sc = tree.validate ( ppSvc ) ;
  Assert ( sc.isSuccess() , "Unable to vaildate the tree" , sc ) ;
}
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
  if   ( !m_first  . valid () && autovalidate() ) { validate ( m_first ) ; }
  if   ( !m_second . valid () && autovalidate() ) { validate ( m_first ) ; }
  //
  Assert ( valid  () , "The trees are   invalid!"  ) ;
  Assert ( marked () , "The trees are non-marked!" ) ;
  //
  m_first.reset() ;
  if ( !m_first( p ) || 1 != m_first.collect  ( vct  ) )
  {
    Error ( "The particle is not compatible with the first  tree!" ) ; 
    return StatusCode::FAILURE ;
  }
  //
  m_second.reset() ;
  if ( !m_second ( p ) || 1 != m_second.collect  ( vct  ) )
  {
    Error ( "The particle is not compatible with the second tree!" ) ; 
    return StatusCode::FAILURE ;
  }
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
  s << " COSPOL( " 
    << " '" << tree1() << "'"
    << ",'" << tree2() << "'" ;    
  
  if       ( !factory().empty() ) 
  {
    s << " , " ; Gaudi::Utils::toStream ( m_mother       , s ) ; 
    s << " , " ; Gaudi::Utils::toStream ( m_autovalidate , s ) ; 
    s << " , " ; Gaudi::Utils::toStream ( m_factory      , s ) ;
  }
  else if  ( !m_autovalidate ) 
  {
    s << " , " ; Gaudi::Utils::toStream ( m_mother       , s ) ; 
    s << " , " ; Gaudi::Utils::toStream ( m_autovalidate , s ) ; 
  }
  else if  ( !m_mother ) 
  {
    s << " , " ; Gaudi::Utils::toStream ( m_mother       , s ) ; 
  }
  //
  return s << " ) " ;
}


// ============================================================================
// constructor from the trees 
// ============================================================================
LoKi::Particles::SinChi::SinChi
( const Decays::IDecay::iTree& particle1    , 
  const Decays::IDecay::iTree& particle2    , 
  const Decays::IDecay::iTree& particle3    , 
  const Decays::IDecay::iTree& particle4    , 
  const bool                   autovalidate ) 
  : LoKi::Particles::PolarizationAngle ( particle1    , 
                                         particle2    , 
                                         true         , 
                                         autovalidate )                                         
  , m_tree3   ( particle3 ) 
  , m_tree4   ( particle4 )
{
  //
  Assert ( tree3().marked() , "The third tree is non-marked!" ) ;
  Assert ( tree4().marked() , "The fourh tree is non-marked!" ) ;
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
  const bool         autovalidate , 
  const std::string& factory      ) 
  : LoKi::Particles::PolarizationAngle ( particle1    , 
                                         particle2    , 
                                         true         , 
                                         autovalidate , 
                                         factory      )
  , m_tree3   ( s_INVALID ) 
  , m_tree4   ( s_INVALID )
{
  //
  m_tree3  = getTree ( particle3 ) ;
  if ( !m_tree3.valid()  ) { validate  ( m_tree3 ) ; }
  //
  m_tree4  = getTree ( particle4 ) ;
  if ( !m_tree4.valid()  ) { validate  ( m_tree4 ) ; }
  //
  Assert ( tree3().marked() , "The third tree is non-marked!" ) ;
  Assert ( tree4().marked() , "The fourh tree is non-marked!" ) ;
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
  if   ( !m_tree3 . valid () && autovalidate() ) { validate ( m_tree3 ) ; }
  if   ( !m_tree4 . valid () && autovalidate() ) { validate ( m_tree4 ) ; }
  //
  Assert ( valid  () , "The trees are   invalid!"  ) ;
  Assert ( marked () , "The trees are non-marked!" ) ;
  //
  m_tree3.reset() ;
  if ( !m_tree3( p ) || 1 != m_tree3.collect  ( vct  ) )
  {
    Error ( "The particle is not compatible with the third tree!" ) ; 
    return StatusCode::FAILURE ;
  }
  //
  m_tree4.reset() ;
  if ( !m_tree4 ( p ) || 1 != m_tree4.collect  ( vct  ) )
  {
    Error ( "The particle is not compatible with the fourth tree!" ) ; 
    return StatusCode::FAILURE ;
  }
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
LoKi::Particles::SinChi::~SinChi() { m_tree3.reset() ; m_tree4.reset() ; } 
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
    <<   "'" << tree1 () 
    << "','" << tree2 ()  
    << "','" << tree3 ()  
    << "','" << tree4 () 
    <<   "'" ;
  //
  if ( !autovalidate()    ) { s << ", False"                  ; }
  if ( !factory().empty() ) { s << " , '" << factory() << "'" ; }
  //
  return s << " ) " ;  
}
// ============================================================================



// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::Particles::CosChi::CosChi 
( const Decays::IDecay::iTree& particle1    ,  
  const Decays::IDecay::iTree& particle2    , 
  const Decays::IDecay::iTree& particle3    , 
  const Decays::IDecay::iTree& particle4    , 
  const bool                   autovalidate ) 
  : LoKi::Particles::SinChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              autovalidate )
{
}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::Particles::CosChi::CosChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const bool         autovalidate , 
  const std::string& factory      ) 
  : LoKi::Particles::SinChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              autovalidate , 
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
    <<   "'" << tree1 () 
    << "','" << tree2 ()  
    << "','" << tree3 ()  
    << "','" << tree4 () 
    <<   "'" ;
  //
  if ( !autovalidate()    ) { s << ", False"                  ; }
  if ( !factory().empty() ) { s << " , '" << factory() << "'" ; }
  //
  return s << " ) " ;  
}
// ============================================================================




// ============================================================================
// constructor form the trees 
// ============================================================================
LoKi::Particles::AngleChi::AngleChi 
( const Decays::IDecay::iTree& particle1    ,  
  const Decays::IDecay::iTree& particle2    , 
  const Decays::IDecay::iTree& particle3    , 
  const Decays::IDecay::iTree& particle4    , 
  const bool                   autovalidate ) 
  : LoKi::Particles::CosChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              autovalidate )
{
}
// ============================================================================
// constructor from the decay descriptors
// ============================================================================
LoKi::Particles::AngleChi::AngleChi 
( const std::string& particle1    ,   
  const std::string& particle2    , 
  const std::string& particle3    , 
  const std::string& particle4    , 
  const bool         autovalidate , 
  const std::string& factory      ) 
  : LoKi::Particles::CosChi ( particle1    , 
                              particle2    , 
                              particle3    , 
                              particle4    , 
                              autovalidate , 
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
    <<   "'" << tree1 () 
    << "','" << tree2 ()  
    << "','" << tree3 ()  
    << "','" << tree4 () 
    <<   "'" ;
  //
  if ( !autovalidate()    ) { s << ", False"                  ; }
  if ( !factory().empty() ) { s << " , '" << factory() << "'" ; }
  //
  return s << " ) " ;  
}
// ============================================================================


  

// ============================================================================
// The END
// ============================================================================
