// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:11:46  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef     GIGA_IGiGaTrackActionFACTORY_H
#define     GIGA_IGiGaTrackActionFACTORY_H 1
// ============================================================================
// GaudiKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaTrackActionFactory.h"
//
class ISvcLocator;
class IGiGaTrackAction;
//

/** @class IGiGaTrackActionFactory IGiGaTrackActionFactory.h 
 *
 *  The IGiGaTrackActionFactory is the interface used to 
 *    create contrete instances of GiGa Track Action 
 *  
 *  @author Vanya Belyaev
 *  @author 23/01/2001
 */

class IGiGaTrackActionFactory : virtual public IFactory 
{
  ///
public:
  
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () 
  { return IID_IGiGaTrackActionFactory; }
  
  /// Create an instance of a concrete GiGaTrackAction
  virtual IGiGaTrackAction* instantiate ( const std::string& ,  
                                          ISvcLocator* ) const = 0;
  
  /// Access to the GiGaTrackAction type.
  virtual const std::string&  trackActionType ()    const  = 0;

  /// destructor 
  virtual ~IGiGaTrackActionFactory(){};

};

///
///

// ============================================================================
#endif   ///< GIGA_IGiGaTrackActionFACTORY_H
// ============================================================================

