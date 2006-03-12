// $Id: IRich1AerogelMonitor.h,v 1.2 2006-03-12 16:12:37 jonrob Exp $

#ifndef RICHCONDITION_IRICH1AEROGELMONITOR_H 
#define RICHCONDITION_IRICH1AEROGELMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations (no other includes should be in interfaces)
  
class IRich1AerogelProperty;
class TabulatedProperty;
class TimePoint;

static const InterfaceID IID_IRich1AerogelMonitor ( "IRich1AerogelMonitor", 1, 0 );

/** @class IRich1AerogelMonitor IRich1AerogelMonitor.h RichCondition/IRich1AerogelMonitor.h
 *  
 *
 *  @author Marco Musy
 *  @date   2006-01-16
 */
class IRich1AerogelMonitor : virtual public IAlgTool 
{

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRich1AerogelMonitor; } 

};

#endif // RICHCONDITION_IRICH1AEROGELMONITOR_H
