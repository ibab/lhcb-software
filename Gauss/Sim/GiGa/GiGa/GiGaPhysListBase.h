// $Id: GiGaPhysListBase.h,v 1.10 2004-04-20 04:26:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2003/04/06 18:49:45  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// Revision 1.8  2002/05/07 12:21:29  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef       GIGA_GiGaPhysListBase_H
#define       GIGA_GiGaPhysListBase_H 1 
// ============================================================================
/// base classes 
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaPhysicsListBase.h"

/** @class GiGaPhysListBase GiGaPhysListBase.h GiGa/GiGaPhysListBase.h
 * 
 *  "implementation" of basis "Physics List"     
 *  it still has 3 pure abstract methods from G4VUserPhysicsList! 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

class GiGaPhysListBase: 
  public virtual IGiGaPhysList   , 
  public          GiGaPhysicsListBase
{
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysListBase( const std::string& type   , 
                    const std::string& name   , 
                    const IInterface*  parent ) ;
  ///
  virtual ~GiGaPhysListBase();

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
  
  /** get G4VUserPhysicsList 
   *  @see IGiGaPhysList
   *  @see IGiGaPhysicsList
   *  @see G4VUserPhysicsList
   *  @return pointer to G4VUserPhysicsList object
   */
  virtual G4VUserPhysicsList* physicsList ()  const ;
  
  /** "SetCuts" method sets a cut value for all particle types 
   *   in the particle table
   *   @see IGiGaPhysList 
   *   @see IGiGaPhysicsList 
   *   @see G4VUserPhysicsList 
   */
  virtual void SetCuts () ;

public:

  virtual unsigned long release() ;
  
private:

  // the default constructor is disabled 
  GiGaPhysListBase() ;
  // the copy constructor is disabled 
  GiGaPhysListBase           ( const GiGaPhysListBase& ) ; 
  // the assignement operator is disabled
  GiGaPhysListBase& operator=( const GiGaPhysListBase& ) ; 

private:

  mutable G4VUserPhysicsList* m_self ;
  
};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysListBase_H
// ============================================================================





