// $Id: IGiGaPhysicsConstructor.h,v 1.1 2003-04-06 18:49:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_IGIGAPHYSICSCONSTRUCTOR_H 
#define GIGA_IGIGAPHYSICSCONSTRUCTOR_H 1
// ============================================================================
// Include files
// GiGa 
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IIDIGiGaPhysicsConstructor.h"
// Genat4 
class G4VPhysicsConstructor ;

/** @class IGiGaPhysicsConstructor IGiGaPhysicsConstructor.h
 *  
 *  An abstract interface to Geant4 Physics Constructor 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class IGiGaPhysicsConstructor : virtual public IGiGaInterface 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() 
  { return IID_IGiGaPhysicsConstructor ; }
  
  /** accessor to G4VPhysicsConstructor
   *  @see G4VPhysicsConstructor
   *  @return pointer to G4VPhysicsConstructor object
   */
  virtual G4VPhysicsConstructor* physicsConstructor ()  const = 0 ;
  
protected:
  
  // destructor 
  virtual ~IGiGaPhysicsConstructor(){} ; 

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_IGIGAPHYSICSCONSTRUCTOR_H
// ============================================================================
