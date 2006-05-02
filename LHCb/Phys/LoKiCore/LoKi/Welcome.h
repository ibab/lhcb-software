// $Id: Welcome.h,v 1.3 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
    static Welcome& instance() ;
  public:
    // destructor 
    ~Welcome () ;
  public:
    void welcome () const ;
  protected:
    void goodbye () const ;    
  protected:
    // Standard constructor
    Welcome  ( std::ostream& stream ) ;
  private:
    Welcome ();
    Welcome ( const Welcome& );
  private: 
    std::ostream&            m_stream ;
    size_t                   m_len1   ;
    std::string              m_str1   ;    
    std::string              m_fmt1   ;    
    std::string              m_fmt2   ;    
    std::string              m_fmt3   ;    
  };
  
}; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_WELCOME_H
// ============================================================================
