// $Id:$ 
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
 *  @date 2013-02-19 
 *  @author Vanya Belyaev Ivan.Belayev@itep.ru
 */
// ============================================================================
// constructor from filename 
// ============================================================================
Gaudi::Utils::Mute::Mute( const std::string& fname , 
                          const bool         out   ) 
  : m_cout   ( out               ) 
  , m_output ( fname.c_str()     ) 
  , m_buffer ( std::cout.rdbuf() ) 
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
  , m_buffer ( std::cout.rdbuf() ) 
{
  // make a trick 
  if  (m_cout ) 
  { std::cout.rdbuf ( m_output. rdbuf () ) ; } // redirect 'cout' to a 'fout'
  else 
  { std::cerr.rdbuf ( m_output. rdbuf () ) ; }  // redirect 'cout' to a 'fout'
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Utils::Mute::~Mute()
{
  if ( m_cout ) 
  { std::cout.rdbuf ( m_buffer ) ; } // restore the original stream buffer 
  else 
  { std::cerr.rdbuf ( m_buffer ) ; } // restore the original stream buffer 
}
// ============================================================================
// The END 
// ============================================================================
