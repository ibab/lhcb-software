// $Id: GiGaPhysicsConstructorBase.h,v 1.2 2004-04-20 04:26:05 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
#ifndef GIGA_GIGAPHYSICSCONSTRUCTORBASE_H 
#define GIGA_GIGAPHYSICSCONSTRUCTORBASE_H 1
// ============================================================================
// Include files
#include "GiGa/IGiGaPhysicsConstructor.h"
#include "GiGa/GiGaBase.h"

/** @class GiGaPhysicsConstructorBase GiGaPhysicsConstructorBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaPhysicsConstructorBase :
  public virtual IGiGaPhysicsConstructor , 
  public GiGaBase
{

public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () { return GiGaBase::initialize () ; }
  
  /** finalize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () { return GiGaBase::finalize   () ; }

protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysicsConstructorBase
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// destructor
  virtual ~GiGaPhysicsConstructorBase() ;
  
private:
  
  // the default constructor is disabled 
  GiGaPhysicsConstructorBase () ;
  // the copy  constructor is disabled 
  GiGaPhysicsConstructorBase            ( const  GiGaPhysicsConstructorBase& ) ;
  // the assignement operator is disabled 
  GiGaPhysicsConstructorBase& operator= ( const  GiGaPhysicsConstructorBase& ) ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GIGA_GIGAPHYSICSCONSTRUCTORBASE_H
// ============================================================================
