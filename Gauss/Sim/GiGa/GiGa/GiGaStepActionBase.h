// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.7  2001/07/23 13:11:43  ibelyaev
/// the package restructurisation(II)
/// 
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
 *  @author  Vanya Belyaev
 *   @date    23/01/2001
 */

class GiGaStepActionBase: virtual public IGiGaStepAction   ,
                          public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaStepActionBase( const std::string& , ISvcLocator* ) ;
  virtual ~GiGaStepActionBase();
  ///
public:
  /// ident 
  virtual const std::string&  name () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode          initialize () ; 
  /// finalize 
  virtual StatusCode          finalize   () ; 
  /// Query Interface
  virtual StatusCode          queryInterface(const InterfaceID& , void** ) ;
  ///
public:
  ///
  virtual void UserSteppingAction ( const G4Step* );
  ///
protected:
  ///
  GiGaStepActionBase ();///< no default constructor!
  GiGaStepActionBase           ( const GiGaStepActionBase& ); ///< no copy 
  GiGaStepActionBase& operator=( const GiGaStepActionBase& ); ///< no = 
  ///
private:
  ///
};
///

// ============================================================================
#endif   // GIGA_GiGaStepActionBase_H
// ============================================================================














