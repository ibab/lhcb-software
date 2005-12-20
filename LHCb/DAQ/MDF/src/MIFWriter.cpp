// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MIFWriter.cpp,v 1.1 2005-12-20 16:33:39 frankb Exp $
//	====================================================================
//  MIFWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MDF/StorageTypes.h"
#include "MDF/MIFWriter.h"
#include "Event/RawEvent.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MIFWriter)

/// Standard algorithm constructor
LHCb::MIFWriter::MIFWriter(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
  declareProperty("Connection", m_connectParams="");
}

/// Standard Destructor
LHCb::MIFWriter::~MIFWriter()     {
}

/// Initialize
StatusCode LHCb::MIFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  m_connection = Descriptor::connect(m_connectParams);
  if ( m_connection.ioDesc > 0 )  {
    log << MSG::INFO << "Received event request connection." << endmsg;
  }
  else {
    log << MSG::INFO << "FAILED receiving event request connection." << endmsg;
  }
  return (m_connection.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode LHCb::MIFWriter::finalize() {
  Descriptor::close(m_connection);
  return StatusCode::SUCCESS;
}

// Execute procedure
StatusCode LHCb::MIFWriter::execute()    {
  SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
  if ( evt )  {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
