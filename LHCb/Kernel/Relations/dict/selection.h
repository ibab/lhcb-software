// $Id: selection.h,v 1.3 2006-03-29 14:38:23 cattanem Exp $ 
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ====================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/07 13:42:53  ibelyaev
//  many new changed : see relese.notes
//
// ====================================================================
// Incldue file 
// ====================================================================
// STD & STL 
// ====================================================================
#include <string>
#include <vector>
// ====================================================================
// Relations 
// ====================================================================
#ifndef WIN32
#include "Relations/RelationsDict.h"
// ====================================================================
// Other incldue files: put your own includes here:
// ====================================================================
//#include ...
//#include ...
//#include ...
// ====================================================================


/** @file
 *
 *  Helper file for building the simplest Reflex dictionaries
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-05
 */

namespace RelationsDict
{
  struct __Instantiations
  {
    GaudiDict::Relation1DDict<int,int>           _1 ;
    GaudiDict::Relation2DDict<std::string,int>   _2 ;
  };
};
#endif // WIN32

// ====================================================================
// The END 
// ====================================================================
