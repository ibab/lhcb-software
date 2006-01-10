// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MDFWriter.cpp,v 1.2 2006-01-10 09:43:16 frankb Exp $
//	====================================================================
//  MDFWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriter)

using namespace LHCb;

/// Standard algorithm constructor
LHCb::MDFWriter::MDFWriter(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
  m_data.reserve(1024*64);
  declareProperty("Connection",     m_connectParams="");
  declareProperty("Compress",       m_compress=0);        // File compression
  declareProperty("CreateChecksum", m_genChecksum=0);     // Generate checksum
}

/// Initialize
StatusCode LHCb::MDFWriter::initialize()   {
  MsgStream log(msgSvc(), name());
  m_connection = Descriptor::connect(m_connectParams);
  if ( m_connection.ioDesc > 0 )  {
    log << MSG::INFO << "Received event request connection." << endmsg;
  }
  else {
    log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
  }
  return (m_connection.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Finalize
StatusCode LHCb::MDFWriter::finalize() {
  Descriptor::close(m_connection);
  return StatusCode::SUCCESS;
}

// Execute procedure
StatusCode LHCb::MDFWriter::execute()    {
  int evType = 1;
  int hdrType = 0;
  int trNumber = 0;
  SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
  if ( raw )  {
    static const size_t hdrSize = sizeof(MDFHeader);
    unsigned int trMask[4] = { -1,-1,-1,-1 };
    size_t len = rawEventLength(raw), newlen=len;
    m_data.reserve(len+hdrSize);
    encodeRawBanks(raw, m_data.data(),len);
    if ( m_compress )   {
      m_tmp.reserve(len+hdrSize);
      if ( compressBuffer(m_compress,
                          m_tmp.data()+hdrSize,len,
                          m_data.data()+hdrSize,len,newlen).isSuccess() )  {
        int chk = m_genChecksum ? xorChecksum(m_tmp.data()+hdrSize,newlen) : 0;
        makeMDFHeader(m_tmp.data(), newlen, evType, hdrType, trNumber, trMask, m_compress, chk);
        return Descriptor::write(m_connection,m_tmp.data(),newlen+hdrSize) 
          ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      // Bad compression; file uncompressed buffer
    }
    int chk = m_genChecksum ? xorChecksum(m_data.data(),len) : 0;
    makeMDFHeader(m_data.data(), len, evType, hdrType, trNumber, trMask, m_compress, chk);
    return Descriptor::write(m_connection, m_data.data(), len+hdrSize) 
      ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }
  MsgStream log(msgSvc(), name());
  log << MSG::ERROR << "Failed to retrieve raw event object:" 
      << RawEventLocation::Default << endmsg;
  return StatusCode::FAILURE;
}
