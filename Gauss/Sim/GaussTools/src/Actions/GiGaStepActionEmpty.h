// $Id: GiGaStepActionEmpty.h,v 1.1 2002-09-26 18:10:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef       GIGA_GiGaStepActionEmpty_H
#define       GIGA_GiGaStepActionEmpty_H 1 
// ============================================================================
// include files 
// GiGa
#include "GiGa/GiGaStepActionBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;

/** @class GiGaStepActionEmpty GiGaStepActionEmpty.h
 *   
 *  Example of "primitive" implementation of Stepping action class
 *  It is just empty!
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class GiGaStepActionEmpty: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<GiGaStepActionEmpty>;

protected:

  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaStepActionEmpty
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaStepActionEmpty();

public:
  
  /** stepping action
   *  @see G4UserSteppingAction
   *  @param step Geant4 step
   */
  virtual void UserSteppingAction ( const G4Step* );
  
private:

  GiGaStepActionEmpty(); ///< no default constructor
  GiGaStepActionEmpty( const GiGaStepActionEmpty& ); ///< no copy  
  GiGaStepActionEmpty& operator=( const GiGaStepActionEmpty& ) ; ///< no = 

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GIGA_GiGaStepActionEmpty_H
// ============================================================================
