// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <iostream>
// ============================================================================
// local
// ============================================================================
#include "LoKi/Welcome.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Welcome 
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
 *  @date 2001-01-23 
 */
// ============================================================================
// Meyers's singleton
// ============================================================================
const LoKi::Welcome& LoKi::Welcome::instance () 
{
  static LoKi::Welcome s_welcome = Welcome() ;
  return s_welcome ;
}
// ============================================================================
// standard constructor 
// ============================================================================
LoKi::Welcome::Welcome ()
  : m_len1   ( 103      ) 
  , m_str1   ( "LoKi"   ) 
  , m_fmt1   ( "%|-5|"  ) 
  , m_fmt2   ( "%|-94|" ) 
  , m_fmt3   ( "%|=94|" ) 
  , m_wel_printed ( false )
  , m_bye_printed ( false )
{} 
// ============================================================================
LoKi::Welcome::~Welcome() { goodbye () ; }
// ============================================================================
void LoKi::Welcome::welcome() const { welcome ( std::cout ) ; }
// ============================================================================
void LoKi::Welcome::welcome( std::ostream& stream ) const
{
  
  if ( m_wel_printed ) { return ; } // never print twice
  m_wel_printed = true ;
  
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Welcome " ; }
  m_str2 = std::string( m_str2.begin() , m_str2.begin() + m_len1 ) ;
  
  stream << std::endl << std::endl ; 
  stream << m_str2    << std::endl ; 
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Welcome to LoKi!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "(LOops & KInematics)" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Smart & Friendly C++ Physics Analysis Tool Kit" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Author:  Vanya BELYAEV Ivan.Belyaev@itep.ru " ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "With the kind help of Galina Pakhlova & Sergey Barsuk " ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt2 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Have fun and enjoy!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt2 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  stream << m_str2    << std::endl ;
  
  stream << m_str2    << std::endl ;  
  stream << std::endl << std::endl ;
  
}
// ============================================================================
void LoKi::Welcome::goodbye () const { goodbye ( std::cout ) ; }
// ============================================================================
void LoKi::Welcome::goodbye ( std::ostream& stream ) const 
{
  if ( !m_wel_printed ) { return ; } // no printout
  if (  m_bye_printed ) { return ; } // never print twice
  m_bye_printed = true ;
  
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Good Bye " ; }
  m_str2 = std::string ( m_str2.begin() ,  m_str2.begin() + m_len1 ) ;
  
  stream << std::endl << std::endl ;
  
  stream << m_str2    << std::endl ;
  stream << m_str2    << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Good Bye from LoKi!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "(LOops & KInematics)" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Smart & Friendly C++ Physics Analysis Tool Kit" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;

  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;

  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt2 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "LoKi mailing list ('LoKi-club') : lhcb-loki@cern.ch ";
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  std::string _s("Comments, suggestions, criticism, ideas, ") ;
  _s += "questions and requests are *ALWAYS* welcome";
  stream << boost::format ( m_fmt3 ) % _s ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << m_str2    << std::endl ;  
  stream << m_str2    << std::endl ;  
  stream << std::endl << std::endl ;
} 
// ============================================================================

// ============================================================================
// Meyers's singleton
// ============================================================================
const Bender::Welcome& Bender::Welcome::instance() 
{
  // make sure that LoKi is instantiated *BEFORE*
  LoKi::Welcome::instance() ;
  static Bender::Welcome s_welcome = Bender::Welcome() ;
  return s_welcome ;
}
// ============================================================================
// standard constructor 
// ============================================================================
Bender::Welcome::Welcome () 
  : m_len1   ( 103      ) 
  , m_str1   ( "Bender" ) 
  , m_fmt1   ( "%|-7|"  ) 
  , m_fmt2   ( "%|-90|" ) 
  , m_fmt3   ( "%|=90|" ) 
  , m_wel_printed ( false )
  , m_bye_printed ( false )
{
  // own welcome 
  welcome  () ;
}
// ============================================================================
// destructor 
// ============================================================================
Bender::Welcome::~Welcome() { goodbye () ; }
// ============================================================================
// print welcome message 
// ============================================================================
void Bender::Welcome::welcome() const { return welcome ( std::cout ) ; }
// ============================================================================
void Bender::Welcome::welcome( std::ostream& stream ) const
{
  // LoKi welcome 
  LoKi::Welcome::instance().welcome ( stream ) ;
  //
  if ( m_wel_printed ) { return ; }                          // RETURN 
  m_wel_printed = true ;
  //
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Welcome " ; }
  m_str2 = std::string( m_str2.begin() , m_str2.begin() + m_len1 ) ;
  //
  stream << std::endl << std::endl ;
  //
  stream << m_str2    << std::endl ;
  stream << m_str2    << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Welcome to Bender!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Python-based Physics Analysis Application" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Author:  Vanya BELYAEV Ivan.Belyaev@itep.ru " ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "With the kind help of Pere Mato & Andrey Tsaregorodtsev " ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt2 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Have fun and enjoy! Good Luck!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  //
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt2 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  //
  stream << m_str2    << std::endl ;  
  stream << m_str2    << std::endl ;  
  stream << std::endl << std::endl ;
}
// ============================================================================
// print good bye message 
// ============================================================================
void Bender::Welcome::goodbye () const { return goodbye ( std::cout ) ; }
// ============================================================================
void Bender::Welcome::goodbye ( std::ostream& stream ) const 
{
  if ( !m_wel_printed ) { return ; }
  if (  m_bye_printed ) { return ; }
  //
  m_bye_printed = true ;
  //
  std::string m_str2 = "" ;
  while ( m_str2.size() < m_len1 ) { m_str2 += "Good Bye " ; }
  m_str2 = std::string ( m_str2.begin() ,  m_str2.begin() + m_len1 ) ;
  
  stream << std::endl << std::endl ;
  
  stream << m_str2    << std::endl ;
  stream << m_str2    << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "Good Bye from Bender!" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;  
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Python-based Physics Analysis Application" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % 
    "Bender mailing list : lhcb-bender@cern.ch ";
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  std::string _s("Comments, suggestions, criticism, ideas, ") ;
  _s += "questions and requests are *ALWAYS* welcome";
  stream << boost::format ( m_fmt3 ) % _s ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << boost::format ( m_fmt1 ) % m_str1 ;
  stream << boost::format ( m_fmt3 ) % "" ;
  stream << boost::format ( m_fmt1 ) % m_str1 << std::endl ;
  
  stream << m_str2    << std::endl ;  
  stream << m_str2    << std::endl ;  
  stream << std::endl << std::endl ;
}
// ============================================================================
// The END 
// ============================================================================
