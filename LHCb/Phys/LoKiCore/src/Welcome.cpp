// $Id: Welcome.cpp,v 1.7 2006-11-09 18:26:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/11/09 17:01:06  ibelyaev
//  v1r8: improve printout
//
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

// ============================================================================
/// Meyers's singleton
// ============================================================================
const LoKi::Welcome& LoKi::Welcome::instance () 
{
  static LoKi::Welcome s_welcome = Welcome() ;
  return s_welcome ;
};
// ============================================================================
/// standard constructor 
// ============================================================================
LoKi::Welcome::Welcome ()
  : m_len1   ( 103      ) 
  , m_str1   ( "LoKi"   ) 
  , m_fmt1   ( "%|-5|"  ) 
  , m_fmt2   ( "%|-94|" ) 
  , m_fmt3   ( "%|=94|" ) 
  , m_wel_printed ( false )
  , m_bye_printed ( false )
{ 
} ;
// ============================================================================
LoKi::Welcome::~Welcome() { goodbye () ; };
// ============================================================================

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
    "Author:  Vanya BELYAEV ibelyaev@physics.syr.edu" ;
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
  
};
// ============================================================================

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
} ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
