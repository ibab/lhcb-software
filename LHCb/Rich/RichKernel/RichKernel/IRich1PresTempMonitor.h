// $Id: IRich1PresTempMonitor.h,v 1.1 2005-10-07 09:00:11 jonrob Exp $
#ifndef RICHCONDITION_IRICH1PRESTEMPMONITOR_H
#define RICHCONDITION_IRICH1PRESTEMPMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations (no other includes should be in interfaces)
class IRich1C4F10Property;
class TabulatedProperty;
class TimePoint;

static const InterfaceID IID_IRich1PresTempMonitor ( "IRich1PresTempMonitor", 1, 0 );

/** @class IRich1PresTempMonitor IRich1PresTempMonitor.h RichKernel/IRich1PresTempMonitor.h
 *
 *
 *  @author Sajan EASO
 *  @date   2005-08-26
 */

class IRich1PresTempMonitor : virtual public IAlgTool
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRich1PresTempMonitor; }

  virtual StatusCode Rich1C4F10MaterialGasPropertyUpdate() = 0;
  virtual StatusCode Rich1C4F10MaterialRefIndexUpdate()    = 0;
  virtual StatusCode Rich1C4F10PressureUpdate()            = 0;
  virtual StatusCode Rich1C4F10TemperatureUpdate()         = 0;

};

#endif // RICHCONDITION_IRICH1PRESTEMPMONITOR_H
