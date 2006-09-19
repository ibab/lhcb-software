// $Id: IGiGaPhysicsConstructor.h,v 1.3 2006-09-19 22:13:22 gcorti Exp $
#ifndef GIGA_IGIGAPHYSICSCONSTRUCTOR_H 
#define GIGA_IGIGAPHYSICSCONSTRUCTOR_H 1

// Include files
// from GiGa 
#include "GiGa/IGiGaInterface.h"
// from Geant4 
#include "G4VPhysicsConstructor.hh"

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
  static const InterfaceID& interfaceID() ;
  
  /** accessor to G4VPhysicsConstructor
   *  @see G4VPhysicsConstructor
   *  @return pointer to G4VPhysicsConstructor object
   */
  virtual G4VPhysicsConstructor* physicsConstructor ()  const = 0 ;
  
protected:
  
  // destructor 
  virtual ~IGiGaPhysicsConstructor() ; 

};

#endif // GIGA_IGIGAPHYSICSCONSTRUCTOR_H

