// ====a========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/25 18:09:38  ibelyaev
/// bug fix
///
/// Revision 1.4  2001/07/23 13:12:26  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef       GIGA_GiGaEventActionEmpty_H
#define       GIGA_GiGaEventActionEmpty_H 1 
// ============================================================================

#include "GiGa/GiGaEventActionBase.h"
template <class EA> 
class GiGaEventActionFactory;

/** @class GiGaEventActionEmpty GiGaEventActionEmpty.h 
 *    
 *  Example of "primitive" implementation of Event Action class
 *  It is just empty!
 * 
 *  @author  Vanya Belyaev
 *  @date    17/03/2001
*/

class GiGaEventActionEmpty: virtual public GiGaEventActionBase
{
  ///
  friend class GiGaEventActionFactory<GiGaEventActionEmpty>;
  ///
 protected:
  ///
  GiGaEventActionEmpty( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaEventActionEmpty();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  /// 
 private:
  ///
  GiGaEventActionEmpty() ; ///< no default constructor
  GiGaEventActionEmpty( const GiGaEventActionEmpty& ) ; ///< no copy  
  GiGaEventActionEmpty& operator=( const GiGaEventActionEmpty& ) ; ///< no = 
  ///
 private:
  ///
};

// ============================================================================
#endif  ///<  GIGA_GiGaEventActionEmpty_H
// ============================================================================
