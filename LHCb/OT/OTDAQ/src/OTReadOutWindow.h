// $Id: OTReadOutWindow.h,v 1.2 2004-11-10 13:02:08 jnardull Exp $
#ifndef OTDAQ_OTREADOUTWINDOW_H
#define OTDAQ_OTREADOUTWINDOW_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

/** @class OTReadOutWindow OTReadOutWindow.h OTDAQ/OTReadOutWindow.h
 *
 *  Tool for getting the read-out window parameters.
 *
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */

class OTReadOutWindow : public GaudiTool, virtual public IOTReadOutWindow {

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
