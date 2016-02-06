#ifndef OTDAQ_OTREADOUTWINDOW_H
#define OTDAQ_OTREADOUTWINDOW_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

#include <DetDesc/Condition.h>

/** @class OTReadOutWindow OTReadOutWindow.h OTDAQ/OTReadOutWindow.h
 *
 *  Tool for getting the read-out window parameters.
 *
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */

class OTReadOutWindow : public extends<GaudiTool, IOTReadOutWindow> {

public:

  ///  Constructor
  OTReadOutWindow(const std::string& type, 
                  const std::string& name, 
                  const IInterface* parent);

  /// destructor
  ~OTReadOutWindow() override;

  StatusCode initialize() override;

  /// Start of read out window for each station
  std::vector<double> startReadOutGate() const override;

  /// Size of the read out window
  double sizeOfReadOutGate() const override;

private:

  std::vector<double> m_startReadOutGate;  ///< start of readout gate
  double m_sizeOfReadOutGate;              ///< end of readout gate

  Condition* condReadOutGate = nullptr;

  StatusCode condReadOutGateUpdate();
};

inline std::vector<double> OTReadOutWindow::startReadOutGate() const
{
  return m_startReadOutGate;
}

inline double OTReadOutWindow::sizeOfReadOutGate() const
{
  return m_sizeOfReadOutGate;
}

#endif // OTDAQ_OTREADOUTWINDOW_H
