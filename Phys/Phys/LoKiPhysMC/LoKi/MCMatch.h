// $Id: MCMatch.h,v 1.4 2007-04-16 16:16:48 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/08/29 11:40:47  ibelyaev
//  many fixed to simplify MC-match
//
// Revision 1.2  2006/04/09 20:14:25  ibelyaev
//  fix for Win32
//
// Revision 1.1.1.1  2006/03/14 19:12:21  ibelyaev
// New package : RC <---> MC links for LoKi 
// 
// ============================================================================
#ifndef LOKI_MCMATCH_H 
#define LOKI_MCMATCH_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Interface.h"
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
 *  @date 2006-03-11 
 */
// ============================================================================

namespace LoKi 
{
  class MCMatchObj ;
  /** @class MCMatch MCMatch.h LoKi/MCMatch.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  class MCMatch 
    : public LoKi::Interface<LoKi::MCMatchObj>
  { 
  public: 
    /// Standard constructor from the object and reporter 
    MCMatch ( const LoKi::MCMatchObj*                  object ) ; 
    /// destructor 
    virtual ~MCMatch( ); ///< Destructor
    /// implicit conversion to the pointer 
    operator const LoKi::MCMatchObj* () const ;
  private:
    /// the default constructor is disabled 
    MCMatch();
  } ; 
} ; // end of the namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCMATCH_H
// ============================================================================
