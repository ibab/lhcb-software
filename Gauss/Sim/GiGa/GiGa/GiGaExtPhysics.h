// $Id: GiGaExtPhysics.h,v 1.1 2004-02-17 18:26:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:45  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
#ifndef GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 
#define GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H 1
// ============================================================================
// Include files
#include "GiGa/GiGaPhysicsConstructorBase.h"
#include "GiGa/GiGaFactory.h"

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
  /// owntype 
  typedef GiGaExtPhysics<PhysConstr> OwnType ;
  /// friend factory for instantiation
  friend class GiGaFactory<OwnType> ;
  
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
  
protected:
  
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
  
  // the default constructor is disabled 
  GiGaExtPhysics () ;
  // the copy  constructor is disabled 
  GiGaExtPhysics           ( const GiGaExtPhysics& ) ;
  // the assigenement operator is disabled 
  GiGaExtPhysics& operator=( const GiGaExtPhysics& ) ;
  
private:
  
  // physics constructor itself 
  mutable G4VPhysicsConstructor* m_phys ;

};
// ============================================================================

// ============================================================================
/** @def IMPLEMENT_ExtPhysics
 *  useful macro to implement a static factory for
 *  instantiation of external("imported") physics list
 *  @see IMPLEMENT_GiGaPhysList
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
// ============================================================================
#define IMPLEMENT_ExtPhysics( PC )                         \
 static const GiGaFactory<GiGaExtPhysics<PC> > s_##PC##F ; \
 const           IFactory&PC##Factory        = s_##PC##F ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAEXTERNALPHYSICSCONSTRUCTOR_H
// ============================================================================
