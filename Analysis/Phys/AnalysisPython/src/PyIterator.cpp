// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <limits>
#include <iostream>
// ============================================================================
// AnalysisPython
// ============================================================================
#include "Analysis/Formula.h"
#include "Analysis/PyIterator.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TTree.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file 
 *  implemetation file for class Analysis::PyIterator
 *  @author Vanya Belyaev
 *  @date   2013-05-06
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  BOOST_STATIC_ASSERT( std::numeric_limits<unsigned long>::is_specialized  && 
                       boost::integer_traits<unsigned long>::is_specialized ) ;
}
// ============================================================================
// constructor 
// ============================================================================
Analysis::PyIterator::PyIterator 
( TTree*              tree  , 
  const std::string&  cuts  , 
  const unsigned long first , 
  const unsigned long last  )  
  : m_tree     ( tree  ) 
  , m_formula  ( 0     )  
  , m_current  ( first )
  , m_last     ( last  )
{
  // 
  if ( 0 == m_tree ) 
  {
    m_current = 0 ;
    m_last    = 0 ;
  }
  else
  { 
    //
    m_last    = std::min ( m_last , (unsigned long) tree->GetEntries() ) ;
    m_formula = new Analysis::Formula ( "" ,  cuts , m_tree ) ;
    //
    if ( !m_formula->GetNdim() ) { delete m_formula ; m_formula = 0 ; }
    else                         { m_tree->SetNotify ( m_formula )  ; }
    //
  }
  //
  m_tree = next () ;
}
// ============================================================================
// destructor 
// ============================================================================
Analysis::PyIterator::~PyIterator()
{
  if ( 0 != m_formula ) { delete m_formula ; m_formula = 0 ; }
  m_tree = 0 ;
}
// ============================================================================
// go to next item 
// ============================================================================
TTree* Analysis::PyIterator::next () const                   // go to next item 
{
  //
  if ( 0 == m_tree         ) { return 0 ; }
  if ( 0 == m_formula      ) { return 0 ; }
  //
  for ( ; m_current < m_last ; ++m_current ) 
  {
    //
    long ievent = m_tree->GetEntryNumber ( m_current ) ;
    if ( 0 > ievent ) { continue ; }  // CONTINUE
    //
    ievent      = m_tree->LoadTree ( ievent ) ;
    if ( 0 > ievent ) { continue ; }  //  CONTINUE 
    //
    if ( m_formula->evaluate() ) 
    { 
      m_tree -> GetEntry ( ievent ) ; // ATTENTION! Load here everything!
      ++m_current   ; // NB! 
      return m_tree ;
    } 
  }
  //
  return 0 ;
}
// ============================================================================
// check if formula is ok 
// ============================================================================
bool Analysis::PyIterator::ok   () const 
{ return 0 != m_formula && m_formula->ok() ; }

// ============================================================================
// The END 
// ============================================================================
