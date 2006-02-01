
//-----------------------------------------------------------------------------
/** @file IRich2PresTempMonitor.h
 *
 *  Header file for tool interface : IRich2PresTempMonitor
 *
 *  CVS Log :-
 *  $Id: IRich2PresTempMonitor.h,v 1.3 2006-02-01 19:46:30 marcocle Exp $
 *
 *  @author Sajan EASO
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICH2PRESTEMPMONITOR_H 
#define RICHKERNEL_IRICH2PRESTEMPMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations (no other includes should be in interfaces)
class IRich2CF4Property;
class TabulatedProperty;

static const InterfaceID IID_IRich2PresTempMonitor ( "IRich2PresTempMonitor", 1, 0 );

//-----------------------------------------------------------------------------------------
/** @class IRich2PresTempMonitor IRich2PresTempMonitor.h RichKernel/IRich2PresTempMonitor.h
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

class IRich2PresTempMonitor : virtual public IAlgTool 
{

public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRich2PresTempMonitor; }

};

#endif // RICHKERNEL_IRICH2PRESTEMPMONITOR_H
