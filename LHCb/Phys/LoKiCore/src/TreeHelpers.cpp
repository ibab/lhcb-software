// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/NodesPIDs.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TreeHelpers.h"
// ============================================================================
/** @file  
 *  Implementation file for class Decays::Parsers::Tree
 *  @date 2009-05-22 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  inline bool noHead ( const Decays::Parsers::Tree& t ) 
  { 
    if ( t.head().valid () ) { return false ; }
    //
    const Decays::iNode* inode = &(t.head())  ;
    //
    return !inode->valid() && 
      0 != dynamic_cast<const Decays::Nodes::Invalid*> ( inode ) ;  
  }
  // ==========================================================================
}
// ============================================================================
// default constructor 
// ============================================================================
Decays::Parsers::Tree::Tree()
  : m_head  ( Decays::Nodes::Invalid() ) 
  , m_or    ()
  , m_and   ()
  , m_arrow      ( Decays::Trees::Single    )
  , m_oscillated ( Decays::Trees::Undefined ) 
  , m_inclusive  ( false                    )                 
  , m_negated    ( false                    )                 
  , m_marked     ( false                    )                 
  , m_stable     ( false                    )                 
  , m_children () 
  , m_optional ()
{}
// ============================================================================
// constructor from the decay head  
// ============================================================================
Decays::Parsers::Tree::Tree
( const Decays::iNode& head   , 
  const bool           stable ) 
  : m_head  ( head ) 
  , m_or    ()
  , m_and   ()
  , m_arrow      ( Decays::Trees::Single    )
  , m_oscillated ( Decays::Trees::Undefined ) 
  , m_inclusive  ( false                    )                 
  , m_negated    ( false                    )                 
  , m_marked     ( false                    )                 
  , m_stable     ( stable                   )                 
  , m_children () 
  , m_optional ()
{}

// ============================================================================
// configuration
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const Decays::Trees::Oscillation& osc ) 
{ m_oscillated = osc ;  return *this ; }
// ============================================================================
// configuration
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const Decays::Trees::Arrow& arr ) 
{ m_arrow = arr ;  return *this ; }
// ============================================================================
// configuration
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const bool inclusive ) 
{ m_inclusive = inclusive ;  return *this ; }
// ============================================================================
// configuration
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator*= ( const bool negated  ) 
{ m_negated = negated  ;  return *this ; }
// ============================================================================
// configuration
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator/= ( const bool marked  ) 
{ m_marked = marked  ;  return *this ; }
// ============================================================================



// ============================================================================
// Operators
// ============================================================================
// add to daughters 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const Decays::Parsers::Tree& tree ) 
{ 
  m_children.push_back ( tree ) ; 
  return *this ; 
}
// ============================================================================
// add to daughters 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const Decays::Parsers::Tree::Trees& trees ) 
{ 
  m_children.insert ( m_children.end() , trees.begin() , trees.end() ) ;
  return *this ; 
}
// ============================================================================
// add to daughters 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator+= ( const Decays::iNode& tree ) 
{ 
  m_children.push_back ( tree ) ; 
  return *this ; 
}
// ============================================================================

// ============================================================================
// add to optional 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator%= ( const Decays::Parsers::Tree& tree ) 
{ m_optional.push_back ( tree ) ;  return *this ; }
// ============================================================================
// add to optional 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator%= ( const Decays::Parsers::Tree::Trees& trees ) 
{ 
  m_optional.insert ( m_optional.end() , trees.begin() , trees.end() ) ;
  return *this ; 
}
// ============================================================================
// add to optional 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator%= ( const Decays::iNode& tree ) 
{ m_optional.push_back ( tree ) ;  return *this ; }
// ============================================================================


// ============================================================================
// OR
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator|= ( const Decays::Parsers::Tree& tree ) 
{
  if  ( noHead ( *this ) 
        && m_children.empty() && m_optional.empty() && m_and.empty() 
        && !m_negated && !m_marked ) 
  { m_or .push_back ( tree ) ; return *this ; }                       // RETURN
  // make temporary tree 
  Tree tmp ;
  tmp |= *this ;
  tmp |=  tree ;
  return (*this) = tmp ; // ASSIGN! 
}
// ============================================================================
// OR
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator|=( const Decays::iNode& tree ) 
{ return (*this) |= Tree ( tree ) ; }
// ============================================================================
// OR
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator|=( const Decays::Parsers::Tree::Trees& trees ) 
{
  for ( Trees::const_iterator it = trees.begin() ; trees.end() != it ; ++it ) 
  { (*this) |= (*it) ; }                                             // ASSIGN! 
  return *this ;
}
// ============================================================================

// ============================================================================
// AND
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator&= ( const Decays::Parsers::Tree& tree ) 
{
  if  ( noHead ( *this ) 
        && m_children.empty() && m_optional.empty() && m_or.empty() 
        && !m_negated && !m_marked ) 
  { m_and .push_back ( tree ) ; return *this ; }                       // RETURN 
  // make temporary tree 
  Tree tmp ;
  tmp &= *this ;
  tmp &=  tree ;
  return (*this) = tmp ; // ASSIGN! 
}
// ============================================================================
// AND
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator&=( const Decays::iNode& tree ) 
{ return (*this) &= Tree ( tree ) ; }
// ============================================================================
// AND 
// ============================================================================
Decays::Parsers::Tree& 
Decays::Parsers::Tree::operator&=( const Decays::Parsers::Tree::Trees& trees ) 
{
  for ( Trees::const_iterator it = trees.begin() ; trees.end() != it ; ++it ) 
  { (*this) &= (*it) ; }                                             // ASSIGN! 
  return *this ;
}
// ============================================================================





// ============================================================================
// printout 
// ============================================================================
std::ostream& Decays::Parsers::Tree::fillStream ( std::ostream& s ) const 
{
  if  ( m_marked  ) { s << " ^" ; }
  if  ( m_negated ) { s << " ~" ; }
  
  typedef Trees::const_iterator CIT ;
  if      ( !m_or.empty() ) 
  {
    s << " (" ;
    for ( CIT it = m_or.begin() ; m_or.end() != it ; ++it ) 
    {
      if ( it != m_or.begin() ) { s << " ||" ; } 
      s << " " << (*it) ;
    }
    return s << " ) " ;                                                // RETURN 
  }
  else if ( !m_and.empty() ) 
  {
    s << " (" ;
    for ( CIT it = m_and.begin() ; m_and.end() != it ; ++it ) 
    {
      if ( it != m_and.begin() ) { s << " &&" ; }
      s << " " << (*it) ;
    }
    return s << " ) " ;                                               // RETURN 
  }
  
  if ( m_children.empty() && m_optional.empty() ) { return s << m_head  ; } // RETURN
  
  
  if  ( m_stable  ) { return s << "<"  << m_head << ">" ; }   // RETURN

  s << " (" ;
  switch  ( m_oscillated ) 
  {
  case Decays::Trees::Oscillated    : 
    s << " [" << m_head << "]os "  ; break ;   
  case Decays::Trees::NotOscillated : 
    s << " [" << m_head << "]nos " ; break ;   
  default : s << m_head ;
  }
  
  s << " " << Decays::Trees::arrow ( m_arrow ) ;
  
  for ( CIT ic = m_children.begin() ; m_children.end() != ic ; ++ic )
  { s << " "  << (*ic) ; }
  
  for ( CIT io = m_optional.begin() ; m_optional.end() != io ; ++io )
  { s << " {" << (*io)  << "}"; }
  
  if ( m_inclusive ) { s << " ..." ; }
  // 
  return s << " ) " ;
}
// ============================================================================
// convert to string 
// ============================================================================
std::string Decays::Parsers::Tree::toString() const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// operator 
// ============================================================================
std::ostream& operator<< ( std::ostream& s , const Decays::Parsers::Tree& t )  
{ return t.fillStream ( s ) ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
