// $Id: OTReadOutWindow.cpp,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"

//CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

//OTSimulation
#include "OTSimulation/OTReadOutWindow.h"

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
  AlgTool( type, name, parent )
{
  m_startReadOutGate.push_back(28.*ns);
  m_startReadOutGate.push_back(30.*ns);
  m_startReadOutGate.push_back(32.*ns);

  declareProperty("startReadoutGate",  m_startReadOutGate);
  declareProperty("sizeOfReadoutGate", m_sizeOfReadOutGate = 50.*ns);

  declareInterface<IOTReadOutWindow>(this);
}

OTReadOutWindow::~OTReadOutWindow()
{
  // destructor
}
