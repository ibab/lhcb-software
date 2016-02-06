#ifndef OTSIMULATION_IOTREADOUTWINDOW_H 
#define OTSIMULATION_IOTREADOUTWINDOW_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

/** @class IOTReadOutWindow IOTReadOutWindow.h "OTSimulation/IOTReadOutWindow.h"
 *  
 *  Outer tracker ReadOutWindow interface.
 *
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */
class IOTReadOutWindow : public extend_interfaces<IAlgTool> {
public:
  /// Retrieve interface ID
  DeclareInterfaceID(IOTReadOutWindow, 2, 0 );
  /// Start of read out window for each station
  virtual std::vector<double> startReadOutGate() const = 0;
  /// Size of the read out window
  virtual double sizeOfReadOutGate() const = 0;

};
#endif // OTSIMULATION_IOTREADOUTWINDOW_H
