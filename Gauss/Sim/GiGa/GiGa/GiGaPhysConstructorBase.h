// $Id: GiGaPhysConstructorBase.h,v 1.4 2004-04-20 04:26:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/04/06 18:49:45  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
#ifndef       GIGA_GiGaPhysConstructorBase_H
#define       GIGA_GiGaPhysConstructorBase_H 1 
// ============================================================================
/// base classes 
#include "GiGa/IGiGaPhysConstructor.h"
#include "GiGa/GiGaPhysicsConstructorBase.h"

class GiGaPhysConstructorBase: 
  public virtual IGiGaPhysConstructor   , 
  public          GiGaPhysicsConstructorBase
{
public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  /** finalize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 
  
  /** accessor to G4VPhysicsConstructor
   *  @see IGiGaPhysConstructor
   *  @see IGiGaPhysicsConstructor
   *  @see G4VPhysicsConstructor
   *  @return pointer to G4VPhysicsConstructor object
   */
  virtual G4VPhysicsConstructor* physicsConstructor ()  const ;

  virtual unsigned long release() ;
  
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysConstructorBase( const std::string& type   , 
                    const std::string& name   , 
                    const IInterface*  parent ) ;
  ///
  virtual ~GiGaPhysConstructorBase();

private:
  
  // the default constructor  is disabled 
  GiGaPhysConstructorBase() ;  
  // the copy constructor     is disabled 
  GiGaPhysConstructorBase           ( const GiGaPhysConstructorBase& ) ;
  // the assigenemtn operator is disabled 
  GiGaPhysConstructorBase& operator=( const GiGaPhysConstructorBase& ) ; 

private:

  mutable G4VPhysicsConstructor* m_self ;
  
};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysConstructorBase_H
// ============================================================================





