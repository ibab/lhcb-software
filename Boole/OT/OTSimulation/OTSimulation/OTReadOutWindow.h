// $Id: OTReadOutWindow.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
#ifndef OTSIMULATION_OTREADOUTWINDOW_H
#define OTSIMULATION_OTREADOUTWINDOW_H 1

// Gaudi
#include "GaudiKernel/AlgTool.h"

// OTSimulation
#include "OTSimulation/IOTReadOutWindow.h"

/** @class OTReadOutWindow OTReadOutWindow.h "OTSimulation/OTReadOutWindow.h"
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

#endif // OTSIMULATION_OTREADOUTWINDOW_H
