// $Id: GiGaExternalPhC.h,v 1.3 2007-03-26 09:01:39 gcorti Exp $
#ifndef GIGA_GIGAEXTERNALPHC_H 
#define GIGA_GIGAEXTERNALPHC_H 1

// Include files
#include "GiGa/IGiGaPhysConstructor.h"
#include "GiGa/GiGaPhysConstructorBase.h"


/** @class GiGaExternalPhC GiGaExternalPhC.h GiGa/GiGaExternalPhC.h
 *  
 *  Helper templated class to make easy the import of the 
 *  external Physics Constructors 
 *  
 *  The external Physics constructor class should 
 *  fulfill following <b>mandatory</b> requirements:
 *  - It <b>MUST</b> have 
 *  the constructor of the type  <tt>Type( const std::string& )</tt>
 *  - It <b>MUST</b> be inherited from <tt>G4VPhysicsConstructor</tt>
 *    using <b>public virtual</b> inhertitance
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-03
 */

template <class PHYSICSCONSTRUCTOR>
class GiGaExternalPhC :
  public virtual IGiGaPhysConstructor     ,  
  public virtual  GiGaPhysConstructorBase ,
  public virtual G4VPhysicsConstructor    ,
  public virtual PHYSICSCONSTRUCTOR
{
  /// own type 
  typedef GiGaExternalPhC<PHYSICSCONSTRUCTOR> MyType ;

protected:
  
  /** standard constructor 
   *  
   *  @warning Constructor exploits the  virtual inheritance 
   *  Constructor COULD produce compiler warning
   *  for CORRECT  inheritance schema and 
   *  it MUST produce the compiler error 
   *  for INCORECT inheritance schema  
   *
   *  @see GiGaPhysConstructorBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @param name of the instance 
   *  @param svc  service locator 
   */ 
  GiGaExternalPhC( const std::string& type   , 
                   const std::string& name   , 
                   const IInterface*  parent )
    : GiGaPhysConstructorBase ( type , name , parent )
    , PHYSICSCONSTRUCTOR      (        name          ) 
    , G4VPhysicsConstructor   (        name          )
  {
    // check for virtual inheritance 
    // the following line COULD produce compiler warning
    // for CORRECT  inheritance schema and 
    // it MUST produce the compiler error 
    // for INCORECT inheritance schema  
    G4ParticleTable* tt = theParticleTable ;
  };
  
  /// destructor (virtual and protected)
  virtual ~GiGaExternalPhC() {};

private:

  // default constructor is provate 
  GiGaExternalPhC();
  // copy constructor is private 
  GiGaExternalPhC           ( const MyType& );
  // assigement operator is private 
  GiGaExternalPhC& operator=( const MyType& );
  
};

#endif // GIGA_GIGAEXTERNALPHC_H

