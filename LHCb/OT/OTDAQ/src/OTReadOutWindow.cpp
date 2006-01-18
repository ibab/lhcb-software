// $Id: OTReadOutWindow.cpp,v 1.4 2006-01-18 14:05:21 janos Exp $

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

static ToolFactory<OTReadOutWindow> s_factory;
const IToolFactory& OTReadOutWindowFactory = s_factory;


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
