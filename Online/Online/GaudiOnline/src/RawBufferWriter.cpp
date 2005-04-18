// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawBufferWriter.cpp,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
//	====================================================================
//  RawBufferWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "Event/RawBuffer.h"

using GaudiOnline::StreamDescriptor;

/**@class RawBufferWriter 
  *
  *
  * @author:  M.Frank
  * @version: 1.0
  */
class RawBufferWriter : public Algorithm   {

private:
  /// Stream descriptor (Initializes networking)
  StreamDescriptor::Access m_connection;
  StreamDescriptor::Access m_client;
  std::string m_acceptParams;
public:

  /// Standard algorithm constructor
  RawBufferWriter(const std::string& name, ISvcLocator* pSvcLocator)
  :	Algorithm(name, pSvcLocator)
  {
    declareProperty("AcceptParams", m_acceptParams="");
  }
  /// Standard Destructor
  virtual ~RawBufferWriter()     {
  }
  /// Initialize
  virtual StatusCode initialize()   {
    m_connection = StreamDescriptor::bind(m_acceptParams);
    if ( m_connection.ioDesc > 0 )  {
      m_client = StreamDescriptor::accept(m_connection);
      return (m_client.ioDesc > 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  /// Finalize
  virtual StatusCode finalize() {
    StreamDescriptor::close(m_client);
    StreamDescriptor::close(m_connection);
    return StatusCode::SUCCESS;
  }
  /// Execute procedure
  virtual StatusCode execute()    {
    SmartDataPtr<RawBuffer> rawBuffer(eventSvc(),RawBufferLocation::Default);
    if ( rawBuffer )  {
      int len = rawBuffer->currentSize()*sizeof(raw_int);
      if ( StreamDescriptor::write(m_client, &len, sizeof(len)) )  {
        return StreamDescriptor::write(m_client, rawBuffer->buffer(), len) 
          ? StatusCode::SUCCESS
          : StatusCode::FAILURE;
      }
    }
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<RawBufferWriter>  Factory;
const IFactory& RawBufferWriterFactory = Factory;
