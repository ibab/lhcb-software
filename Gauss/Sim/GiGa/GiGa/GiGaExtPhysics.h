// $Id: GiGaExtPhysics.h,v 1.7 2010-03-08 17:58:53 silviam Exp $
#ifndef GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 
#define GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 1

// Include files
#include "GiGa/GiGaPhysicsConstructorBase.h"

/// Class to allow the extension of GiGaExtPhysics via template specialization
template <class PhysConstr>
class GiGaExtPhysicsExtender {
public:
  /// Method to override in the template specialization to add extra properties
  //  to GiGaExtPhysics.
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // -- to be overridden in a template specialization to declare specific properties.
    // tool->declareProperty("BooleanProp", m_boolProp = true, "A specific boolean property");
  }
  /// Factory function for the actual G4VPhysicsConstructor.
  //  The specialization should implement it to take care of the special cases,
  //  like extra parameters.
  inline PhysConstr *newInstance(const std::string &name, int /*verbosity*/) const {
    // -- the specialization should implement this method to correctly pass the
    //    arguments to the "PhysConstr" constructor. By default the default
    //    constructor is used and the name is passed afterwards.
    PhysConstr *tmp = new PhysConstr;
    tmp->SetPhysicsName(name);
    //tmp->SetVerboseLevel(verbosity);
    return tmp;
  }
private:
  // -- each specialization must provide the storage for the specific properties
  // bool m_boolProp;
};

/** @class GiGaExtPhysics GiGaExtPhysics.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */

template <class PhysConstr>
class GiGaExtPhysics : public GiGaPhysicsConstructorBase
{
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
      // use the extender to create the new instance, passing the two standard
      // properties, the extender (if specialized) will pass some specific
      // arguments to the actual constructor.

      int verb = outputLevel();
      if      (verb == MSG::DEBUG)    { verb = 1 ;} 
      else if (verb == MSG::VERBOSE)  { verb = 2 ;} 
      else                            { verb = 0 ;} 

      m_phys = m_extender.newInstance(name(), verb);
      // m_phys -> SetPhysicsName( name() );
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
      , m_extender()
  {
    m_extender.addPropertiesTo(this); // this add the properties defined in the extender
  }
  
  /// destructor 
  virtual ~GiGaExtPhysics(){} ;
  
private:
  
  // physics constructor itself 
  mutable G4VPhysicsConstructor* m_phys ;

  // Object to allow extended configuration of the G4 classes through template
  // specialization.
  GiGaExtPhysicsExtender<PhysConstr> m_extender;

};

#endif // GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H
