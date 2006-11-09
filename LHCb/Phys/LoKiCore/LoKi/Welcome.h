// $Id: Welcome.h,v 1.5 2006-11-09 17:01:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/06/24 17:18:41  ibelyaev
//  reduce the printout
//
// ============================================================================
#ifndef LOKI_WELCOME_H 
#define LOKI_WELCOME_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
#include <iostream>
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
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi 
{
  /** @class Welcome Welcome.h LoKi/Welcome.h
   *  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-23
   */
  class Welcome : protected std::vector<int>
  {
  public:
    /// get the static instance of class LoKi::Welcome 
    static const Welcome& instance() ;
  public:
    // destructor 
    ~Welcome () ;
  public:
    void welcome  ( std::ostream& stream = std::cout ) const ;
    void goodbye  ( std::ostream& stream = std::cout ) const ;    
  protected:
    // Standard constructor
    Welcome ();
  private:
    Welcome ( const Welcome& );
  private:
    size_t                   m_len1        ;
    std::string              m_str1        ;    
    std::string              m_fmt1        ;    
    std::string              m_fmt2        ;    
    std::string              m_fmt3        ;    
    mutable bool             m_wel_printed ;
    mutable bool             m_bye_printed ;    
  };
  
}; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_WELCOME_H
// ============================================================================
