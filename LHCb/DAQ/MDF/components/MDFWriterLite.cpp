// $Id: MDFWriterLite.cpp,v 1.3 2010-05-19 06:48:45 kakiba Exp $
//  ====================================================================
//  MDFWriterLite.cpp
//  --------------------------------------------------------------------
//
//  Author    : Sai Suman & Niko Neufeld 
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/MDFWriterLite.h"

#include <ctime>
#include <clocale>

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterLite)

using namespace std;
using namespace LHCb;
using namespace Gaudi;

/// Standard algorithm constructor
MDFWriterLite::MDFWriterLite(const string& nam, ISvcLocator* pSvc)
  : MDFWriter(nam, pSvc), m_lastOpen(0), m_eventsWritten(0), m_lastEventWritten(0)
{
  declareProperty("MaxFileSizeKB",  m_maxFileSizeKB=2000000);
  declareProperty("MaxFileEvents",  m_maxFileEvents=1000000);
  declareProperty("TimeOut",        m_timeOut = 10); // this is mutually exclusive with file-size	
}

MDFWriterLite::~MDFWriterLite()   {}

/// Transform file name in presence of catalogs
std::string MDFWriterLite::getConnection(const std::string& org_conn)  {
  char dateStr[40];
  ::setlocale(LC_ALL, "");
  m_lastOpen = ::time(0);
  struct tm *currtime = ::localtime(&m_lastOpen);
  ::strftime (dateStr,sizeof(dateStr), "%EY.%0m.%0d-%0H.%0M.%0S.mdf",currtime);
  string con = org_conn+dateStr;
  
  MsgStream log(msgSvc(), name());
  log << MSG::ALWAYS << "New file name:" << con << endmsg;
  return con;
}

/// Execute procedure
StatusCode MDFWriterLite::execute()    {
  time_t now;
  now = ::time(0);
  bool do_open;

  if (m_timeOut == 0) {  
    do_open = (size_t(m_bytesWritten/1024) > m_maxFileSizeKB) || (m_eventsWritten >= m_maxFileEvents);
    do_open &= (now-m_lastOpen)>1;
  } else {
    do_open = (now > m_lastEventWritten + m_timeOut);
  }	
  if ( do_open ) { 
    MsgStream log(msgSvc(), name());
    string con = getConnection(m_connectParams);
    m_ioMgr->disconnect(m_connection).ignore();
    m_connection = new RawDataConnection(this,con);
    StatusCode status = m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
    status.ignore();
    
    if ( m_connection->isConnected() ) {
      log << MSG::INFO << "Received event request connection." << endmsg;
    }
    else {
      log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
      return StatusCode::FAILURE;
    }
    m_bytesWritten = m_eventsWritten = 0;
  }
  m_lastEventWritten = now;
  m_eventsWritten++;
  return MDFWriter::execute();
}

StatusCode MDFWriterLite::initialize() {
  MsgStream log(msgSvc(), name());
  m_lastEventWritten = ::time(NULL);
  return MDFWriter::initialize();
}		
