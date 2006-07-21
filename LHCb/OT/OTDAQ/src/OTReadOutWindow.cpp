// $Id: OTReadOutWindow.cpp,v 1.6 2006-07-21 08:04:27 janos Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "OTReadOutWindow.h"

/** @file OTReadOutWindow.cpp 
 *
 *  Implementation of OTReadOutWindow tool.
 *  
 *  @author Jeroen van Tilburg
 *  @date   11/11/2002
 */

// Declaration of the tool Factory
DECLARE_TOOL_FACTORY( OTReadOutWindow );

OTReadOutWindow::OTReadOutWindow(const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent) : 
  GaudiTool( type, name, parent )
{
  m_startReadOutGate.push_back(28.0*Gaudi::Units::ns);
  m_startReadOutGate.push_back(30.0*Gaudi::Units::ns);
  m_startReadOutGate.push_back(32.0*Gaudi::Units::ns);

  declareProperty("startReadoutGate",  m_startReadOutGate);
  declareProperty("sizeOfReadoutGate", m_sizeOfReadOutGate = 75.0*Gaudi::Units::ns);

  declareInterface<IOTReadOutWindow>(this);
}

OTReadOutWindow::~OTReadOutWindow()
{
  // destructor
}
