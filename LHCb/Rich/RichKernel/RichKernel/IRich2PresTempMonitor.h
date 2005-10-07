// $Id: IRich2PresTempMonitor.h,v 1.1 2005-10-07 09:00:11 jonrob Exp $
#ifndef RICHCONDITION_IRICH2PRESTEMPMONITOR_H 
#define RICHCONDITION_IRICH2PRESTEMPMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations (no other includes should be in interfaces)
class IRich2CF4Property;
class TabulatedProperty;
class TimePoint;

static const InterfaceID IID_IRich2PresTempMonitor ( "IRich2PresTempMonitor", 1, 0 );

/** @class IRich2PresTempMonitor IRich2PresTempMonitor.h RichKernel/IRich2PresTempMonitor.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2005-08-26
 */

class IRich2PresTempMonitor : virtual public IAlgTool 
{

public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRich2PresTempMonitor; }

  virtual StatusCode Rich2CF4MaterialGasPropertyUpdate() = 0;
  virtual StatusCode Rich2CF4MaterialRefIndexUpdate()    = 0;
  virtual StatusCode Rich2CF4PressureUpdate()            = 0;
  virtual StatusCode Rich2CF4TemperatureUpdate()         = 0;

};

#endif // RICHCONDITION_IRICH2PRESTEMPMONITOR_H
