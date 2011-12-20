// $Id: $
#ifndef OTSIMULATION_IOTDOUBLEPULSETOOL_H
#define OTSIMULATION_IOTDOUBLEPULSETOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// MCEvent
#include "Event/MCOTDeposit.h"

static const InterfaceID IID_OTDoublePulseTool( "IOTDoublePulseTool", 1, 0 );

/** @class IOTDoublePulseTool IOTDoublePulseTool.h \
 *         "OTSimulation/IOTDoublePulseTool.h"
 *
 *  Outer tracker double pulse tool interface.
 *
 *  @author A Bien
 *  @date   17/10/2011
 */

class IOTDoublePulseTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTDoublePulseTool; }

  virtual double timeOffset(const LHCb::MCOTDeposit* deposit) const = 0;

};
#endif // OTSIMULATION_IOTDOUBLEPULSETOOL_H
