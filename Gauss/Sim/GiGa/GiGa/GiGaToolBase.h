// $Id: GiGaToolBase.h,v 1.1 2003-04-06 18:49:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGATOOLBASE_H 
#define GIGA_GIGATOOLBASE_H 1
// ============================================================================
// Include files
#include "GiGa/IGiGaTool.h"
#include "GiGa/GiGaBase.h"
// ============================================================================

/** @class GiGaToolBase GiGaToolBase.h GiGa/GiGaToolBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaToolBase :
  public virtual IGiGaTool , 
  public         GiGaBase
{
public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize  () {   return GiGaBase::initialize () ; }
  
  /** finalize the object  
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize    () {   return GiGaBase::finalize   () ; }
  
  
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaToolBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  virtual ~GiGaToolBase ();
  
private:
  
  // the default constructor is disabled 
  GiGaToolBase () ;
  // the copy     constructor is disabled 
  GiGaToolBase           ( const GiGaToolBase& ) ;
  // the assignement operator is disabled 
  GiGaToolBase& operator=( const GiGaToolBase& ) ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GIGA_GIGATOOLBASE_H
// ============================================================================
