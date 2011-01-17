// $Id: selection.h,v 1.4 2006-06-11 15:23:47 ibelyaev Exp $ 
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ====================================================================
// $Log: not supported by cvs2svn $
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
}

// ====================================================================
// The END 
// ====================================================================
