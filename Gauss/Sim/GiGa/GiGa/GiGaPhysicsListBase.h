// $Id: GiGaPhysicsListBase.h,v 1.2 2003-12-05 16:18:24 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
#ifndef       GIGA_GiGaPhysicsListBase_H
#define       GIGA_GiGaPhysicsListBase_H 1 
// ============================================================================
/// base classes 
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaBase.h"
#include "GiGa/GiGaFactory.h"

/** @class GiGaPhysicsListBase GiGaPhysicsListBase.h GiGa/GiGaPhysicListBase.h
 * 
 *  "implementation" of basis "Physics List"     
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaPhysicsListBase: 
  public virtual IGiGaPhysicsList   , 
  public          GiGaBase
{
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysicsListBase
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// destrucutor 
  virtual ~GiGaPhysicsListBase();
  
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
  
protected:
  
  /** an accessor for special cut for gamma 
   *  @return value of cut for gamma 
   */
  double    cutForGamma        () const { return m_cutForGamma ; }

  /** an accessor for special cut for electron 
   *  @return value of cut for electron
   */
  double    cutForElectron     () const { return m_cutForElectron ; }

  /** an accessor for special cut for positron
   *  @return value of cut for positron
   */
  double    cutForPositron     () const { return m_cutForPositron ; }  

private:
  
  //  the default constructor   is disabled 
  GiGaPhysicsListBase() ; 
  //  the copy constructor      is disabled 
  GiGaPhysicsListBase           ( const GiGaPhysicsListBase& ) ; 
  //  the assignement operator  is disabled 
  GiGaPhysicsListBase& operator=( const GiGaPhysicsListBase& ) ; 
  
private:
  
  // cut for gammas 
  double    m_cutForGamma       ;

  // cut for e-
  double    m_cutForElectron    ;

  // cut for e+
  double    m_cutForPositron    ;
  
};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysicsListBase_H
// ============================================================================





