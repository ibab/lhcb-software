// $Id: GiGaExtPhysics.h,v 1.3 2007-03-18 18:06:39 gcorti Exp $
#ifndef GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 
#define GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 1

// Include files
#include "GiGa/GiGaPhysicsConstructorBase.h"

/** @class GiGaExternalPhysicsConstructor GiGaExternalPhysicsConstructor.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */

template <class PHYSCONSTR>
class GiGaExtPhysics : public GiGaPhysicsConstructorBase
{
  /// actual type of Physics Constructor 
  typedef PHYSCONSTR PhysConstr ;
  
public:
  
  /** accessor to G4VPhysicsConstructor
   *  @see IGiGaPhysConstructor
   *  @see IGiGaPhysicsConstructor
   *  @see G4VPhysicsConstructor
   *  @return pointer to G4VPhysicsConstructor object
   */
  G4VPhysicsConstructor* physicsConstructor ()  const
  {
    if( 0 == m_phys )
    {
      m_phys =  new PhysConstr() ;
      m_phys -> SetPhysicsName( name() );
    }
    return m_phys ;
  };
  
  /** Standard constructor
   *  @see GiGaPhysicsConstructorBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @param type type of the instance 
   *  @param name name of the instance 
   *  @param svc  service locator
   */
  GiGaExtPhysics
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent )
    : GiGaPhysicsConstructorBase( type , name , parent ) 
      , m_phys ( 0 )
  {}
  
  /// destructor 
  virtual ~GiGaExtPhysics(){} ;
  
private:
  
  // physics constructor itself 
  mutable G4VPhysicsConstructor* m_phys ;

};

#endif // GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H
