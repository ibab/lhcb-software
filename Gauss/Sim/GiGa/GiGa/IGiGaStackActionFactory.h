// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:11:46  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef     GIGA_IGiGaStackActionFACTORY_H
#define     GIGA_IGiGaStackActionFACTORY_H 1
// ============================================================================
// GauidKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaStackActionFactory.h"
// 
class ISvcLocator;
class IGiGaStackAction;


/** @class IGiGaStackActionFactory IGiGaStackActionFactory.h
 *    
 *  The IGiGaStackActionFactory is the interface used to 
 *     create concrete instances of GiGa Stack Action 
 *  
 *  @author Vanya Belyaev
 *  @author 23/01/2001
 */

class IGiGaStackActionFactory : virtual public IFactory 
{
  ///
 public:

  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () 
  { return IID_IGiGaStackActionFactory; }

  /// Create an instance of a concrete GiGaStackAction
  virtual IGiGaStackAction* instantiate ( const std::string& ,  
                                          ISvcLocator* ) const = 0;

  /// Access to the GiGaStackAction type.
  virtual const std::string&  stackActionType ()    const  = 0;
  ///
};
///

// ============================================================================
#endif   ///< GIGA_IGiGaStackActionFACTORY_H
// ============================================================================

