// $Id: GiGaPhysicsListBase.h,v 1.4 2007-03-18 18:25:05 gcorti Exp $
#ifndef       GIGA_GiGaPhysicsListBase_H
#define       GIGA_GiGaPhysicsListBase_H 1 

// Include files
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaBase.h"

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

#endif    ///<  GIGA_GiGaPhysicsListBase_H





