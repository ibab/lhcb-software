// $Id$ 
// =============================================================================
// Include files 
// =============================================================================
#include <iostream>
// =============================================================================
// local
// =============================================================================
#include "Analysis/Mute.h"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Utils::Mute
 *  @see Gaudi::Utils::Mute
 *  @see Gaudi::Utils::Tee
 *  @date 2013-02-19 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *
 *  Version           $Revision$
 *  Last Modification $Date$
 *                 by $Author$ 
 */
// ============================================================================
// constructor from filename 
// ============================================================================
Gaudi::Utils::Mute::Mute ( const std::string& fname , 
                           const bool         out   ) 
  : m_cout   ( out               ) 
  , m_output ( fname.c_str()     ) 
  , m_buffer ( out ? std::cout.rdbuf() : std::cerr.rdbuf() ) 
{
  // make a trick 
  if ( m_cout ) 
  { std::cout.rdbuf ( m_output. rdbuf () ) ; }  // redirect 'cout' to a 'fout'
  else 
  { std::cerr.rdbuf ( m_output. rdbuf () ) ; }  // redirect 'cout' to a 'fout'
}
// ============================================================================
// default constructor
// ============================================================================
Gaudi::Utils::Mute::Mute( const bool out  )
  : m_cout   ( out               ) 
  , m_output ( "/dev/null"       ) 
  , m_buffer ( out ? std::cout.rdbuf() : std::cerr.rdbuf() ) 
{
  // make a trick 
  if  ( m_cout ) 
  { std::cout.rdbuf ( m_output. rdbuf () ) ; } // redirect 'cout' to a 'fout'
  else 
  { std::cerr.rdbuf ( m_output. rdbuf () ) ; }  // redirect 'cout' to a 'fout'
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Utils::Mute::~Mute() { exit() ; }
// ============================================================================
// helper function to implement __exit__   
// ============================================================================
void Gaudi::Utils::Mute::exit  () 
{
  // to avoid double 'exit'
  if ( 0 == m_buffer ) { return ; } // return
  //
  if ( m_cout ) 
  { std::cout.rdbuf ( m_buffer ) ; } // restore the original stream buffer 
  else 
  { std::cerr.rdbuf ( m_buffer ) ; } // restore the original stream buffer 
  //
  m_buffer = 0 ;
}
// ============================================================================
// helper (empty) function to implement __enter__   
// ============================================================================
void Gaudi::Utils::Mute::enter () {}
// ============================================================================





// ============================================================================
// The END 
// ============================================================================
