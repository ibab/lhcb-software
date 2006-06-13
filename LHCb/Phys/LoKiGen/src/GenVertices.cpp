// $Id: GenVertices.cpp,v 1.5 2006-06-13 09:51:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $,
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/05/02 14:30:28  ibelyaev
//  censored
//
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Constants.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/GenVertices.h"
// ============================================================================

// ============================================================================
/** @file
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
 *  @date 2006-02-08 
 */
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenVertices::BarCode*
LoKi::GenVertices::BarCode::clone() const 
{ return new LoKi::GenVertices::BarCode(*this); }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenVertices::BarCode::~BarCode(){};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::BarCode::result_type 
LoKi::GenVertices::BarCode::operator() 
  ( LoKi::GenVertices::BarCode::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* points to NULL; return 0" ) ;
    return 0 ;
  }
  return v->barcode() ;  
} ;
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::BarCode::fillStream( std::ostream& stream ) const 
{ return stream << "GVBARCODE" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenVertices::PositionX*
LoKi::GenVertices::PositionX:: clone() const 
{ return new PositionX(*this); }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenVertices::PositionX::~PositionX(){};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::PositionX::result_type 
LoKi::GenVertices::PositionX::operator() 
  ( LoKi::GenVertices::PositionX::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* points to NULL; return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().x() ;  
} ;
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::PositionX::fillStream
( std::ostream& stream ) const 
{ return stream << "GVX" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenVertices::PositionY*
LoKi::GenVertices::PositionY:: clone() const 
{ return new PositionY(*this); }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenVertices::PositionY::~PositionY(){};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::PositionY::result_type 
LoKi::GenVertices::PositionY::operator() 
  ( LoKi::GenVertices::PositionY::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* points to NULL; return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().y() ;  
} ;
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::PositionY::fillStream( std::ostream& stream ) const 
{ return stream << "GVY" ; }
// ============================================================================


// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenVertices::PositionZ*
LoKi::GenVertices::PositionZ:: clone() const 
{ return new PositionZ(*this); }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenVertices::PositionZ::~PositionZ(){};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::PositionZ::result_type 
LoKi::GenVertices::PositionZ::operator() 
  ( LoKi::GenVertices::PositionZ::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* points to NULL; return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().z() ;  
} ;
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::PositionZ::fillStream( std::ostream& stream ) const 
{ return stream << "GVZ" ; }
// ============================================================================


// ============================================================================
/// MANDATORY: clone method ("virtual" constructor")
// ============================================================================
LoKi::GenVertices::PositionT*
LoKi::GenVertices::PositionT:: clone() const 
{ return new PositionT(*this); }
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenVertices::PositionT::~PositionT(){};
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::PositionT::result_type 
LoKi::GenVertices::PositionT::operator() 
  ( LoKi::GenVertices::PositionT::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* points to NULL; return 'InvalidTime'" ) ;
    return LoKi::Constants::InvalidTime ;
  }
  return v->position().t() ;  
} ;
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::PositionT::fillStream( std::ostream& stream ) const 
{ return stream << "GVT" ; }
// ============================================================================

// ============================================================================
/** constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenVertices::CountIF::CountIF
( const LoKi::Types::GCuts& cut   , 
  HepMC::IteratorRange      range ) 
  : LoKi::Function<const HepMC::GenVertex*>()
  , m_cut   ( cut   ) 
  , m_range ( range ) 
{};
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::GenVertices::CountIF::CountIF
( const LoKi::GenVertices::CountIF& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Function<const HepMC::GenVertex*>( right ) 
  , m_cut   ( right.m_cut   ) 
  , m_range ( right.m_range ) 
{};
// ============================================================================
/// virtual destructor
// ============================================================================
LoKi::GenVertices::CountIF::~CountIF(){};
// ============================================================================
/// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::GenVertices::CountIF*
LoKi::GenVertices::CountIF::clone() const 
{ return new CountIF( *this ) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::CountIF::result_type
LoKi::GenVertices::CountIF::operator() 
  ( LoKi::GenVertices::CountIF::argument v ) const 
{
  if ( 0 == v ) { return 0 ; }
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*>( v ) ;
  return std::count_if 
    ( _v->particles_begin ( m_range ) ,
      _v->particles_end   ( m_range ) , m_cut ) ;
};
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::CountIF::fillStream
( std::ostream& stream ) const 
{ 
  stream << "GVCOUNT[" << m_cut << ",";
  switch ( m_range ) 
  {
  case HepMC::parents       :
    stream << "parents"     ; break ;
  case HepMC::children      :
    stream << "children"    ; break ;
  case HepMC::family        :
    stream << "family"      ; break ;
  case HepMC::ancestors     :
    stream << "ancestors"   ; break ;
  case HepMC::descendants   :
    stream << "descendants" ; break ;
  case HepMC::relatives     :
    stream << "relatives"   ; break ;
  default: 
    stream << m_range       ; break ;
  } ;
  return stream << "]" ;  
} ;
// ============================================================================


// ============================================================================
/** constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenVertices::SumIF::SumIF
( const LoKi::Types::GFunc& fun   , 
  HepMC::IteratorRange      range , 
  const LoKi::Types::GCuts& cut   ) 
  : LoKi::Function<const HepMC::GenVertex*>()
  , m_fun   ( fun   ) 
  , m_cut   ( cut   ) 
  , m_range ( range ) 
{};
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::GenVertices::SumIF::SumIF
( const LoKi::GenVertices::SumIF& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Function<const HepMC::GenVertex*>( right ) 
  , m_fun   ( right.m_fun   ) 
  , m_cut   ( right.m_cut   ) 
  , m_range ( right.m_range ) 
{};
// ============================================================================
/// virtual destructor
// ============================================================================
LoKi::GenVertices::SumIF::~SumIF(){};
// ============================================================================
/// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::GenVertices::SumIF*
LoKi::GenVertices::SumIF::clone() const 
{ return new SumIF( *this ) ; }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenVertices::SumIF::result_type
LoKi::GenVertices::SumIF::operator() 
  ( LoKi::GenVertices::SumIF::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "HepMC::GenVertex* poitns to NULL, return 0 " ) ;
    return 0 ; 
  }
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*>( v ) ;
  double result = 0 ;
  for ( HepMC::GenVertex::particle_iterator ip = 
          _v->particles_begin( m_range ) ; 
        _v->particles_end( m_range ) != ip ; ++ip ) 
  {
    const HepMC::GenParticle* p = *ip ;
    if ( 0 == p ) 
    {
      Warning("HepMC::GenPArticle* points to 0; skip it") ;
      continue ;
    }
    if ( m_cut ( p ) ) { result += m_fun ( p ) ; }
  }
  return result ;
};
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenVertices::SumIF::fillStream
( std::ostream& stream ) const 
{ 
  stream << "GVSUM[" << m_fun << ",";
  switch ( m_range ) 
  {
  case HepMC::parents       :
    stream << "parents"     ; break ;
  case HepMC::children      :
    stream << "children"    ; break ;
  case HepMC::family        :
    stream << "family"      ; break ;
  case HepMC::ancestors     :
    stream << "ancestors"   ; break ;
  case HepMC::descendants   :
    stream << "descendants" ; break ;
  case HepMC::relatives     :
    stream << "relatives"   ; break ;
  default: 
    stream << m_range       ; break ;
  } ;
  return stream << "," << m_cut << "]" ;  
} ;
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

