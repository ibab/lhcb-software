// $Id: GiGaStepActionBase.h,v 1.10 2002-05-07 12:21:30 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_GiGaStepActionBase_H
#define     GIGA_GiGaStepActionBase_H 1 
// ============================================================================
// STL
#include <vector> 
// GiGa
#include "GiGa/IGiGaStepAction.h" 
#include "GiGa/GiGaBase.h" 
//
class G4Step;
//

/** @class GiGaStepActionBase GiGaStepActionBase.h
 *
 *  Base class for implementation of concrete Steping Action for GiGa
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class GiGaStepActionBase:
  public virtual IGiGaStepAction ,
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
  GiGaStepActionBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  virtual ~GiGaStepActionBase();

public:
  
  /** initialize the step action  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize () ; 
  
  /** finalize the step action
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         finalize   () ; 

protected:
  ///
  GiGaStepActionBase ();///< no default constructor!
  GiGaStepActionBase           ( const GiGaStepActionBase& ); ///< no copy 
  GiGaStepActionBase& operator=( const GiGaStepActionBase& ); ///< no = 
  ///
private:
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   // GIGA_GiGaStepActionBase_H
// ============================================================================














