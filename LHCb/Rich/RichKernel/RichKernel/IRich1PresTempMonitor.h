
//-----------------------------------------------------------------------------
/** @file IRich1PresTempMonitor.h
 *
 *  Header file for tool interface : IRich1PresTempMonitor
 *
 *  CVS Log :-
 *  $Id: IRich1PresTempMonitor.h,v 1.2 2005-10-25 17:25:02 jonrob Exp $
 *
 *  @author Sajan EASO
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICH1PRESTEMPMONITOR_H
#define RICHKERNEL_IRICH1PRESTEMPMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations (no other includes should be in interfaces)
class IRich1C4F10Property;
class TabulatedProperty;
class TimePoint;

static const InterfaceID IID_IRich1PresTempMonitor ( "IRich1PresTempMonitor", 1, 0 );

//-----------------------------------------------------------------------------------------
/** @class IRich1PresTempMonitor IRich1PresTempMonitor.h RichKernel/IRich1PresTempMonitor.h
 *
 *  Interface to tool that handles updates of RICH1 tabulated properties 
 *  when conditions change.
 *
 *  It is sufficent to simply instantiate this tool, there are no user interface methods.
 *  
 *  @author Sajan EASO
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------------------

class IRich1PresTempMonitor : virtual public IAlgTool
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRich1PresTempMonitor; }

};

#endif // RICHKERNEL_IRICH1PRESTEMPMONITOR_H
