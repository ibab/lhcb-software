// $Id: OTReadOutWindow.cpp,v 1.5 2006-03-30 21:49:02 janos Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"

// MathCore
#include "Kernel/SystemOfUnits.h"

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
  m_startReadOutGate.push_back(28.*ns);
  m_startReadOutGate.push_back(30.*ns);
  m_startReadOutGate.push_back(32.*ns);

  declareProperty("startReadoutGate",  m_startReadOutGate);
  declareProperty("sizeOfReadoutGate", m_sizeOfReadOutGate = 75.*ns);

  declareInterface<IOTReadOutWindow>(this);
}

OTReadOutWindow::~OTReadOutWindow()
{
  // destructor
}
