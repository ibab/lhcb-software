// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawBufferWriter.cpp,v 1.5 2005-05-11 07:33:00 frankb Exp $
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
#include "Event/RawEvent.h"
#include "Event/L1Event.h"

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
  std::string              m_acceptParams;
  std::string              m_type;
  int                      m_intType;
public:

  /// Standard algorithm constructor
  RawBufferWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator)
  {
    declareProperty("AcceptParams", m_acceptParams="");
    declareProperty("Type",         m_type="DAQ");
  }
  /// Standard Destructor
  virtual ~RawBufferWriter()     {
  }
  /// Initialize
  virtual StatusCode initialize()   {
    m_intType = StreamDescriptor::dataType(m_type);
    if ( m_intType == StreamDescriptor::NONE ) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Cannot work with unknown buffer type!" << endmsg;
      return StatusCode::FAILURE;
    }
    m_connection = StreamDescriptor::bind(m_acceptParams);
    if ( m_connection.ioDesc > 0 )  {
      MsgStream log(msgSvc(), name());
      log << MSG::INFO << "Waiting for connection from:" << m_acceptParams << endmsg;
      m_client = StreamDescriptor::accept(m_connection);
      if ( m_client.ioDesc ) {
        log << MSG::INFO << "Received event request connection." << std::endl;
      }
      else {
        log << MSG::INFO << "FAILED receiving event request connection." << endmsg;        
      }
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
    SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
    if ( evt ) {}    
    switch(m_intType) {
    case StreamDescriptor::DAQ_BUFFER: {
      SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default);
      if ( raw )  {
        int len = raw->bufLen()*sizeof(raw_int);
        if ( StreamDescriptor::write(m_client, &len, sizeof(len)) )  {
          return StreamDescriptor::write(m_client, raw->buffer(), len) 
                 ? StatusCode::SUCCESS : StatusCode::FAILURE;
        }
      }
    }
    break;
    case StreamDescriptor::L1_BUFFER:  {
      SmartDataPtr<L1Event> l1(eventSvc(),L1EventLocation::Default);
      if ( l1 )  {
        int len = l1->bufLen()*sizeof(l1_int);
        if ( StreamDescriptor::write(m_client, &len, sizeof(len)) )  {
          return StreamDescriptor::write(m_client, l1->buffer(), len) 
                  ? StatusCode::SUCCESS : StatusCode::FAILURE;
        }
      }
    }
    break;
    default:
    break;
    }
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Failed to retrieve event buffer object!" << endmsg;
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<RawBufferWriter>  Factory;
const IFactory& RawBufferWriterFactory = Factory;
