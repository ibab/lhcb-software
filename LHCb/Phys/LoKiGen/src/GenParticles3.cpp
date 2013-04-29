// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Algs.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenParticles3.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/GenExtract.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  inline std::size_t count_parts 
  ( HepMC::GenVertex*           v , 
    HepMC::IteratorRange        r , 
    const LoKi::GenTypes::GCut& c ) 
  {
    return 
      0 == v ? 0 :
      LoKi::Algs::count_if ( v -> particles_begin ( r ) , 
                             v -> particles_end   ( r ) , c.func() ) ;
  }
  // ==========================================================================
  inline bool has_parts 
  ( HepMC::GenVertex*           v , 
    HepMC::IteratorRange        r , 
    const LoKi::GenTypes::GCut& c ) 
  {
    return 
      0 == v ? false :
      LoKi::Algs::found ( v -> particles_begin ( r ) , 
                          v -> particles_end   ( r ) , c.func() ) ;
  }
  // ==========================================================================
}
// ============================================================================
/*  constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenParticles::Count::Count
( const LoKi::Types::GCuts& cut   , 
  HepMC::IteratorRange      range ) 
  : LoKi::GenTypes::GFunc () 
  , m_cut   ( cut   ) 
  , m_range ( range )
{}
// ============================================================================
/*  constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenParticles::Count::Count
( HepMC::IteratorRange      range , 
  const LoKi::Types::GCuts& cut   ) 
  : LoKi::GenTypes::GFunc () 
  , m_cut   ( cut   ) 
  , m_range ( range )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::Count::~Count(){}
// ============================================================================
// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::GenParticles::Count*
LoKi::GenParticles::Count::clone() const 
{ return new LoKi::GenParticles::Count ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Count::result_type 
LoKi::GenParticles::Count::operator() 
  ( LoKi::GenParticles::Count::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::Particle* points to NULL, return -1000 " ) ;
    return -1000 ;  
  }
  //
  switch ( m_range ) 
  {
  case HepMC::parents     :
  case HepMC::ancestors   :
    return count_parts ( p -> production_vertex () , m_range , m_cut ) ; // RETURN
  case HepMC::children    :
  case HepMC::descendants :
    return count_parts ( p -> end_vertex        () , m_range , m_cut ) ; // RETURN 
  case HepMC::family      :
  case HepMC::relatives   :
    return 0 != p->production_vertex() ? 
      count_parts ( p -> production_vertex () , m_range , m_cut ) :
      count_parts ( p -> end_vertex        () , m_range , m_cut ) ;      // RETURN 
  default:
    // =
    Error ( "Invalid HepMC::Ityerator range, return -900 ") ;
    return -900 ; 
  }
  //
  return 0 ;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenParticles::Count::fillStream
( std::ostream& stream ) const 
{ 
  stream << " GCOUNT(" << m_cut << ",";
  switch ( m_range ) 
  {
  case HepMC::parents       :
    stream << "HepMC.parents"     ; break ;
  case HepMC::children      :
    stream << "HepMC.children"    ; break ;
  case HepMC::family        :
    stream << "HepMC.family"      ; break ;
  case HepMC::ancestors     :
    stream << "HepMC.ancestors"   ; break ;
  case HepMC::descendants   :
    stream << "HepMC.descendants" ; break ;
  case HepMC::relatives     :
    stream << "HepMC.relatives"   ; break ;
  default: 
    stream << m_range       ; break ;
  } ;
  return stream << ") " ;  
}
// ============================================================================


// ============================================================================
/*  constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenParticles::Has::Has
( const LoKi::Types::GCuts& cut   , 
  HepMC::IteratorRange      range ) 
  : LoKi::GenTypes::GCuts () 
  , m_cut   ( cut   ) 
  , m_range ( range )
{}
// ============================================================================
/*  constructor from the criteria and "range"
 *  @param cut the criteria
 *  @param range search region 
 *  @see HepMC::IteratorRange 
 */
// ============================================================================
LoKi::GenParticles::Has::Has
( HepMC::IteratorRange      range , 
  const LoKi::Types::GCuts& cut   ) 
  : LoKi::GenTypes::GCuts () 
  , m_cut   ( cut   ) 
  , m_range ( range )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::Has::~Has(){}
// ============================================================================
// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::GenParticles::Has*
LoKi::GenParticles::Has::clone() const 
{ return new LoKi::GenParticles::Has ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Has::result_type 
LoKi::GenParticles::Has::operator() 
  ( LoKi::GenParticles::Has::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::Particle* points to NULL, return false" ) ;
    return false ;  
  }
  //
  switch ( m_range ) 
  {
  case HepMC::parents     :
  case HepMC::ancestors   :
    return has_parts ( p -> production_vertex () , m_range , m_cut ) ; // RETURN
  case HepMC::children    :
  case HepMC::descendants :
    return has_parts ( p -> end_vertex        () , m_range , m_cut ) ; // RETURN 
  case HepMC::family      :
  case HepMC::relatives   :
    return 0 != p->production_vertex() ? 
      has_parts ( p -> production_vertex () , m_range , m_cut ) :
      has_parts ( p -> end_vertex        () , m_range , m_cut ) ;      // RETURN 
  default:
    // =
    Error ( "Invalid HepMC::Ityerator range, return false") ;
    return false ; 
  }
  //
  return false;
}
// ============================================================================
//  "SHORT" representation, @see LoKi::AuxFunBase
// ============================================================================
std::ostream& 
LoKi::GenParticles::Has::fillStream
( std::ostream& stream ) const 
{ 
  stream << " GHAS(" << m_cut << ",";
  switch ( m_range ) 
  {
  case HepMC::parents       :
    stream << "HepMC.parents"     ; break ;
  case HepMC::children      :
    stream << "HepMC.children"    ; break ;
  case HepMC::family        :
    stream << "HepMC.family"      ; break ;
  case HepMC::ancestors     :
    stream << "HepMC.ancestors"   ; break ;
  case HepMC::descendants   :
    stream << "HepMC.descendants" ; break ;
  case HepMC::relatives     :
    stream << "HepMC.relatives"   ; break ;
  default: 
    stream << m_range       ; break ;
  } ;
  return stream << ") " ;  
} 





   

// ============================================================================
// The END 
// ============================================================================
