// $Id: GiGaPhysicsListBase.h,v 1.1 2003-04-06 18:49:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
  
  /** an accessor to the default cut value 
   *  @return value of defauld cut 
   */ 
  double    defCut             () const { return defaultCut()  ; } 
  
  /** an accessor to the default cut value 
   *  @return value of defautl cut 
   */ 
  double    defaultCut         () const { return m_defaultCut ; } 
  
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
  
  /** an accessor for special cut for proton
   *  @return value of cut for proton
   */
  double    cutForProton       () const { return m_cutForProton   ; }
  
  /** an accessor for special cut for anti_proton
   *  @return value of cut for anti_proton
   */
  double    cutForAntiProton   () const { return m_cutForAntiProton  ; }  

  /** an accessor for special cut for neutron
   *  @return value of cut for neutron
   */
  double    cutForNeutron      () const { return m_cutForNeutron     ; }
  
  /** an accessor for special cut for anti_neutron
   *  @return value of cut for anti_neutron
   */
  double    cutForAntiNeutron  () const { return m_cutForAntiNeutron ; }

private:
  
  //  the default constructor   is disabled 
  GiGaPhysicsListBase() ; 
  //  the copy constructor      is disabled 
  GiGaPhysicsListBase           ( const GiGaPhysicsListBase& ) ; 
  //  the assignement operator  is disabled 
  GiGaPhysicsListBase& operator=( const GiGaPhysicsListBase& ) ; 
  
private:
  
  // default cut 
  double    m_defaultCut        ; 

  // cut for gammas 
  double    m_cutForGamma       ;

  // cut for e-
  double    m_cutForElectron    ;

  // cut for e+
  double    m_cutForPositron    ;

  // cut for proton
  double    m_cutForProton      ;

  // cut for anti_proton
  double    m_cutForAntiProton  ;

  // cut for neutron
  double    m_cutForNeutron     ;

  // cut for anti_netron
  double    m_cutForAntiNeutron ;
  
};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysicsListBase_H
// ============================================================================





