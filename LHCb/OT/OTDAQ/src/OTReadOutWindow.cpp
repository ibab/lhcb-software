// Gaudi files
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
DECLARE_TOOL_FACTORY( OTReadOutWindow )

OTReadOutWindow::OTReadOutWindow(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent) :
  base_class( type, name, parent )
{
  declareInterface<IOTReadOutWindow>(this);
  declareProperty("startReadoutGate",  m_startReadOutGate= { 28.0*Gaudi::Units::ns,
                                                             30.0*Gaudi::Units::ns,
                                                             32.0*Gaudi::Units::ns });
  declareProperty("sizeOfReadoutGate", m_sizeOfReadOutGate = 75.0*Gaudi::Units::ns);
}

OTReadOutWindow::~OTReadOutWindow() = default;

StatusCode OTReadOutWindow::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  if(existDet<DataObject>(detSvc(), "Conditions/Calibration/OT/ReadOutGates"))
  {
    try
    {
      registerCondition("Conditions/Calibration/OT/ReadOutGates", condReadOutGate, &OTReadOutWindow::condReadOutGateUpdate);
      StatusCode statusUpdate = runUpdate();
      if(statusUpdate.isFailure())
        return Error("Unable to update conditions from database.", statusUpdate);
    }
    catch(GaudiException &ex)
    {
      fatal() << ex << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode OTReadOutWindow::condReadOutGateUpdate()
{
  std::vector<double> readoutGates = condReadOutGate->paramVect<double>("ReadOutGates");
  m_startReadOutGate[0] = readoutGates[0];
  m_startReadOutGate[1] = readoutGates[1];
  m_startReadOutGate[2] = readoutGates[2];

  if(msgLevel(MSG::DEBUG))
  {
    debug() << " :: readOutGateT1 = " << readoutGates[0] << endmsg;
    debug() << " :: readOutGateT2 = " << readoutGates[1] << endmsg;
    debug() << " :: readOutGateT3 = " << readoutGates[2] << endmsg;
  }

  return StatusCode::SUCCESS;
}
