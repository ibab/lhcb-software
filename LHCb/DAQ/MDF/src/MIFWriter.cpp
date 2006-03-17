// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MIFWriter.cpp,v 1.2 2006-03-17 17:23:56 frankb Exp $
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
#include "GaudiKernel/IRegistry.h"
#include "MDF/RawDataAddress.h"
#include "MDF/StorageTypes.h"
#include "MDF/MIFWriter.h"
#include "MDF/MIFHeader.h"
#include "Event/RawEvent.h"
#include <cerrno>
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MIFWriter)

typedef std::pair<LHCb::MIFHeader*,LHCb::MIFHeader::Event*> MIFDesc;

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
  SmartDataPtr<DataObject> raw(eventSvc(),LHCb::RawEventLocation::Default);
  if ( raw )  {
    IRegistry* pReg = raw->registry();
    if ( pReg )  {
      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pReg->address());
      if ( pA )  {
        char memory[1024];
        const std::string& fname = pA->par()[0];
        FidMap::const_iterator i = m_fidMap.find(fname);
        if ( i == m_fidMap.end() )   {
          m_fidMap.insert(std::make_pair(fname,m_fidMap.size()));
          MIFHeader* hdr = MIFHeader::create(memory, fname, m_fidMap.size());
          if ( !Descriptor::write(m_connection, hdr, hdr->totalSize()) )  {
            MsgStream log(msgSvc(),name());
            log << MSG::ERROR << "Failed to write File record for " << fname << endmsg;
            return StatusCode::FAILURE;
          }
        }
        int fid = (*m_fidMap.find(fname)).second;
        MIFDesc dsc = MIFHeader::create<MIFHeader::Event>(memory, fid, MIFHeader::MIF_EVENT);
        dsc.second->setOffset(pA->fileOffset());
        dsc.second->setTriggerMask(pA->triggerMask());
        return Descriptor::write(m_connection, dsc.first, dsc.first->totalSize())
          ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::FAILURE;
}
