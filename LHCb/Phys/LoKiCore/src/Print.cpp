// $Id: Print.cpp,v 1.3 2006-05-02 14:29:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Print.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::Print 
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
 *  @date 2006-02-16 
 */
// ============================================================================

// ============================================================================
/// convert double  value to string 
// ============================================================================
std::string LoKi::Print::print ( const double value ) 
{ return boost::lexical_cast<std::string>( value ) ;}
// ============================================================================
std::string print ( const long   value ) ;
// ============================================================================
std::string LoKi::Print::print ( const long   value ) 
{ return boost::lexical_cast<std::string>( value ) ;}
// ============================================================================
/// convert boolean value to string 
// ============================================================================
std::string LoKi::Print::print ( const bool   value ) 
{ return value? "True" : "False" ; }
// ============================================================================

// ============================================================================
// The END
// ============================================================================


