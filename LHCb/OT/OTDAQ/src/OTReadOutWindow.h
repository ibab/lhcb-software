// $Id: OTReadOutWindow.h,v 1.1 2004-09-10 13:14:59 cattanem Exp $
#ifndef OTDAQ_OTREADOUTWINDOW_H
#define OTDAQ_OTREADOUTWINDOW_H 1

// Gaudi
#include "GaudiKernel/AlgTool.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

/** @class OTReadOutWindow OTReadOutWindow.h OTDAQ/OTReadOutWindow.h
 *
 *  Tool for getting the read-out window parameters.
 *
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */

class OTReadOutWindow : public AlgTool, virtual public IOTReadOutWindow {

public:

  ///  Constructor
  OTReadOutWindow(const std::string& type, 
                  const std::string& name, 
                  const IInterface* parent);

  /// destructor
  virtual ~OTReadOutWindow();

  /// Start of read out window for each station
  std::vector<double> startReadOutGate();

  /// Size of the read out window
  double sizeOfReadOutGate();

private:

  std::vector<double> m_startReadOutGate;  ///< start of readout gate
  double m_sizeOfReadOutGate;              ///< end of readout gate

};

inline std::vector<double> OTReadOutWindow::startReadOutGate()
{
  return m_startReadOutGate;
}

inline double OTReadOutWindow::sizeOfReadOutGate() 
{
  return m_sizeOfReadOutGate;
}

#endif // OTDAQ_OTREADOUTWINDOW_H
