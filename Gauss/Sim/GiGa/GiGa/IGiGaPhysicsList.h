// $Id: IGiGaPhysicsList.h,v 1.2 2004-02-20 18:58:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
#ifndef GIGA_IGIGAPHYSICSLIST_H 
#define GIGA_IGIGAPHYSICSLIST_H 1
// ============================================================================
// Include files
// GiGa 
#include "GiGa/IGiGaInterface.h"
// Geant4 
class G4VUserPhysicsList ;

/** @class IGiGaPhysicsList IGiGaPhysicsList.h GiGa/IGiGaPhysicsList.h
 *
 *  An abstract interface for Geant4 Physics List 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class IGiGaPhysicsList : virtual public IGiGaInterface 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ; 
  
  /** get G4VUserPhysicsList 
   *  @see G4VUserPhysicsList
   *  @return pointer to G4VUserPhysicsList object
   */
  virtual G4VUserPhysicsList* physicsList ()  const = 0 ;
  
protected:
  
  // virtual destructor 
  virtual ~IGiGaPhysicsList () ; 
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_IGIGAPHYSICSLIST_H
// ============================================================================
