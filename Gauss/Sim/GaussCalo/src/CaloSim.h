// $Id: CaloSim.h,v 1.4 2004-10-08 15:06:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
// 
// ============================================================================
#ifndef CALOSIM_CALOSIM_H 
#define CALOSIM_CALOSIM_H 1
// Include files
// STD & STL 
#include <vector>
// forward declaration
class G4VPhysicalVolume;

/** @namespace CaloSim 
 *  
 *  Definiton of useful types, functions 
 *  and constants for CaloSim package 
 *
 *  @author Alexei Berdiouguine
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-07-09
 */

namespace CaloSim 
{
  // cell id for CaloSim package 
  //typedef  std::vector<int>                      Path  ;
  typedef  std::vector<const G4VPhysicalVolume*> Path ;
};


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM_H
// ============================================================================
