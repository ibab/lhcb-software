// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/08/01 09:42:21  ibelyaev
// redesign and reimplementation of GiGaRunManager class
// 
// ============================================================================
#ifndef GIGA_GIGA_H 
#define GIGA_GIGA_H 1
/// STD & STL 
#include <string>
///@{
/**  forward declarations */
class ISvcLocator;       ///< GaudiKernbel
class IGiGaRunManager;   ///< GiGa 
class G4VVisManager;     ///< Geant4 
class G4UIsession;       ///< Geant4 
///@}

/** @namespace GiGa GiGa.h "GiGa/GiGa.h"
 *  
 *  @brief wrapper methods for creation of Geant4 objects 
 *
 *  Tne namespace consists of a number of wrapper methods 
 *  for creation of Geant4 objects 
 *
 *  @author Ivan Belyaev
 *  @date   31/07/2001
 */

namespace GiGa
{
  
  /** create GiGa Run Manager
   *  @param   Name name of run manager 
   *  @param   Loc  pointer to service Locator  
   *  @return  pointer to giga run manager abstract interface 
   */     
  IGiGaRunManager* 
  createRunManager( const std::string& Name = "GiGaMgr" , 
                    ISvcLocator*       Loc  =  0        );
  
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
#endif ///< GIGA_GIGA_H
// ============================================================================
