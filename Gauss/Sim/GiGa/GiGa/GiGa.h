// $Id: GiGa.h,v 1.3 2002-05-07 12:21:28 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGA_H 
#define GIGA_GIGA_H 1
// STD & STL 
#include <string>
//
class G4VVisManager;
class G4UIsession;  

/** @namespace GiGa GiGa.h "GiGa/GiGa.h"
 *  
 *  Wrapper methods for creation of some Geant4 objects 
 *
 *  Tne namespace consists of a number of wrapper methods 
 *  for creation of Geant4 objects 
 *
 *  @author Ivan Belyaev
 *  @date   31/07/2001
 */

namespace GiGa
{  

  /** create (and initialize) GiGa Vis manager
   *  @return pointer to Visualization Manager 
   */
  G4VVisManager* createVisManager();
  
  /** create  User Interface session
   *  @param  type    the session type 
   *  @return pointer to user interface session 
   */
  G4UIsession*  createUIsession( const std::string& type );

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGA_H
// ============================================================================
