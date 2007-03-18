// $Id: GiGaStepActionBase.h,v 1.12 2007-03-18 18:25:05 gcorti Exp $ 
#ifndef     GIGA_GiGaStepActionBase_H
#define     GIGA_GiGaStepActionBase_H 1 

// Include files
// STL
#include <vector> 
// GiGa
#include "GiGa/IGiGaStepAction.h" 
#include "GiGa/GiGaBase.h" 

// Forward declarations
class G4Step;


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

public:  
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
  
  virtual ~GiGaStepActionBase();  ///< Destructor

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

  GiGaStepActionBase ();///< no default constructor!
  GiGaStepActionBase           ( const GiGaStepActionBase& ); ///< no copy 
  GiGaStepActionBase& operator=( const GiGaStepActionBase& ); ///< no = 

private:

};

#endif   // GIGA_GiGaStepActionBase_H















