// $Id: GiGaPhysListBase.h,v 1.8 2002-05-07 12:21:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GIGA_GiGaPhysListBase_H
#define       GIGA_GiGaPhysListBase_H 1 
// ============================================================================
/// base classes 
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaBase.h"

/** @class GiGaPhysListBase GiGaPhysListBase.h GiGa/GiGaPhysListBase.h
 * 
 *  "implementation" of basis "Physics List"     
 *  it still has 3 pure abstract methods from G4VUserPhysicsList! 
 *
 *  @author Vanya Belyaev
 */

class GiGaPhysListBase: 
  public virtual IGiGaPhysList   , 
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
  
protected:
  ///
  double    defCut() const { return m_DefaultCutValue; } 
  ///
private:
  ///
  GiGaPhysListBase() ; ///< no default constructor 
  GiGaPhysListBase           ( const GiGaPhysListBase& ) ; //< no copy
  GiGaPhysListBase& operator=( const GiGaPhysListBase& ) ; //< no assignment  
  ///
private:

  double    m_DefaultCutValue  ; //default cut value

};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysListBase_H
// ============================================================================





