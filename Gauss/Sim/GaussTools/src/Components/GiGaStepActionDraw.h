// $Id: GiGaStepActionDraw.h,v 1.1 2002-12-12 15:19:32 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:53  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.6  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef       GIGA_GiGaStepActionDraw_H
#define       GIGA_GiGaStepActionDraw_H 1 
// ============================================================================
/// GiGa 
#include "GiGa/GiGaStepActionBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class GiGaStepActionDraw GiGaStepActionDraw.h
 *  
 *  Example of "primitive" implementation of Stepping action class
 *  It performs visualization of steps!
 *
 *  @author  Vanya Belyaev
 *   @date    17/03/2001
 */


class GiGaStepActionDraw: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaStepActionDraw>;
  
protected:
  
  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaStepActionDraw
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected) 
  virtual ~GiGaStepActionDraw();
  
public: 
  
  /** the "main" method
   *  @see   G4UserSteppingAction
   *  @param aStep  pointer to current step object
   */
  virtual void UserSteppingAction( const G4Step* aStep );
  
private:

  GiGaStepActionDraw() ; ///< no default constructor
  GiGaStepActionDraw( const GiGaStepActionDraw& ); ///< no copy constructor 
  GiGaStepActionDraw& operator=( const GiGaStepActionDraw& ) ; // no assignment 

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GIGA_GiGaStepActionDraw_H
// ============================================================================
