// $Id: GiGaStepActionDraw.h,v 1.2 2007-01-12 15:36:56 ranjard Exp $ 
#ifndef       GIGA_GiGaStepActionDraw_H
#define       GIGA_GiGaStepActionDraw_H 1 

/// GiGa 
#include "GiGa/GiGaStepActionBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;
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
  //friend class GiGaFactory<GiGaStepActionDraw>;
  
public:
  //protected:
  
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
