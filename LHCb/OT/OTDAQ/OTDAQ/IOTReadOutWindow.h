// $Id: IOTReadOutWindow.h,v 1.1 2004-09-10 13:14:58 cattanem Exp $
#ifndef OTSIMULATION_IOTREADOUTWINDOW_H 
#define OTSIMULATION_IOTREADOUTWINDOW_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_OTReadOutWindow( "IOTReadOutWindow", 1, 0 );

/** @class IOTReadOutWindow IOTReadOutWindow.h "OTSimulation/IOTReadOutWindow.h"
 *  
 *  Outer tracker ReadOutWindow interface.
 *
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */
class IOTReadOutWindow : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTReadOutWindow; }
  /// Start of read out window for each station
  virtual std::vector<double> startReadOutGate() = 0;
  /// Size of the read out window
  virtual double sizeOfReadOutGate() = 0;

};
#endif // OTSIMULATION_IOTREADOUTWINDOW_H
