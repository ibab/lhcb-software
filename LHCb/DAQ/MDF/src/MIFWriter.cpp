// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MIFWriter.cpp,v 1.5 2006-10-05 16:38:02 frankb Exp $
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
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/StorageTypes.h"
#include "MDF/MIFWriter.h"
#include "MDF/MIFHeader.h"
#include "Event/RawEvent.h"
#include <cerrno>
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MIFWriter)

typedef std::pair<LHCb::MIFHeader*,LHCb::MIFHeader::Event*> MIFDesc;

/// Standard algorithm constructor
LHCb::MIFWriter::MIFWriter(const std::string& nam, ISvcLocator* svc)
: Algorithm(nam, svc)
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
  SmartDataPtr<DataObject> raw(eventSvc(),"/Event/DAQ/RawEvent");
  if ( raw )  {
    IRegistry* pReg = raw->registry();
    if ( pReg )  {
      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(pReg->address());
      if ( pA )  {
        const std::string& fname = pA->par()[0];
        int fid = LHCb::genChecksum(1,fname.c_str(),fname.length()+1);
        char memory[1024];
        FidMap::const_iterator i = m_fidMap.find(fid);
        if ( i == m_fidMap.end() )   {
          MsgStream log(msgSvc(),name());
          m_fidMap.insert(std::make_pair(fid,fname));
          MIFHeader* hdr = MIFHeader::create(memory, fname, fid);
          if ( !Descriptor::write(m_connection, hdr, hdr->totalSize()) )  {
            log << MSG::ERROR << "Failed to write File record for " << fname << endmsg;
            return StatusCode::FAILURE;
          }
          log << MSG::INFO << "Using FileID " << std::hex << std::setw(8) << fid 
              << " for '" << fname << "'" << endmsg;
          i = m_fidMap.find(fid);
        }
        if ( fname != (*i).second )  {
          MsgStream log(msgSvc(),name());
          log << MSG::ERROR << "MDF file name clash! cannot write MIF." << std::endl
            << "FileID:" << std::hex << std::setw(8) << fid << " is hashed by '" << fname 
            << "' and by '" << (*i).second << "'" << endmsg;
          return StatusCode::FAILURE;
        }
        MIFDesc dsc = MIFHeader::create<MIFHeader::Event>(memory, fid, MIFHeader::MIF_EVENT);
        dsc.second->setOffset(pA->fileOffset());
        return Descriptor::write(m_connection, dsc.first, dsc.first->totalSize())
          ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::FAILURE;
}
